#include "Spctr_data_processing.h"
#include "QCoreApplication"
#include "QDataStream"
#include "QDir"
#include "QFile"
#include "QMessageBox"
#include "QTextStream"
#include "QtCore"
#include "grafik.h"
#include "readfilepath.h"
#include "ui_mainwindow.h"

QElapsedTimer timer1;
MainWindow::tPolar tpolar;
grafik *grf;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->filetext->setReadOnly(true);
    this->setAttribute(Qt::WA_DeleteOnClose);   // - флаг для удаления экземпляра mainwindow
    grf = new grafik(nullptr);
    grf->setWindowTitle("Spectr");
    grf->show();
    //connect(this,SIGNAL(FileisOpen(QString)),this,SLOT(SaveDataFile(QString)));
    connect(this,SIGNAL(ReadyToBuild()),this,SLOT(on_BuildGraph_triggered()));
    //connect(this, SIGNAL(ReadyToBuild()), grf, SLOT(show()));
    //connect(this,SIGNAL(FileReadyToRead(QString)),this,SLOT(ReadFile(QString)));
    connect(this,
            SIGNAL(BuildGraf(MainWindow::tPolar &, bool)),
            grf,
            SLOT(receiveDataOfGrafik(MainWindow::tPolar &, bool)));
    connect(grf, SIGNAL(closeGrafik()), this, SLOT(close()));
    //timer1.start();
    on_OpenFile_triggered();
    //qDebug()<<"time 1 = "<<timer1.elapsed()<<" ms";
}

MainWindow::~MainWindow()
{
    for (int i = 0; i < tpolar.nfun; i++) {
        delete[] tpolar.data_y[i];
    }
    delete[] tpolar.data_y;
    delete ui;
}
//функция для получения пути к файлу gtx (вызывает отдельное окно, где выбирается путь к файлу)
/*void MainWindow::on_OpenFile_triggered() //старая версия
{
    ReadFilePath *wnd = new ReadFilePath(this);
    wnd->show();
    connect(wnd,SIGNAL(FilePath(QString)),this,SLOT(ReadFile(QString)));
}*/
void MainWindow::on_OpenFile_triggered()
{
    QStringList argumentlist = QCoreApplication::arguments();
    QString path;
    if(argumentlist.size() > 1)
    {
        path = argumentlist[argumentlist.size() - 1];
        tpolar.mode_exe = argumentlist[argumentlist.size() - 2];
    }
    else
    {
        //path = "C:/Spectr/spectr_01_d24_07_2020_t03_49_25V_31_08_2020_08_27_10.gxt";
        path = ":/data_files/files/spectr_01_d24_07_2020_t03_49_25V_31_08_2020_08_27_10.gxt";
        //tpolar.mode_exe = "-CтрSpPr";
    }
    //emit FileReadyToRead(path);

    timer1.start();
    //ReadFile(path);
    qDebug()<<"time 2 = "<<timer1.elapsed()<<" ms";
}
//чтение данных из файла
void MainWindow::ReadFile(const QString &path)
{
    QFile file(path);


    if(!(file.open(QFile::ReadOnly | QFile::Text)))
    {
        QMessageBox::critical(this,"ошибка","неправильный путь к файлу gxt!");
    }
    else
    {
        QTextStream readfile(&file);
        ui->filetext->setText(readfile.readAll());
    }

    file.close();
    //emit FileisOpen(path);
    timer1.start();
    SaveDataFile(path);
    qDebug()<<"time 3 = "<<timer1.elapsed()<<" ms";
}

void MainWindow::processingSpectrData(tworkParamstruct tworkparam, tworkstruct twork)
{
    qDebug()<<"YYYYYYYYYYYYYYYYYEEEEEEEEEEEEEEEEESSSSSSSSSSSSSSSSSSS!!!!!!!!!!!!!!!";
    params_data_have_read = false;
    struct_work_param = tworkparam;
    struct_work = twork;
    SaveDataFile("some path to file. If you want u can delete this string");
}

//функция для записи данных из файла в структуру
void MainWindow::SaveDataFile(const QString &path)
{
        tpolar.num_of_points = 2048;
        qDebug()<<"num 0f points = "<<tpolar.num_of_points;
        tpolar.init_x = 0. - 180. * (fabs(struct_work_param.ugolz)) > 80;
        tpolar.samp_freq = struct_work.fd / struct_work_param.n;

         tpolar.nfun = struct_work_param.GrafCountAniz + 1;

         tpolar.dlog = 10.;
         tpolar.ref = 1. / (struct_work.ref * struct_work.ref);

         tpolar.params.append("узкополосный спектр дБ отн.");
         tpolar.params.append("Полоса " + QString::number(struct_work.fd / struct_work_param.n) + " Гц Среднее " + QString::number(struct_work_param.timl) + "сек.");
         tpolar.params.append("Частота freq Гц Уровень level дБ");
         tpolar.params.append("Угол поворота оси, град:   XY " + QString::number(struct_work_param.ugol) + " XZ " + QString::number(struct_work_param.ugolz));
         tpolar.grfsnames.append("P^2");
         tpolar.grfsnames.append("W");
         tpolar.grfsnames.append("Wx");
         tpolar.grfsnames.append("Wy");

         params_data_have_read = true;
         params_data_changed = true;
//            if (param.contains("1/3 октавный спектр  ")) {
//                tpolar.oktav_mode = 1;
//            }

    //emit ReadyToBuild();
}

void MainWindow::transferChartData(float **data)
{
    qDebug()<<"HHHHHHHHHHEEEEEEEEEEEEEEERRRRRRRRRRRRREEEEEEEEEEEEEE!!!!!!!!!!!!";
    if(params_data_have_read)
    {
        on_BuildGraph_triggered(data);
    }
    else
    {
        QMessageBox::information(this, "","Метаданные настройки отображения графиков спектра не сохранены. Нажмите \"Ок\"");
    }
}

//функция открывает, считывает данные из dtx файла в структуру и вызывает класс формы grafik
void MainWindow::on_BuildGraph_triggered(float **chart_data)
{
    //QString file_path = "C:\\Users\\HP\\Desktop\\Диплом\\Обработка\\spectr\\" + tpolar.name;  //старая версия
    QFile file_dxt(tpolar.name);
    tpolar.data_y = new float *[tpolar.nfun];  // двумерный массив для хранения чисел для графиков
    int i = 0,j;

    for(; i<tpolar.nfun; i++)  //считываем числа из dtx файла в двумерный массив структуры
    {
        tpolar.data_y[i] = new float [tpolar.num_of_points];
        j = 0;

        for(; j<tpolar.num_of_points; j++)
        {
            tpolar.data_y[i][j] = chart_data[i][j];
            //if (tpolar.mode_exe != "-CтрSpPr")
            tpolar.data_y[i][j] = tpolar.dlog
                    * log10f(tpolar.data_y[i][j]
                             * tpolar.ref); //перевод данных в децибелы
            if(QString::number(tpolar.data_y[i][j]) == "nan" || QString::number(tpolar.data_y[i][j]) == "-inf")
            {
                tpolar.data_y[i][j] = -300;
            }
        }
    }
    //ui->filetext->setText(deleteme);
    emit BuildGraf(tpolar, params_data_changed);
    params_data_changed = false;
}
