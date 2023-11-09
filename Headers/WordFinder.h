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
	constexpr static unsigned int DEFAULT_NB_RESULTS{ 25 };

	static QString wordListFolder;
	static QString defaultWordList;

	QStringList wordList;

	QLineEdit* searchInput;
	QListWidget* resultsList;

	QGroupBox* initParameters();
	QHBoxLayout* initSearch();
	QVBoxLayout* initResults();

	WordFinderWorker* wordFinderWorker;
	QThread workerThread;

	void loadWordList(const QString& filePath);
	void setupDefaultWordLists();
};