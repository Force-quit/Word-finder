#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "QWordFinder.hpp"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QTranslator translator;
	if (translator.load(QLocale(), "", "", "resources", ".qm"))
	{
		app.installTranslator(&translator);
	}

	QWordFinder w;
	w.show();
	return app.exec();
}