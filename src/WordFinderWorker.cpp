#include "WordFinderWorker.h"
#include <QStringList>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QDebug>

WordFinderWorker::WordFinderWorker(quint16 iMaxResults)
	: mMaxResults{ iMaxResults }, mStopSearching{}, mWordList()
{}

void WordFinderWorker::setWordList(QStringList&& iWordList)
{
	mWordList = iWordList;
}

void WordFinderWorker::stopSearching()
{
	mStopSearching = true;
}

void WordFinderWorker::setMaxResults(quint16 iNbResults)
{
	mMaxResults = iNbResults;
}

void WordFinderWorker::findWords(const QString& iPattern)
{
	QStringList wResults;

	if (!iPattern.isEmpty())
	{
		mStopSearching = false;

		for (auto& wWord : mWordList)
		{
			if (mStopSearching)
				return;

			if (wWord.contains(iPattern, Qt::CaseInsensitive))
			{
				wResults.append(wWord);

				if (wResults.size() == mMaxResults)
					break;
			}
		}
	}

	emit wordsFound(wResults);
}