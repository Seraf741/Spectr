#include "Spctr_data_processing.h"
#include "grafik.h"
#include <QtWidgets/QStyleOption>
#include <QApplication>
#include "data_source.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("windows"));
    //MainWindow w;
    //w.setWindowTitle("Спектральный анализ");
    //setCentralWidget(w);
    //w.show();
    data_source src;
    return a.exec();
}
