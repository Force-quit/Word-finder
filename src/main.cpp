#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "WordFinder.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QTranslator translator;
	if (translator.load(QLocale(), "", "", "resources", ".qm"))
	{
		app.installTranslator(&translator);
	}

	WordFinder w;
	w.show();
	return app.exec();
}