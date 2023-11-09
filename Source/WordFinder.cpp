#include <QDir>
#include "../Headers/WordFinder.h"
#include "../Headers/WordFinderWorker.h"
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QFileDialog>
#include <EQUtilities/EQIntLineEdit.h>
#include <EQUtilities/EQTextValidator.h>
#include <QLineEdit>
#include <QRegularExpression>
#include <QStringList>
#include <QThread>
#include <QComboBox>
#include <QGuiApplication>
#include <QClipBoard>
#include <QIcon>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDirIterator>

QString WordFinder::wordListFolder;
QString WordFinder::defaultWordList;

WordFinder::WordFinder(QWidget* parent)
	: QMainWindow(parent), wordList(), wordFinderWorker(), workerThread(), searchInput(), resultsList()
{
	setupDefaultWordLists();

	wordFinderWorker = new WordFinderWorker(wordList, DEFAULT_NB_RESULTS);

	QVBoxLayout* centralLayout{ new QVBoxLayout };

	QGroupBox* parametersGroupBox{ initParameters() };
	QHBoxLayout* searchLayout{ initSearch() };
	QVBoxLayout* resultsLayout{ initResults() };

	centralLayout->addWidget(parametersGroupBox);
	centralLayout->addLayout(searchLayout);
	centralLayout->addLayout(resultsLayout);
	centralLayout->setAlignment(Qt::AlignTop);

	QWidget* centralWidget{ new QWidget };
	centralWidget->setLayout(centralLayout);
	setCentralWidget(centralWidget);
	setWindowIcon(QIcon(":/images/glass.png"));
}

QGroupBox* WordFinder::initParameters()
{
	QGroupBox* parametersGroupBox{ new QGroupBox("Parameters") };

	QHBoxLayout* wordListLayout{ new QHBoxLayout };
	QLabel* wordListLabel{ new QLabel("Word list :") };
	QLabel* wordListPath{ new QLabel(!defaultWordList.isEmpty() ? defaultWordList : "None") };
	QPushButton* wordListButton{ new QPushButton("Select file") };
	connect(wordListButton, &QPushButton::clicked, [this, wordListPath]() {
		QString filePath = QFileDialog::getOpenFileName(this, "Select word list", defaultWordList, "text files (*.txt)");
		if (!filePath.isEmpty())
		{
			loadWordList(filePath);
			wordListPath->setText(filePath);
			resultsList->clear();
			searchInput->clear();
		}
	});
	wordListLayout->addWidget(wordListLabel);
	wordListLayout->addWidget(wordListPath);
	wordListLayout->addWidget(wordListButton);

	QHBoxLayout* resultNbLayout{ new QHBoxLayout };
	QLabel* resultNbLabel{ new QLabel("Max results :") };
	EQIntLineEdit* resultNbInput{ new EQIntLineEdit(1, 25000) };
	resultNbInput->setText(QString::number(DEFAULT_NB_RESULTS));
	connect(resultNbInput, &EQIntLineEdit::valueChanged, wordFinderWorker, &WordFinderWorker::setMaxResults);
	resultNbLayout->addWidget(resultNbLabel);
	resultNbLayout->addWidget(resultNbInput);

	QVBoxLayout* parametersLayout{ new QVBoxLayout };
	parametersLayout->addLayout(wordListLayout);
	parametersLayout->addLayout(resultNbLayout);
	parametersGroupBox->setLayout(parametersLayout);
	return parametersGroupBox;
}

QHBoxLayout* WordFinder::initSearch()
{
	QHBoxLayout* searchLayout{ new QHBoxLayout };
	QLabel* searchLabel{ new QLabel("Pattern to find :") };

	searchInput = new QLineEdit;
	searchInput->setValidator(new EQTextValidator);
	connect(searchInput, &QLineEdit::textEdited, [this]() {wordFinderWorker->queueWork(); });
	connect(searchInput, &QLineEdit::textEdited, wordFinderWorker, &WordFinderWorker::findWords);
	connect(wordFinderWorker, &WordFinderWorker::wordsFound, [this](const QStringList& result) {
		resultsList->clear();
		resultsList->addItems(result);
		resultsList->update(); // Didn't update by itself
		});
	connect(&workerThread, &QThread::finished, wordFinderWorker, &QObject::deleteLater);

	wordFinderWorker->moveToThread(&workerThread);
	workerThread.start();

	searchLayout->addWidget(searchLabel);
	searchLayout->addWidget(searchInput);
	return searchLayout;
}

QVBoxLayout* WordFinder::initResults()
{
	QVBoxLayout* resultsLayout{ new QVBoxLayout };
	resultsList = new QListWidget;

	QPushButton* resultsButton{ new QPushButton("Copy") };
	connect(resultsButton, &QPushButton::clicked, [this]() {
		if (QListWidgetItem * selectedItem{ resultsList->currentItem() })
			QGuiApplication::clipboard()->setText(selectedItem->text());
		});

	resultsLayout->addWidget(resultsList);
	resultsLayout->addWidget(resultsButton);
	return resultsLayout;
}

void WordFinder::loadWordList(const QString& filePath)
{
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		wordList.clear();
		QTextStream in{ &file };
		while (!in.atEnd())
			wordList.append(in.readLine());
		wordList.squeeze();
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("File error");
		msgBox.setInformativeText("Error reading file" + filePath);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
	}
	file.close();
}

void WordFinder::setupDefaultWordLists()
{
	QString userDocumentsPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

	if (!userDocumentsPath.isEmpty())
	{
		wordListFolder = userDocumentsPath + "/WordFinder/";
		defaultWordList = wordListFolder + "francais.txt";
		
		QDir().mkdir(wordListFolder);

		QDirIterator it(":/word-lists/", QDirIterator::Subdirectories);
		while (it.hasNext()) 
		{
			QString qResourceWordList = it.next();
			QString fileNameOnComputer(wordListFolder + qResourceWordList.split("/").last());

			if (!QFile::exists(fileNameOnComputer))
			{
				QFile qResourceWordListFile(qResourceWordList);
				qResourceWordListFile.copy(fileNameOnComputer);
			}
		}

		loadWordList(defaultWordList);
	}
}

WordFinder::~WordFinder()
{
	workerThread.quit();
	workerThread.wait();
}