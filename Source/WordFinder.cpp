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

WordFinder::WordFinder(QWidget* parent)
	: QMainWindow(parent), wordList(), wordFinderWorker(), workerThread(), searchInput(), resultsList(),
	APP_PATH(QCoreApplication::applicationDirPath())
{
	QDir().mkdir(APP_PATH + '/' + WORD_LIST_FOLDER);
	if (QFile::exists(APP_PATH + '/' + DEFAULT_WORD_LIST))
		loadWordList(APP_PATH + '/' + DEFAULT_WORD_LIST);

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
	initWindow();
}

QGroupBox* WordFinder::initParameters()
{
	QGroupBox* parametersGroupBox{ new QGroupBox("Parameters") };

	QHBoxLayout* wordListLayout{ new QHBoxLayout };
	QLabel* wordListLabel{ new QLabel("Word list :") };
	QLabel* wordListPath{ new QLabel(wordList.size() > 0 ? DEFAULT_WORD_LIST : "None") };
	QPushButton* wordListButton{ new QPushButton("Select file") };
	connect(wordListButton, &QPushButton::clicked, [this, wordListPath]() {
		QString filePath = QFileDialog::getOpenFileName(this, "Select word list", DEFAULT_WORD_LIST, "text files (*.txt)");
		if (!filePath.isEmpty())
		{
			loadWordList(filePath);
			wordListPath->setText(QDir().relativeFilePath(filePath));
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

void WordFinder::initWindow()
{
	resize(minimumSizeHint());
	setWindowIcon(QIcon(APP_PATH + "/glass.png"));
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

WordFinder::~WordFinder()
{
	workerThread.quit();
	workerThread.wait();
}