#include <QStyleFactory>
#include <QApplication>
#include "../Headers/WordFinder.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    WordFinder w;
    w.show();
    return a.exec();
}