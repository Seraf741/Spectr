#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H
#include <QWidget>
#include "Spctr_data_processing.h"

class data_source : public QWidget
{
    Q_OBJECT

public:
    data_source(QWidget *parent = 0);
    ~data_source();


signals:
    void chartDataChanged(float**);
    void dataForAnizIsReady(tworkParamstruct, tworkstruct);

private slots:
    void generateSomeRandData(int, int);

private:
    MainWindow *main_wnd;
    int grfs_count = 4;
    int arr_size = 2048;
    float  **data;
};

#endif // DATA_SOURCE_H
