#include "QWordFinderWorker.hpp"
#include <QStringList>
#include <QString>

QWordFinderWorker::QWordFinderWorker(quint16 iMaxResults)
	: mMaxResults{ iMaxResults }
{

}

void QWordFinderWorker::setWordList(QStringList&& iWordList)
{
	mWordList = iWordList;
}

void QWordFinderWorker::stopSearching()
{
	mStopSearching = true;
}

void QWordFinderWorker::setMaxResults(quint16 iMaxResults)
{
	mMaxResults = iMaxResults;
}

void QWordFinderWorker::findWords(const QString& iPattern)
{
	if (iPattern.isEmpty())
	{
		return;
	}

	QStringList foundWords;
	mStopSearching = false;

	for (const QString& word : mWordList)
	{
		if (mStopSearching)
		{
			return;
		}

		if (word.contains(iPattern, Qt::CaseInsensitive))
		{
			foundWords.append(word);

			if (foundWords.size() == mMaxResults)
			{
				break;
			}
		}
	}

	emit wordsFound(foundWords);
}