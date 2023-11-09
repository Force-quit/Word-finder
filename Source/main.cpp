#include <QStyleFactory>
#include <QLocale>
#include <QTranslator>
#include <QApplication>
#include "../Headers/WordFinder.h"


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QTranslator translator;
	if (translator.load(QLocale(), "main", ".", ":/translations", ".qm"))
	{
		a.installTranslator(&translator);
	}
	WordFinder w;
	w.show();
	return a.exec();
}