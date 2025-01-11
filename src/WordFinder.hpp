#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QLineEdit>
#include <QListWidget>
#include <QGroupBox>
#include <QBoxLayout>
#include <QLabel>
#include "WordFinderWorker.hpp"
#include <QThread>

class WordFinder : public QMainWindow
{
	Q_OBJECT

public:
	WordFinder(QWidget* iParent = nullptr);
	~WordFinder();

private slots:
	void wordsFound(const QStringList& iWords);
	void selectNewWordList();
	void copySelectedWord();

private:
	void loadWordList(const QString& iFilePath);
	void setupDefaultWordLists();

	constexpr static quint16 DEFAULT_NB_RESULTS{ 25 };
	constexpr static quint16 MAX_NB_RESULTS{ 0xFFFF };

	inline static QString mWordListFolder;
	inline static QString mDefaultWordListPath;

	QLineEdit* mSearchInput;
	QListWidget* mWordListWidget;
	QLabel* mWordListPathLabel;

	QGroupBox* initParameters();
	QHBoxLayout* initSearch();
	QVBoxLayout* initResults();

	WordFinderWorker* mWordFinderWorker;
	QThread mWorkerThread;
};