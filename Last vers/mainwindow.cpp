#include "mainwindow.h"
#include "QCoreApplication"
#include "QDataStream"
#include "QDir"
#include "QElapsedTimer"
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
    //connect(this,SIGNAL(FileisOpen(QString)),this,SLOT(SaveDataFile(QString)));
    connect(this,SIGNAL(ReadyToBuild()),this,SLOT(on_BuildGraph_triggered()));
    connect(this, SIGNAL(ReadyToBuild()), grf, SLOT(show()));
    //connect(this,SIGNAL(FileReadyToRead(QString)),this,SLOT(ReadFile(QString)));
    connect(this,
            SIGNAL(BuildGraf(MainWindow::tPolar &)),
            grf,
            SLOT(receiveDataOfGrafik(MainWindow::tPolar &)));
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
    ReadFile(path);
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
//функция для записи данных из файла в структуру
void MainWindow::SaveDataFile(const QString &path)
{
    QString DataFile = ui->filetext->toPlainText().trimmed();
    QStringList lst = DataFile.split("\n",QString::SkipEmptyParts);  // разбиваем текст из файла на строки

    foreach(QString str,lst)         //заполнение полей структуры данными из файла
    {
        if(str.contains("FILE"))
        {
            /*QStringList name_dtx_file = str.split(" ",Qt::SkipEmptyParts); //разбиваем строку на подстроки и записываем нужные в структуру
            tpolar.name = name_dtx_file[1].toStdString().c_str();
            continue;*/
            QString pa = path;
            tpolar.name = pa.replace("gxt","dxt");
        }
        if (str.contains("NPTX"))
        {
            QStringList data_x = str.split(" ",Qt::SkipEmptyParts);
            tpolar.num_of_points = data_x[1].toFloat();
            tpolar.init_x = data_x[2].toFloat();
            tpolar.samp_freq = data_x[3].toFloat();
            continue;
        }
        if (str.contains("NFUN"))
        {
            QStringList num_of_graf = str.split(" ",Qt::SkipEmptyParts);
            tpolar.nfun = num_of_graf[1].toInt();
            continue;
        }
        if (str.contains("COMMENT"))
        {
            QStringList grfname = str.split(" ",Qt::SkipEmptyParts);
            tpolar.grfsnames.append(grfname[1]);
            continue;
        }
        if (str.contains("DB_SCALE"))
        {
            QStringList data_to_convert_to_log = str.split(" ",Qt::SkipEmptyParts);
            tpolar.dlog = data_to_convert_to_log[1].toFloat();
            tpolar.ref = data_to_convert_to_log[2].toFloat();
            continue;
        }
        if(str.contains("CMTW"))
        {
            QString param = str.replace("CMTW ","");
            QStringList split_param;
//            if(param.contains("Узкополосный") ||param.contains("спектр") || param.contains("Полоса") || param.contains("Угол") || param.contains("Приемный") || param.contains("Размер"))
//            {
//                split_param = param.split(" ",Qt::SkipEmptyParts);
//                param = "";
//                for (int i = 0; i < split_param.size(); i++) {
//                    param += split_param[i] + " ";
//                }
//                tpolar.params.append(param);
//                continue;
//            }
            if (param.contains("1/3 октавный спектр  ")) {
                tpolar.oktav_mode = 1;
            }
            if(param.contains("Частота") || param.contains("Время"))
            {
                split_param = param.split(" ",Qt::SkipEmptyParts);
                tpolar.params.append(split_param[0] + " freq " + split_param[1] + "  " + split_param[2] + " level");
                continue;
            }
            split_param = param.split(" ",Qt::SkipEmptyParts);
            param = "";
            for (int i = 0; i < split_param.size(); i++) {
                param += split_param[i] + " ";
            }
            tpolar.params.append(param);

        }

    }
    emit ReadyToBuild();
}

//функция открывает, считывает данные из dtx файла в структуру и вызывает класс формы grafik
void MainWindow::on_BuildGraph_triggered()
{
    //QString file_path = "C:\\Users\\HP\\Desktop\\Диплом\\Обработка\\spectr\\" + tpolar.name;  //старая версия
    QFile file_dxt(tpolar.name);
    tpolar.data_y = new float *[tpolar.nfun];  // двумерный массив для хранения чисел для графиков
    int i = 0,j;

    if(!(file_dxt.open(QIODevice::ReadOnly)))
    {
        QMessageBox::critical(this,"ошибка","неправильный путь к файлу dxt!");
    }
    else
    {
        for(; i<tpolar.nfun; i++)  //считываем числа из dtx файла в двумерный массив структуры
        {
            tpolar.data_y[i] = new float [tpolar.num_of_points];
            j = 0;

            for(; j<tpolar.num_of_points && !(file_dxt.atEnd()); j++)
            {
                file_dxt.read((char*)&tpolar.data_y[i][j],sizeof(float));
                //qDebug() << "Данные " << tpolar.data_y[i][j] << "ref " << tpolar.ref << "лог "<< tpolar.dlog;
                if (tpolar.mode_exe != "-CтрSpPr")
                    tpolar.data_y[i][j] = tpolar.dlog
                                          * log10f(tpolar.data_y[i][j]
                                                   * tpolar.ref); //перевод данных в децибелы
                //qDebug() << "Данные после" << tpolar.data_y[i][j];
                if(QString::number(tpolar.data_y[i][j]) == "-inf")
                {
                    tpolar.data_y[i][j] = -300;
                }
            }
        }
    }
    //ui->filetext->setText(deleteme);
    emit BuildGraf(tpolar);
}
