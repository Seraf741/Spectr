#include "mainwindow.h"
#include "grafik.h"
#include <QtWidgets/QStyleOption>
#include "QElapsedTimer"

QElapsedTimer timer;

int main(int argc, char *argv[])
{
    timer.start();
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("windows"));
    MainWindow w;
    //grafik h;
    w.setWindowTitle("Спектральный анализ");
    //setCentralWidget(w);
    //w.show();
    //h.show();
    //qDebug()<<"time = "<<timer.elapsed();
    return a.exec();
}
