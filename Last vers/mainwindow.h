#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //структура для хранения данных, считанных из файла
    typedef struct
    {
        int			nfun;     /* число графиков */
        float		k;
        float       init_x;       //начальное значение х
        float       samp_freq;   //расстояние между соседними точками на оси х
        float       **data_y;   //массив для хранения чисел из dtx файла
        int         num_of_points; //число точек для графика
        int			width;
        int			centrx;
        int			centry;
        int			curx;
        int			cury;
        int			linugol;
        int			posugol;
        int			linval;
        int			posval;
        int			n;
        int         points;
        float		am;			/* max */
        float		an;			/* min */
        float		mas;
        float		xmin;//мин по оси х
        float		dfi; //полоса (промежуток по оси y)
        float		ref; // относительная какой величины идет измерение (для преобразование в Дб)
        float		dlog;// для преобразования в
        float		ugolz;
        float		ugolx;
        int			tipdb;
        int			radius;		/* размер сигнала */
        int			numgraf;	/* номер фильтра */
        float		*addrold;
        float		*addr;
        bool        oktav_mode = 0;
        QString mode_exe;
        //TImage		*PolarGrafic;

        //TRichEdit	*Zagolovok;
        int			mode;
        //char		name[600];
        QString     name;
        QStringList grfsnames;  //  - названия графиков
        QStringList params;    //  - параметры программы
    } tPolar;

signals:
    void FileisOpen(const QString& path);             // сигнал вызывает функцию, сохраняющую данные из файла в структуру
    void BuildGraf(MainWindow::tPolar &tpolar);
    void FileReadyToRead(const QString& path);
    void ReadyToBuild();

private slots:
    void ReadFile(const QString &str);
    void on_OpenFile_triggered();
    void SaveDataFile(const QString& path);

    void on_BuildGraph_triggered();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
