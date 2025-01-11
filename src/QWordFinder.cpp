#include "QWordFinder.hpp"
#include "QWordFinderWorker.hpp"
#include <QDir>
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QStringLiteral>
#include <QFileDialog>
#include <QFileInfo>
#include <EQIntLineEdit.hpp>
#include <EQTextValidator.hpp>
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

QWordFinder::QWordFinder()
{
	auto* centralWidget{ new QWidget };
	setCentralWidget(centralWidget);

	auto* centralLayout{ new QVBoxLayout };
	centralWidget->setLayout(centralLayout);

	QGroupBox* parameters{ initParameters() };
	centralLayout->addWidget(parameters);

	QHBoxLayout* searchLayout{ initSearch() };
	centralLayout->addLayout(searchLayout);

	QVBoxLayout* resultsLayout{ initResults() };
	centralLayout->addLayout(resultsLayout);

	setWindowIcon(QIcon(":/images/icon.png"));
	loadDefaultWordList();
}

QGroupBox* QWordFinder::initParameters()
{
	auto* groupBox{ new QGroupBox(tr("Parameters")) };

	auto* groupBoxLayout{ new QVBoxLayout };
	groupBox->setLayout(groupBoxLayout);

	auto* wordListLayout{ new QHBoxLayout };
	groupBoxLayout->addLayout(wordListLayout);

	auto* wordListLabel{ new QLabel(tr("Word list :")) };
	wordListLayout->addWidget(wordListLabel);

	mWordListPathLabel = new QLabel(tr("None"));
	wordListLayout->addWidget(mWordListPathLabel);

	auto* wordListSelectButton{ new QPushButton(tr("Select file")) };
	wordListLayout->addWidget(wordListSelectButton);
	connect(wordListSelectButton, &QPushButton::clicked, this, &QWordFinder::selectNewWordList);

	auto* maxResultsLayout{ new QHBoxLayout };
	groupBoxLayout->addLayout(maxResultsLayout);

	auto* maxResultsLabel{ new QLabel(tr("Max results :")) };
	maxResultsLayout->addWidget(maxResultsLabel);

	auto* maxResultsLineEdit{ new EQIntLineEdit(1, MAX_NB_RESULTS, DEFAULT_NB_RESULTS) };
	maxResultsLayout->addWidget(maxResultsLineEdit);
	connect(maxResultsLineEdit, &EQIntLineEdit::valueChanged, mWordFinderWorker, &QWordFinderWorker::setMaxResults);

	return groupBox;
}

QHBoxLayout* QWordFinder::initSearch()
{
	auto* layout{ new QHBoxLayout };

	auto* label{ new QLabel(tr("Pattern to find :")) };
	layout->addWidget(label);

	mSearchLineEdit = new QLineEdit;
	layout->addWidget(mSearchLineEdit);
	mSearchLineEdit->setValidator(new EQTextValidator);

	// Unavoidable lambda function.. There's a queue of called slots in mWorkerThread
	// and we cannot trigger a slot when another slot is still executing.
	// We have to explicitly call QWordFinderWorker::stopSearching
	// so that it executes now.
	connect(mSearchLineEdit, &QLineEdit::textEdited, [this]() {mWordFinderWorker->stopSearching(); });
	connect(mSearchLineEdit, &QLineEdit::textEdited, mWordFinderWorker, &QWordFinderWorker::findWords);
	connect(mWordFinderWorker, &QWordFinderWorker::wordsFound, this, &QWordFinder::setFoundWords);
	connect(&mWorkerThread, &QThread::finished, mWordFinderWorker, &QObject::deleteLater);

	mWordFinderWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();

	return layout;
}

QVBoxLayout* QWordFinder::initResults()
{
	auto* layout{ new QVBoxLayout };
	
	mWordListWidget = new QListWidget;
	layout->addWidget(mWordListWidget);

	auto* copyButton{ new QPushButton(tr("Copy")) };
	layout->addWidget(copyButton);
	copyButton->setMinimumHeight(40);
	connect(copyButton, &QPushButton::clicked, this, &QWordFinder::copySelectedWord);

	return layout;
}

void QWordFinder::loadWordList(const QString& iFilePath)
{
	QFile wordListFile(iFilePath);
	if (wordListFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QStringList wordList;
		QTextStream textStream{ &wordListFile };
		while (!textStream.atEnd())
		{
			wordList.append(textStream.readLine());
		}
		mWordFinderWorker->setWordList(std::move(wordList));
		mLastWordListPath = iFilePath;
		mWordListPathLabel->setText(iFilePath);
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText(tr("File error"));
		msgBox.setInformativeText(tr("Error reading file") + " " + iFilePath);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
	}
}

void QWordFinder::loadDefaultWordList()
{
	if (!loadWordListFromDocuments())
	{
		QString wordListFolder(QStringLiteral("resources/word-lists/"));
		QDirIterator dirIterator(wordListFolder);
		while (dirIterator.hasNext())
		{
			QFileInfo wordListFileInfo(dirIterator.next());
			if (wordListFileInfo.isFile())
			{
				loadWordList(wordListFileInfo.absoluteFilePath());
			}
		}
	}
}

bool QWordFinder::loadWordListFromDocuments()
{
	const QString userDocumentsPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
	QString wordListFolder(userDocumentsPath + "/Word-finder/");
	if (wordListFolder.isEmpty())
	{
		return false;
	}

	QDirIterator dirIterator(wordListFolder, QDirIterator::Subdirectories);
	while (dirIterator.hasNext())
	{
		QFileInfo wordListFileInfo(dirIterator.next());
		if (wordListFileInfo.isFile())
		{
			loadWordList(wordListFileInfo.absoluteFilePath());
			if (!mLastWordListPath.isEmpty())
			{
				return true;
			}
		}
	}

	return false;
}

void QWordFinder::selectNewWordList()
{
	QString wFilePath = QFileDialog::getOpenFileName(this, tr("Select word list"), mLastWordListPath, tr("Text files (*.txt)"));
	if (!wFilePath.isEmpty())
	{
		loadWordList(wFilePath);
		mWordListWidget->clear();
		mSearchLineEdit->clear();
	}
}

void QWordFinder::copySelectedWord()
{
	if (QListWidgetItem* selectedItem{ mWordListWidget->currentItem() })
	{
		QGuiApplication::clipboard()->setText(selectedItem->text());
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setWindowIcon(windowIcon());
		msgBox.setInformativeText(tr("Please select a word to copy."));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}

void QWordFinder::setFoundWords(const QStringList& iWords)
{
	mWordListWidget->clear();
	mWordListWidget->addItems(iWords);
}

QWordFinder::~QWordFinder()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}