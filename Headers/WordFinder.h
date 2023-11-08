#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QLineEdit>
#include <QListWidget>
#include <QGroupBox>
#include <QBoxLayout>
#include "WordFinderWorker.h"
#include <QThread>

class WordFinder : public QMainWindow
{
	Q_OBJECT

public:
	WordFinder(QWidget *parent = nullptr);
	~WordFinder();

private:
	const unsigned int DEFAULT_NB_RESULTS{ 25 };
	const QString APP_PATH;
	inline static const QString WORD_LIST_FOLDER{ "Word-lists" };
	inline static const QString DEFAULT_WORD_LIST{ WORD_LIST_FOLDER + '/' + "francais.txt"};

	QStringList wordList;

	QLineEdit* searchInput;
	QListWidget* resultsList;

	QGroupBox* initParameters();
	QHBoxLayout* initSearch();
	QVBoxLayout* initResults();

	WordFinderWorker* wordFinderWorker;
	QThread workerThread;

	void loadWordList(const QString& filePath);
};