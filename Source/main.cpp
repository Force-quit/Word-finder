#include <QStyleFactory>
#include <QLocale>
#include <QTranslator>
#include <QApplication>
#include "../Headers/WordFinder.h"


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QApplication::setStyle(QStyleFactory::create("Fusion"));

	QTranslator translator;
	if (translator.load(QLocale(), "", "", ":/translations", ".qm"))
		app.installTranslator(&translator);

	WordFinder w;
	w.show();
	return app.exec();
}