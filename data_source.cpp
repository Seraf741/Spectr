#include "data_source.h"
#include <random>
#include "qelapsedtimer.h"
#include "QRandomGenerator"
#include "QElapsedTimer"


using namespace std;


random_device rd;
mt19937 gen(rd());


data_source::data_source(QWidget *parent) : QWidget(parent)
{
    main_wnd = new MainWindow(this);
    //main_wnd->show();
    tworkParamstruct twork_param;
    tworkstruct twork;
    twork_param.dugol = 2;
    twork_param.ugol = 90;
    twork_param.ugolz = 0;
    twork_param.GrafCountAniz = 3;
    twork.ref = 2.453;
    twork_param.fnreg = new float[1]; twork_param.fnreg[0] = 1;
    twork_param.fvreg = new float[1]; twork_param.fvreg[0] = 1000;
    twork_param.timl = 8.94;
    twork.namedevvp = new QString("кгп ");
    twork.numdevvp = 25;
    twork.dat = "12.10.2023";
    twork.tim = "13:47";
    twork.pfilefch = new QStringList("/COMPAS/");
    twork_param.n = 2048;
    twork.fd = 1600;

    connect(this, SIGNAL(chartDataChanged(float**)),main_wnd,SLOT(transferChartData(float**)));
    connect(this, SIGNAL(dataForAnizIsReady(tworkParamstruct, tworkstruct)), main_wnd, SLOT(processingSpectrData(tworkParamstruct, tworkstruct)));
    data = new float *[grfs_count];
    for(int i = 0; i < grfs_count; i++)
    {
        data[i] = new float [arr_size];
    }
    emit dataForAnizIsReady(twork_param, twork);
    for(int i = 0; i < 40; i++)
    {

        for(int j = 0; j < grfs_count; j++)
        {
            generateSomeRandData(i * 2, j);
        }
        emit chartDataChanged(data);
        QElapsedTimer timer;
        timer.start();
        while(true)
        {
            qApp->processEvents();
            if(timer.elapsed() > 600){break;}
        }
    }
}

data_source::~data_source()
{
    for(int i = 0; i < grfs_count; i++)
    {
        delete data[i];
    }
    delete[] data;
}

int sign = 1;
int step = 0;
void data_source::generateSomeRandData(int peak, int row)
{
    uniform_int_distribution<> dist(35, 45);

    for(int i = 0; i < arr_size; i++)
    {
        int val = dist(gen);
        data[row][i] = val;
    }

//    QString str1;
//    for(int i = 1; i <= 180; i++)
//    {
//        str1 += QString::number(data[i - 1]) + "  ";
//        if(i % 10 == 0)
//        {
//            qDebug()<<str1;
//            str1 = "";
//        }
//    }


    if(step == 15)
    {
        sign = -10;
    }
    else  if (step == 0)
    {
        sign = 10;
    }
    step += 1 * sign;
    qDebug()<<step<<"   +++++++++++++++++++++++++++++++++++++++++++";

    for(int i = 0; i < arr_size; i++)
    {
        int val = QRandomGenerator::global()->bounded(0.025 + step / 1000, 750 + step * 2);
        if(i == peak)
        {
            val = 100000 + abs(step * 10);
        }
        data[row][i] = val;
    }

//    qDebug()<<"------------------------";
//    str1 = "";
//    for(int i = 1; i <= 180; i++)
//    {
//        str1 += QString::number(data[i - 1]) + "  ";
//        if(i % 10 == 0)
//        {
//            qDebug()<<str1;
//            str1 = "";
//        }
//    }
//    qDebug()<<"------------------------";
}
