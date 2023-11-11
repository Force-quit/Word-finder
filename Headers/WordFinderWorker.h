#pragma once

#include <QObject>
#include <QStringList>
#include <QString>

class WordFinderWorker : public QObject
{
	Q_OBJECT

public:
	WordFinderWorker(quint16 mMaxResults);
	void setWordList(QStringList&& iWordList);

public slots:
	void stopSearching();
	void findWords(const QString& pattern);
	void setMaxResults(quint16 nbResults);

signals:
	void wordsFound(const QStringList& results);

private:
	QStringList mWordList;
	quint16 mMaxResults;
	bool mStopSearching;
};
