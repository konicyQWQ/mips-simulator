#include "mips.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mips w;
    w.show();
    return a.exec();
}
