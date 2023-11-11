#include "../Headers/WordFinder.h"
#include "../Headers/WordFinderWorker.h"
#include <QDir>
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QFileDialog>
#include <EQUtilities/EQIntLineEdit.h>
#include <EQUtilities/EQTextValidator.h>
#include <QLineEdit>
#include <QStringList>
#include <QThread>
#include <QGuiApplication>
#include <QClipboard>
#include <QIcon>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDirIterator>

WordFinder::WordFinder(QWidget* iParent)
	: QMainWindow(iParent), mWorkerThread(), mWordFinderWorker{ new WordFinderWorker(DEFAULT_NB_RESULTS) }
{
	setupDefaultWordLists();

	QWidget* wCentralWidget{ new QWidget };

	QVBoxLayout* wCentralLayout{ new QVBoxLayout };
	wCentralLayout->setAlignment(Qt::AlignTop);
	QGroupBox* wParametersGroupBox{ initParameters() };
	wCentralLayout->addWidget(wParametersGroupBox);
	QHBoxLayout* wSearchLayout{ initSearch() };
	wCentralLayout->addLayout(wSearchLayout);
	QVBoxLayout* wResultsLayout{ initResults() };
	wCentralLayout->addLayout(wResultsLayout);

	wCentralWidget->setLayout(wCentralLayout);
	setCentralWidget(wCentralWidget);
	setWindowIcon(QIcon(":/images/glass.png"));
}

QGroupBox* WordFinder::initParameters()
{
	QGroupBox* wParametersGroupBox{ new QGroupBox(tr("Parameters")) };
	QVBoxLayout* wParametersLayout{ new QVBoxLayout };

	QHBoxLayout* wWordListLayout{ new QHBoxLayout };
	QLabel* wWordListLabel{ new QLabel(tr("Word list :")) };
	wWordListLayout->addWidget(wWordListLabel);
	mWordListPathLabel = new QLabel(!mDefaultWordListPath.isEmpty() ? mDefaultWordListPath : tr("None"));
	wWordListLayout->addWidget(mWordListPathLabel);
	QPushButton* wWordListButton{ new QPushButton(tr("Select file")) };
	wWordListLayout->addWidget(wWordListButton);
	wParametersLayout->addLayout(wWordListLayout);

	QHBoxLayout* wResultNbLayout{ new QHBoxLayout };
	QLabel* wResultNbLabel{ new QLabel(tr("Max results :")) };
	wResultNbLayout->addWidget(wResultNbLabel);
	EQIntLineEdit* wResultNbLineEdit{ new EQIntLineEdit(1, MAX_NB_RESULTS) };
	wResultNbLineEdit->setText(QString::number(DEFAULT_NB_RESULTS));
	wResultNbLayout->addWidget(wResultNbLineEdit);
	wParametersLayout->addLayout(wResultNbLayout);

	connect(wWordListButton, &QPushButton::clicked, this, &WordFinder::selectNewWordList);
	connect(wResultNbLineEdit, &EQIntLineEdit::valueChanged, mWordFinderWorker, &WordFinderWorker::setMaxResults);

	wParametersGroupBox->setLayout(wParametersLayout);
	return wParametersGroupBox;
}

void WordFinder::wordsFound(const QStringList& iWords)
{
	mWordListWidget->clear();
	mWordListWidget->addItems(iWords);
}

QHBoxLayout* WordFinder::initSearch()
{
	QHBoxLayout* wSearchLayout{ new QHBoxLayout };

	QLabel* wSearchLabel{ new QLabel(tr("Pattern to find :")) };
	wSearchLayout->addWidget(wSearchLabel);

	mSearchInput = new QLineEdit;
	mSearchInput->setValidator(new EQTextValidator);
	wSearchLayout->addWidget(mSearchInput);

	// Unavoidable lambda function.. There's a queue of called slots in mWorkerThread
	// and we cannot trigger a slot when another slot is still executing.
	// We have to explicitly call WordFinderWorker::stopSearching
	// so that it executes now.
	connect(mSearchInput, &QLineEdit::textEdited, [this]() {mWordFinderWorker->stopSearching(); });
	connect(mSearchInput, &QLineEdit::textEdited, mWordFinderWorker, &WordFinderWorker::findWords);
	connect(mWordFinderWorker, &WordFinderWorker::wordsFound, this, &WordFinder::wordsFound);
	connect(&mWorkerThread, &QThread::finished, mWordFinderWorker, &QObject::deleteLater);

	mWordFinderWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();

	return wSearchLayout;
}

QVBoxLayout* WordFinder::initResults()
{
	QVBoxLayout* wResultsLayout{ new QVBoxLayout };
	
	mWordListWidget = new QListWidget;
	wResultsLayout->addWidget(mWordListWidget);

	QPushButton* wResultsButton{ new QPushButton(tr("Copy")) };
	wResultsLayout->addWidget(wResultsButton);
	
	connect(wResultsButton, &QPushButton::clicked, this, &WordFinder::copySelectedWord);

	return wResultsLayout;
}

void WordFinder::loadWordList(const QString& iFilePath)
{
	QFile wFile(iFilePath);
	if (wFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QStringList wStringList;
		QTextStream wInputStream{ &wFile };
		while (!wInputStream.atEnd())
			wStringList.append(wInputStream.readLine());
		wStringList.squeeze();
		mWordFinderWorker->setWordList(std::move(wStringList));
	}
	else
	{
		QMessageBox wMsgBox;
		wMsgBox.setText(tr("File error"));
		wMsgBox.setInformativeText(tr("Error reading file") + iFilePath);
		wMsgBox.setStandardButtons(QMessageBox::Ok);
		wMsgBox.setDefaultButton(QMessageBox::Ok);
		wMsgBox.exec();
	}
	wFile.close();
}

void WordFinder::setupDefaultWordLists()
{
	QString wUserDocumentsPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

	if (!wUserDocumentsPath.isEmpty())
	{
		mWordListFolder = wUserDocumentsPath + "/WordFinder/";
		QDir().mkdir(mWordListFolder);

		QDirIterator wDirIterator(":/word-lists/", QDirIterator::Subdirectories);
		while (wDirIterator.hasNext())
		{
			QString wWordListPathInResources = wDirIterator.next();
			QString wWordListPath(mWordListFolder + wWordListPathInResources.split("/").last());

			if (!QFile::exists(wWordListPath))
			{
				QFile wWordListFile(wWordListPathInResources);
				wWordListFile.copy(wWordListPath);
			}
		}

		mDefaultWordListPath = mWordListFolder + "francais.txt";
		loadWordList(mDefaultWordListPath);
	}
}

void WordFinder::selectNewWordList()
{
	QString wFilePath = QFileDialog::getOpenFileName(this, tr("Select word list"), mDefaultWordListPath, tr("text files (*.txt)"));
	if (!wFilePath.isEmpty())
	{
		loadWordList(wFilePath);
		mWordListPathLabel->setText(wFilePath);
		mWordListWidget->clear();
		mSearchInput->clear();
	}
}

void WordFinder::copySelectedWord()
{
	QListWidgetItem* wSelectedItem{ mWordListWidget->currentItem() };

	if (wSelectedItem)
		QGuiApplication::clipboard()->setText(wSelectedItem->text());
}

WordFinder::~WordFinder()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}