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
#include "QWordFinderWorker.hpp"
#include <QThread>

class QWordFinder : public QMainWindow
{
	Q_OBJECT

public:
	QWordFinder();
	~QWordFinder();

private slots:
	void setFoundWords(const QStringList& iWords);
	void selectNewWordList();
	void copySelectedWord();

private:
	constexpr static quint16 MAX_NB_RESULTS{ 5'000 };
	constexpr static quint16 DEFAULT_NB_RESULTS{ 100 };

	void loadWordList(const QString& iFilePath);
	
	void loadDefaultWordList();
	bool loadWordListFromDocuments();

	QGroupBox* initParameters();
	QHBoxLayout* initSearch();
	QVBoxLayout* initResults();

	QString mLastWordListPath;
	QLineEdit* mSearchLineEdit{};
	QListWidget* mWordListWidget{};
	QLabel* mWordListPathLabel{};
	QWordFinderWorker* mWordFinderWorker{ new QWordFinderWorker(DEFAULT_NB_RESULTS) };
	QThread mWorkerThread;
};