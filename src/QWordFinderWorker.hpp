#pragma once

#include <QObject>
#include <QStringList>
#include <QString>

class QWordFinderWorker : public QObject
{
	Q_OBJECT

public:
	QWordFinderWorker(quint16 iMaxResults);
	void setWordList(QStringList&& iWordList);

public slots:
	void stopSearching();
	void findWords(const QString& iPattern);
	void setMaxResults(quint16 iMaxResults);

signals:
	void wordsFound(const QStringList& iResults);

private:
	QStringList mWordList;
	bool mStopSearching{};
	quint16 mMaxResults;
};
