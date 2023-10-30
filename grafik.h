#ifndef GRAFIK_H
#define GRAFIK_H

#include <QDialog>
#include "mainwindow.h"
#include <QSpinBox>
#include "QStandardItemModel"
#include "qcustomplot.h"

namespace Ui {
class grafik;
}

class grafik : public QDialog
{
    Q_OBJECT

public:
    explicit grafik(QWidget *parent = nullptr);
    //~grafik();

private:

//    class MyItemText : public QCPItemText
//    {
//    public:
//        MyItemText(QCustomPlot *parentplot) : QCPItemText(parentplot)
//        {
//            but = new QPushButton("X",parentplot);
//            but->setFixedSize(100,100);
//        }
//    private:
//        QPushButton *but;
//    //private slots:
//    };
    Ui::grafik *ui;
    MainWindow::tPolar *tpolar;                                                                                                                             //  - структура с данными для построения графика
    Qt::GlobalColor color_grf[7]{ Qt::red,Qt::black,Qt::darkMagenta,Qt::blue,Qt::green,Qt::darkRed,Qt::darkGray};                                            //  - массив цветов для графиков
    QColor color_inform_window_background[7]{QColor(255,148,148),QColor(169,169,169),QColor(209,142,209),QColor(127,179,255),QColor(179,255,179),QColor(204,102,102),QColor(190,190,190)};
    QStandardItemModel *model_for_selecting_grafs;                                                                      //  - встраиваемая модель, содержащая чекбоксы для вкл/откл графиков, в выпадающий список
    QStandardItemModel *model_for_displaying_info;
    QStandardItemModel *model_for_sel_grfs_to_show_coord;                                                             //  - модель, содержащая список графиков, для выбора графика для получения и отображения координат его точек
    QCPCurve **border_lines = new QCPCurve *[2]{};                                                          //  - массив для создания 2-х линий масштабирования, задающих границы масштабируемой области
    QCPCurve *vertline = nullptr;
    QCPItemTracer *tracer;                                               //  - визуально показывает ближайшую точку графика относительно vertline
    QVector<double> data_peaks;                                         //  - хранит значения пиков графика
    QVector<double> data_peaks_pos;                                    //  - хранит позиции пиков графика
    QList<QCPItemText*> text_items_lst;                                 //  - список для хранения информационных табло с координатами в режиме отображения координат
    QList<QCPItemLine*> line_items_lst;                                //  - список для хранения линий, соединяющих точку с информационным окном, отображающим координаты этой точки, в режиме отображения координат
    float **data_grafs;                                               //  - хранит значения координат точек графиков
    int *order_of_grafs;                                            //  - хранит номера графиков, которые должны отображаться на координатной плоскости
    int quantity_of_grafs;                                             //  - размер массива с номерами графиков
    int border_line_num = 0;                                         //  - номер линии масштабирования
    double segment_borders[2]{0,0};                                 //  - содержит координаты границ выделенного линиями участка масштабирования
    int del_me_num = 0;               //delete it
    bool first_zoom = true;                                         //  - флаг для отслеживания первого приближения
    bool reset = true;                                             // - флаг для отслеживания отката графиков к начальному состоянию
    bool scale_changed = false;                                   //  - флаг для отслеживания изменения масштаба графика (нужно добавить еще 1 флаг, чтобы можно было отслеживать изменение масштаба по каждой оси в отдельности)

signals:
    void closeGrafik();

private slots:
    void ShowGraf();                                            // функция постройки и отображения графиков
    void receiveDataOfGrafik(MainWindow::tPolar &tpolar);
    void SetGrafList(QStringList grfs_names, int &ngrf);                            //  - функция создает модель, ее элементы (чекбоксы) и добавляет в выпадающий список
    void SetGrafInfo(QStringList info_of_grafs, int ngrf);                         //  - функция создает модель, содержащую информацию о графиках (отображается в верхнем левом углу программы)
    void SetGrafListForShowCoord(QStringList grfs_names, int &ngrf);
    void ChangeGrafsForShow(const QModelIndex& numgrf, const QModelIndex& unused);      //  - функция для изменеия массива с номерами графиков для отображения
    void ChangeNumberOfDisplayedGrafs();                                            //  - функция для изменения количества отображаемых графиков в выпадающем списке в режиме отображения координат точек графика
    void ChangeDisplayedGraf(int index_selected_graf);                       //  - функция для смены текущего графика на выбранный в режиме отображения кооординат
    void on_ZoomOnX_stateChanged(int arg1);                                 //  - вызывается при изменении состояния чекбокса ZoomOnX
    void on_ZoomOnY_stateChanged(int arg1);                                //  - вызывается при изменении состояния чекбокса ZoomOnX
    void disableZoom();                                                   //  - вызывается при отключении функции масштабирования по осям
    void SlotMouseMove(QMouseEvent *event);                               //  - вызывается при изенении положения курсора мыши
    void SlotMouseMoveToShowGrafValue(QMouseEvent *event);
    void tracerMoveToShowGrafValue(double coordx);                         //  - функция принимает координаты и смещает согласно им линию, отображающую выбранную точку на графике
    void SlotMouseMoveToDragInfoWindow(QMouseEvent *event);
    void SlotMousePress(QMouseEvent *event);                             //  - вызывается при нажатии на клавишу мыши
    void SlotMousePressToShowGrafValue(QMouseEvent *event);
    void SlotMousePressToDragInfoWindow(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event) override;
    void on_DispCoordValue_stateChanged(int index_of_graf);
    void closeEvent(QCloseEvent *event) override;
    void on_Reset_clicked();
    void on_MovingAverage_stateChanged(int arg1);
    void on_zoomplus_clicked();
    void on_zoomminus_clicked();
    void on_resetbut_clicked();
    void on_up_y_scale_but_clicked();
    void on_down_y_scale_but_clicked();
    void on_zoomplus_x_clicked();
    void on_right_x_scale_but_clicked();
    void on_left_x_scale_but_clicked();
    void on_zoomminus_x_clicked();
    void on_resetbut_x_clicked();
    void on_FindPeaks_stateChanged(int arg1);
    void on_FindPeaksButton_clicked();
    void on_SaveDataGrafs_txt_clicked();
    void addPlottable(QCPAbstractPlottable *plottable);
};

#endif // GRAFIK_H
