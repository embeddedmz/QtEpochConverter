#include "epochconverter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EpochConverter w;
    w.show();

    return a.exec();
}
