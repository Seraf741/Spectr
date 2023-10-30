#include "grafik.h"
#include "ui_grafik.h"
#include "QMessageBox"
#include "QStandardItemModel"
#include "QtCore"
#include "QDebug"
#include "QList"

QVector<QString> FrqList;

float yshift,xshift,xcur_min,xcur_max,cur_min,cur_max;
float yMin,yMax,xMin,xMax;
QVector<float> average_deviation;                               // - массив для хранения значения среднего отклонения смежных точек каждого графика
int size_of_averaging = 20;
grafik::grafik(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::grafik)
{
    ui->setupUi(this);
    ui->sizeOfAveraging->setVisible(false);
    ui->FindPeaksWidget->setVisible(false);
    ui->GrafList2->setVisible(false);
    connect(ui->sizeOfAveraging,SIGNAL(valueChanged(int)),this,SLOT(on_MovingAverage_stateChanged(int)));
}
//grafik::~grafik()
//{
//    for (int i = 0; i < tpolar->nfun; i++) {
//        delete[] data_grafs[i];
//    }
//    delete[] data_grafs;
//    delete border_lines[0];
//    delete border_lines[1];
//    delete[] border_lines;
//    delete order_of_grafs;
//    delete[] model_for_selecting_grafs;
//    delete[] model_for_displaying_info;
//    delete tpolar;

//    delete ui;
//}

QString IsNan(qreal value)
{
    if (value != value) {
        return "NaN";
    } else if (value > std::numeric_limits<qreal>::max()) {
        return "+Inf";
    } else if (value < -std::numeric_limits<qreal>::max()) {
        return "-Inf";
    } else
        return "";
}

/*
 -----------------------------------------------------------------------------------------------
 функция принимает структуру с данными и вызывает функции создания модели и постройки графиков
 -----------------------------------------------------------------------------------------------
*/
void grafik::receiveDataOfGrafik(MainWindow::tPolar &tpolar1)
{
    tpolar = new MainWindow::tPolar(tpolar1);
    SetGrafList(tpolar->grfsnames, tpolar->nfun);                 //  - вызываем функцию, которая создает выпадающий список с названиями графиков
    SetGrafInfo(tpolar->params, tpolar->params.size());          //  - вызываем функцию, которая добавляет информацию о графиках в ListView
    SetGrafListForShowCoord(tpolar->grfsnames, tpolar->nfun);   //  - вызываем функцию, которая создает выпадающий список с названиями графиков для выбора нужного для дальнейшего отображения координат его точек
    connect(ui->GrafList->model(),SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(ChangeGrafsForShow(const QModelIndex&,const QModelIndex&)));
    order_of_grafs = new int [tpolar->nfun];
    data_grafs = new float *[tpolar->nfun];
    average_deviation = QVector<float>(tpolar->nfun);
    for (int i = 0; i < tpolar->nfun; i++) {
        data_grafs[i] = new float [tpolar->num_of_points];                                //  - создаем двумерный массив для хранения координат крафиков
        for (int j = 0; j < tpolar->num_of_points; j++)
        {
            data_grafs[i][j] = tpolar->data_y[i][j];
            if(j != 0)                                   // - считаем среднюю разницу между смежными точками, чтобы при усредении не пропадали дискреты
            {
                float val_av = data_grafs[i][j] - data_grafs[i][j - 1];
                if(val_av < 0)
                {
                    average_deviation[i] += val_av * (-1);
                }
                else
                {
                    average_deviation[i] += val_av;
                }
            }
        }
        average_deviation[i] /= tpolar->num_of_points - 1;
        //QMessageBox::about(this,"","average " + QString::number(i + 1) + " = " + QString::number(average_deviation[i]));
        order_of_grafs[i] = i;                                                       //  - добавляем номер графика отображения
    }
    quantity_of_grafs = tpolar->nfun;

    ui->ZoomOnX->adjustSize();
    ui->ZoomOnY->adjustSize();
    ShowGraf();
}
/*
 -----------------------------------------------------------------------------------------------
 функция берет данные из структуры tpolar и массива с номерами графиков для отображения,
 затем строит по ним графики
 -----------------------------------------------------------------------------------------------
*/
void grafik::ShowGraf()
{
    FrqList << "1"
            << "1.25"
            << "1.6"
            << "2"
            << "2.5"
            << "3.15"
            << "4"
            << "5"
            << "6.3"
            << "8"
            << "10"
            << "12.5"
            << "16"
            << "20"
            << "25"
            << "31.5"
            << "40"
            << "50"
            << "63"
            << "80"
            << "100"
            << "125"
            << "160"
            << "200"
            << "250"
            << "315"
            << "400"
            << "500"
            << "630"
            << "800"
            << "1000"
            << "1250"
            << "1600"
            << "2000"
            << "2500"
            << "3150"
            << "4000"
            << "5000"
            << "6300"
            << "8000"
            << "10000"
            << "12500"
            << "16000"
            << "20000"
            << "25000"
            << "31500"
            << "40000"
            << "50000"
            << "63000"
            << "80000"; //таблица 1/3 октавных частот
    int numgraf,init_pos;
    ui->widget->clearGraphs();     //  - очищаем координатную плоскость
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //qDebug()<<"num of fun"<<tpolar->nfun;
    // Отключаем масштабирование по осям и вывод значений графика
//    ui->ZoomOnX->setChecked(false);
//    ui->ZoomOnY->setChecked(false);
//    ui->DispCoordValue->setChecked(false);

    for (int i = ui->widget->plottableCount(); i >= 0 && (ui->widget->plottableCount() != 0); i--)  // - удаляем все графики
    {
        ui->widget->removePlottable(i);
    }
    if(ui->ZoomOnX->isChecked() || ui->ZoomOnY->isChecked())
    {
        border_lines[0] = nullptr;
        border_lines[1] = nullptr;
        border_line_num = 0;
    }
    if(ui->DispCoordValue->isChecked())
    {
        delete tracer;
    }
    qDebug()<<"num_of_grafs = "<< ui->widget->plottableCount();
    if (order_of_grafs != nullptr)
    {
        if (tpolar->oktav_mode == 1) { //ищем ближайшую 1/3 октаву
            int closest_frq;
            bool foundAnyClosest = false;
            for (auto num : FrqList) {
                if (num.toFloat() <= tpolar->init_x)
                    if (!foundAnyClosest) {
                        closest_frq = num.toFloat();
                        foundAnyClosest = true;
                    } else if (num.toFloat() > closest_frq) {
                        closest_frq = num.toFloat();
                    }
            }
            init_pos = FrqList.indexOf(QString::number(closest_frq)); //позиция октавы
        }

        for(int i = 0; i < quantity_of_grafs; i++)
        {
            numgraf = order_of_grafs[i];                 //  - номер текущего графика
            QVector<double> x(tpolar->num_of_points), y(tpolar->num_of_points);     //  - векторы для хранения точек графика

            for(int j = 0; j<tpolar->num_of_points; j++)
            {
                if(tpolar->oktav_mode==1)
                    x[j] = FrqList.at(init_pos+j).toFloat();
                else
                    x[j] = tpolar->init_x + tpolar->samp_freq * j;
                y[j] = data_grafs[numgraf][j];
            }
            ui->widget->addGraph();
            ui->widget->graph(i)->setData(x,y);
            ui->widget->graph(i)->setName(tpolar->grfsnames[numgraf]);            //  - устанавливаем название графика для отображения в легенде
            ui->widget->graph(i)->setPen(QPen(color_grf[numgraf]));     //  - устанавливаем цвет графика
//            if(numgraf == 0)
//            {
//                //ui->widget->graph(i)->setLineStyle(QCPGraph::lsNone);
//                ui->widget->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted,8));
//            }
        }

        if(ui->FindPeaks->isChecked())
        {
            int num = quantity_of_grafs;
            ui->widget->addGraph();
            ui->widget->graph(num)->setData(data_peaks_pos,data_peaks);
            ui->widget->graph(num)->setPen(QPen(Qt::black));     //  - устанавливаем цвет графика
            ui->widget->graph(num)->setLineStyle(QCPGraph::lsNone);
            ui->widget->graph(num)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted,8));
        }
        else
        {
            ui->widget->addGraph();
        }
        ui->widget->legend->removeItem(ui->widget->legend->itemCount() - 1);            //  - удаляем из легенды добавленный в условии выше график

        if(tpolar->oktav_mode == 1)
        {
            ui->widget->xAxis->setLabel("x,  Гц");
            ui->widget->yAxis->setLabel("y,  Гц");
        }
        else
        {
            ui->widget->xAxis->setLabel("x,  Гц");
            ui->widget->yAxis->setLabel("y,  Дб");
        }

        if (tpolar->mode_exe == "-CtrSpPr") {
            ui->widget->xAxis->setLabel("x,  cек.");
            ui->widget->yAxis->setLabel("y,  Дб");
        }

        //if(!(ui->ZoomOnX->isChecked() || ui->ZoomOnY->isChecked()))     // - если включена функция масштабирования по одной из осей, то оставляем границы видимости графиков без изменений
        if(reset == true)
        {
            reset = false;
            if(tpolar->oktav_mode==1)
            {
                xMin = FrqList.at(init_pos).toFloat();
                xMax = FrqList.at(init_pos+tpolar->num_of_points-1).toFloat();

            }
            else
            {
                xMin = tpolar->init_x;
                xMax = (tpolar->init_x + tpolar->num_of_points * tpolar->samp_freq);
            }
            ui->widget->xAxis->setRange(xMin,xMax);
            xshift = (xMax-xMin)*0.25;
            xcur_min = xMin;
            xcur_max = xMax;
//            if (IsNan(data_grafs[order_of_grafs[0]][0]) == "NaN")
//                yMin = 0;
//            else
//                yMin = data_grafs[order_of_grafs[0]][0];
//            if (IsNan(data_grafs[order_of_grafs[0]][0]) == "NaN")
//                yMax = 0;
//            else
//                yMax = data_grafs[order_of_grafs[0]][0];
//            qDebug() << data_grafs[order_of_grafs[0]][0];
            yMin = data_grafs[order_of_grafs[0]][0], yMax = data_grafs[order_of_grafs[0]][0];
            // цикл для выбора начального значения для поиска нижней границы масштаба оси у
            for (int i = 0; i < tpolar->num_of_points - 1; i++) {
                if(QString::number(data_grafs[order_of_grafs[0]][i]) == "-300")    //  если значение графика искажено (равно -300), то берем следующее число в качестве начального значения
                {
                    yMin = data_grafs[order_of_grafs[0]][i+1];
                    continue;
                }
                break;
            }
            //циклы для нахождения минимума и максимума для оси у
            for(int i = 0; i < quantity_of_grafs; i++)
            {
                numgraf = order_of_grafs[i];
                for(int j = 0; j<tpolar->num_of_points; j++)
                {
                    if(data_grafs[numgraf][j] == -300)    //  если значение графика искажено (равно -inf)
                    {
                        continue;
                    }
                    if(yMin > data_grafs[numgraf][j]){yMin = data_grafs[numgraf][j];}
                    if(yMax < data_grafs[numgraf][j] && (i == 0 || j >= 10)){yMax = data_grafs[numgraf][j];}   //  - выражение (i == 0 || j >= 10) нужно, чтобы не потерять найденный максимум
                    if(j == 10 && i == 0)
                    {
                        if((yMax/data_grafs[numgraf][j]) > 1.3){yMax = data_grafs[numgraf][j];}
                    }
                }
            }

            //qDebug()<<"yMin = "<<yMin<<" yMax = "<<yMax;
            yMax = yMax + 5;                                //  - чтобы были видны найденные максимумы (перевернутые треугольники)
            ui->widget->yAxis->setRange(yMin, yMax);
        }
        yshift = (yMax-yMin)*0.25;
        cur_min = yMin;
        cur_max = yMax;
        //ui->widget->legend->removeItem(5);
        ui->widget->legend->setVisible(true);
        ui->widget->replot();
        if(ui->DispCoordValue->isChecked())
        {
            on_DispCoordValue_stateChanged(1);
            return;
        }
        if(ui->ZoomOnX->isChecked())
        {
            on_ZoomOnX_stateChanged(1);
        }
        if(ui->ZoomOnY->isChecked())
        {
            on_ZoomOnY_stateChanged(1);
        }
        //ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    }

    else
    {
        ui->widget->replot();
    }
}
/*
 -----------------------------------------------------------------------------------------------
 функция создает модель, заполняет ее элементами (чекбоксами) управления выбором отображения графиков и
 добавляет эту модель в выпадающий список
 -----------------------------------------------------------------------------------------------
*/
void grafik::SetGrafList(QStringList grfs_names, int &ngrf)
{
    model_for_selecting_grafs = new QStandardItemModel();

    for(int i = 0; i < ngrf; i++)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(grfs_names[i]);
        item->setBackground(QBrush(QColor(240,248,255)));
        //item->setForeground(QBrush(QColor(255,255,255)));
        item->setForeground(color_grf[i]);
        item->setFont(QFont("Times New Roman",14, QFont::Bold));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Checked, Qt::CheckStateRole);

        model_for_selecting_grafs->setItem(i,0,item);           //  - добавляем элемент в модель

    }
    ui->GrafList->setModel(model_for_selecting_grafs);          //  - добавляем модель в выпадающий список
}
/*
 -----------------------------------------------------------------------------------------------
 функция создает модель, заполняет ее элементами, содержащими информацию о графиках, и добавляет
 ее в listview для отображения
 -----------------------------------------------------------------------------------------------
*/
void grafik::SetGrafInfo(QStringList grfs_info, int nparams)
{
    model_for_displaying_info = new QStandardItemModel();

    for(int i = 0; i <nparams; i++)                                   //  - i = 1 т.к 0 строка идет в название
    {
        QStandardItem *item = new QStandardItem(grfs_info[i]);
        model_for_displaying_info->appendRow(item);
        if(tpolar->params[i].contains("freq"))
        {
            QString freq_and_level = tpolar->params[i];
            model_for_displaying_info->item(i)->setText((freq_and_level.replace("freq","--")).replace("level","--"));
            continue;
        }
    }
    ui->listView->setModel(model_for_displaying_info);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
/*
 -----------------------------------------------------------------------------------------------
 функция создает модель, заполняет ее элементами (строками с возможностью выбора нужной) с названием
 графиков с возможностью выбора графика для отображения координат его точек и добавляет эту модель
 в выпадающий список
 -----------------------------------------------------------------------------------------------
*/
void grafik::SetGrafListForShowCoord(QStringList grfs_names, int &ngrf)
{
    model_for_sel_grfs_to_show_coord = new QStandardItemModel();

    for(int i = 0; i < ngrf; i++)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(grfs_names[i]);
        item->setBackground(QBrush(QColor(240,248,255)));
        item->setForeground(color_grf[i]);
        item->setFont(QFont("Times New Roman",13));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        model_for_sel_grfs_to_show_coord->setItem(i,0,item);
    }
    ui->GrafList2->setModel(model_for_sel_grfs_to_show_coord);
}
/*
 -----------------------------------------------------------------------------------------------
 функция
 -----------------------------------------------------------------------------------------------
*/
void grafik::keyPressEvent(QKeyEvent *event)
{
    //qDebug()<<"VISIBLE = "<<ui->widget->legend->visible();
    //qDebug()<<"VISIBLE text = "<<event->text()<<" key = "<<event->key();
    if(event->modifiers()&Qt::ControlModifier)
    {
        if(ui->widget->legend->visible())
        {
            ui->widget->legend->setVisible(false);
            ui->widget->replot();
            return;
        }
        ui->widget->legend->setVisible(true);
    }
    if(event->key() == Qt::Key_0)
    {
        if(ui->widget->legend->visible())
        {
            ui->widget->legend->setVisible(false);
            ui->widget->replot();
            return;
        }
        ui->widget->legend->setVisible(true);
    }
    else if((event->key() == 16777234 || event->key() == 16777236) && ui->DispCoordValue->isChecked())      //  - условие для отслеживания нажатия стрелок "←", "→" и перемещения трейсера,
    {                                                                                                       //    отображающего координаты графика, на одну точку в направлении,
        double coordx = tracer->position->key();                                                            //    соответствующем нажатой стрелке
        switch (event->key())
        {
        case 16777234:
            coordx -= tpolar->samp_freq;
            break;
        case 16777236:
            coordx += tpolar->samp_freq;
            break;
        default:
            return;
        }
        //qDebug()<<"tracer pos = "<<tracer->position->key()<<" coordx = "<<coordx;
        tracerMoveToShowGrafValue(coordx);
        return;
    }
    else if(event->key() == 16777220 && (ui->ZoomOnX->isChecked() || ui->ZoomOnY->isChecked()))
    {
        QPointF *pos;
        if(ui->ZoomOnX->isChecked())
        {
            pos = new QPointF(ui->widget->xAxis->coordToPixel(ui->X_Cord->text().toDouble()), 0);
        }
        else if(ui->ZoomOnY->isChecked())
        {
            pos = new QPointF(0, ui->widget->yAxis->coordToPixel(ui->Y_Cord->text().toDouble()));
        }
        //qDebug()<<"pos_x = "<<pos->x()<<"pos_y = "<<pos->y();
        QMouseEvent *m_event = new QMouseEvent(QEvent::MouseButtonPress, *pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        SlotMouseMove(m_event);
        SlotMousePress(m_event);
        delete pos;
        return;
    }
    ui->widget->replot();
}
/*
 -----------------------------------------------------------------------------------------------
 функция вызывается при изменении пользователем числа отображаемых графиков,
 принимает номер графика, который добавили/убрали из модели и изменяет (добавляет/убирает)
 данные в массиве номеров графика для отображения
 -----------------------------------------------------------------------------------------------
*/
void grafik::ChangeGrafsForShow(const QModelIndex& numgrf,const QModelIndex& unused)
{
    //reset = true;
    // отключаем масштабирование, сняв выделение с чекбоксов выбора оси масштабирования и просмотр значений графика
//    ui->ZoomOnX->setChecked(false);
//    ui->ZoomOnY->setChecked(false);
//    ui->DispCoordValue->setChecked(false);
//    ui->FindPeaks->setChecked(false);
//    scale_changed = false;

    int *new_order_of_grafs;
    if (model_for_selecting_grafs->itemFromIndex(numgrf)->checkState() == Qt::Unchecked)        //  - если пользователь снял выделение какого-то чекбокса, соответствующего графику
    {
        if (quantity_of_grafs == 1)              //  - если удаляемый график является единственным в координатной плоскости
        {
            delete[] order_of_grafs;
            order_of_grafs = nullptr;
            quantity_of_grafs--;
            ShowGraf();
            //ui->DispCoordValue->setChecked(false);     // - выключаем отображение значений графика
            ui->DispCoordValue->setEnabled(false);    // - делаем неактивным окно включения отображения значений графика
            ui->FindPeaks->setEnabled(false);        //  - делаем неактивным окно включения функции поиска пиков
            return;
        }
        /* копируем данные из массива номеров графиков в новый массив за исключением отключенного графика
         * удаляем старые данные массива номеров и записываем в него значения из нового
        */
        new_order_of_grafs = new int [quantity_of_grafs - 1];

        for(int i = 0, j = 0; i < quantity_of_grafs; i++)
        {
            if (numgrf.row() != order_of_grafs[i])
            {
                new_order_of_grafs[j] = order_of_grafs[i];
                j++;
            }
        }
        delete[] order_of_grafs;
        order_of_grafs = new_order_of_grafs;
        quantity_of_grafs--;
        ChangeNumberOfDisplayedGrafs();
        if(ui->FindPeaks->isChecked())
        {
           on_FindPeaksButton_clicked();
        }
        else
        {
            ShowGraf();
        }
    }
    else
    {
        if (order_of_grafs == nullptr)           //  - если добавляемый график  - первый в массиве номеров графиков
        {
            order_of_grafs = new int [1];
            order_of_grafs[0] = numgrf.row();
            quantity_of_grafs++;
            if(ui->FindPeaks->isChecked())
            {
               on_FindPeaksButton_clicked();
            }
            else
            {
                ShowGraf();
            }
            ui->DispCoordValue->setEnabled(true);       // - добавляем функцию отображения значений графика
            ui->FindPeaks->setEnabled(true);           //  - включаем функцию поиска пиков
            ChangeNumberOfDisplayedGrafs();
            return;
        }
        /* копируем данные из массива номеров графиков в новый массив c добавлением включенного графика
         * удаляем старые данные массива номеров и записываем в него значения из нового
        */
        new_order_of_grafs = new int [quantity_of_grafs + 1];
        for(int i = 0; i < quantity_of_grafs; i++)
        {
            new_order_of_grafs[i] = order_of_grafs[i];
        }
        new_order_of_grafs[quantity_of_grafs] = numgrf.row();
        delete[] order_of_grafs;
        order_of_grafs = new_order_of_grafs;
        quantity_of_grafs++;
        ChangeNumberOfDisplayedGrafs();
        if(ui->FindPeaks->isChecked())
        {
           on_FindPeaksButton_clicked();
        }
        else
        {
            ShowGraf();
        }

    }

}
/*
 -----------------------------------------------------------------------------------------------
 функция, в зависимости от состояния ячеек выбора осей масштабирования, соединяет/отсоединяет
 сигналы и слоты изменения положения курсора мыши и нажатия на кнопку мыши, добавляет/убирает
 линии границ участка масштабирования
 -----------------------------------------------------------------------------------------------
*/
void grafik::on_ZoomOnX_stateChanged(int arg1)
{
    if ((ui->ZoomOnX->isChecked() || ui->ZoomOnY->isChecked()) == false)        //  - если обе ячейки выбора осей масштабирования отключены
    {
        ui->X_Cord->setText("");
        disableZoom();
        return;
    }
    if(ui->ZoomOnX->isChecked())            //  - если включена ячейка масштабирования по оси х
    {
        ui->ZoomOnY->setChecked(false);
        if(ui->DispCoordValue->isChecked())    // - если показ значений графика активен (стоит галочка)
        {
            ui->DispCoordValue->setChecked(false);        // - отключаем показ значений графика
            return;                                      //  - выходим из функции
        }
        border_lines[0] = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);   //  - создаем первую линию границы
        del_me_num++;
        //qDebug()<<"created 1 element border_line_num = "<<border_line_num;
        //qDebug()<<"del_me_num"<<del_me_num;
        //qDebug()<<"1 count plot x = "<<ui->widget->plottableCount();
        addPlottable(border_lines[0]);                           //  - добавляем линию на график
        //qDebug()<<"2 count plot x = "<<ui->widget->plottableCount();
        border_lines[1] = nullptr;          //  - ставим указатель 2 линии в nullptr, чтобы при смене масштабируемой оси с у на х выполнилось условие создания 2 линии на оси х
        //ui->DispCoordValue->setCheckState()
    }
    else
    {
//        int grfs_count = quantity_of_grafs;
//        if(!(ui->FindPeaks->isChecked()))           //  - если функция поиска пиков неактивна, значит в виджете отсутствует график пиков, поэтому надо уменьшить переменную, хранящую число графиков
//        {
//            grfs_count--;
//        }
        //qDebug()<<"count plot before del x = "<<ui->widget->plottableCount();
        //  - удаляем линии границ масштабирования при смене оси масштабирования
        for (int i = 0; i < (ui->widget->plottableCount() - (quantity_of_grafs + 1));)           /* + 1 т.к есть еще 1 график пиков */
        {
            ui->widget->removePlottable(ui->widget->plottableCount() - 1);
            //qDebug()<<"count plot y iter = "<<ui->widget->plottableCount();
        }
        border_line_num = 0;
       //qDebug()<<"count plot after del x = "<<ui->widget->plottableCount();
        ui->X_Cord->setText(""); // очищаем поле численного отображения положения мыши по оси x
    }
    if(first_zoom)
    {
        connect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMove(QMouseEvent*)));
        connect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePress(QMouseEvent*)));
        first_zoom = false;
    }

}
/*
 -----------------------------------------------------------------------------------------------
 функция, в зависимости от состояния ячеек выбора осей масштабирования, соединяет/отсоединяет
 сигналы и слоты изменения положения курсора мыши и нажатия на кнопку мыши, добавляет/убирает
 линии границ участка масштабирования
 -----------------------------------------------------------------------------------------------
*/
void grafik::on_ZoomOnY_stateChanged(int arg1)
{
    if ((ui->ZoomOnX->isChecked() || ui->ZoomOnY->isChecked()) == false)                //  - если обе ячейки выбора осей масштабирования отключены
    {
        ui->Y_Cord->setText("");
        disableZoom();
        return;
    }
    if(ui->ZoomOnY->isChecked())            //  - если включена ячейка масштабирования по оси y
    {
        ui->ZoomOnX->setChecked(false);
        if(ui->DispCoordValue->isChecked())    // - если показ значений графика активен (стоит галочка)
        {
            ui->DispCoordValue->setChecked(false);        // - отключаем показ значений графика
            return;                                      //  - выходим из функции
        }
        border_lines[0] = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);   //  - создаем первую линию границы
        del_me_num++;
        //qDebug()<<"created 1 element border_line_num = "<<border_line_num;
        //qDebug()<<"del_me_num"<<del_me_num;
        addPlottable(border_lines[0]);                           //  - добавляем линию на график
        qDebug()<<"count plot y = "<<ui->widget->plottableCount();
        border_lines[1] = nullptr;          //  - ставим указатель 2 линии в nullptr, чтобы при смене масштабируемой оси с x на y выполнилось условие создания 2 линии на оси y
    }
    else
    {
//        int grfs_count = quantity_of_grafs;
//        if(!(ui->FindPeaks->isChecked()))           //  - если функция поиска пиков неактивна, значит в виджете отсутствует график пиков, поэтому надо уменьшить переменную, хранящую число графиков
//        {
//            grfs_count--;
//        }
        qDebug()<<"count plot before del y = "<<ui->widget->plottableCount();
        //  - удаляем линии границ масштабирования при смене оси масштабирования
        for (int i = 0; i < (ui->widget->plottableCount() - (quantity_of_grafs + 1));)       /* + 1 т.к есть еще 1 график пиков */
        {
            ui->widget->removePlottable(ui->widget->plottableCount() - 1);
        }
        border_line_num = 0;
        //qDebug()<<"count plot after del y = "<<ui->widget->plottableCount();
        ui->Y_Cord->setText(""); // очищаем поле численного отображения положения мыши по оси y
    }
    if(first_zoom)
    {
        connect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMove(QMouseEvent*)));
        connect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePress(QMouseEvent*)));
        first_zoom = false;
    }
}


void grafik::disableZoom()
{
    disconnect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMove(QMouseEvent*)));      //  - отсоединяем сигналы
    disconnect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePress(QMouseEvent*)));
    first_zoom = true;
    // удаляем линии границ масштабирования
    if(border_line_num == 1)
    {
        //qDebug()<<"All delete _1_ x = "<<ui->widget->plottableCount();
        ui->widget->removePlottable(ui->widget->plottableCount() - 1);          //  - удаляем 1 линию
        border_lines[border_line_num] = nullptr;
        border_line_num--;
    }
    if(border_line_num == 0)
    {
        ui->widget->removePlottable(ui->widget->plottableCount() - 1);              //  - удаляем 2 линию
        border_lines[border_line_num] = nullptr;
        //qDebug()<<"All delete _0_ x = "<<ui->widget->plottableCount();
    }

    ui->widget->replot();

    if(ui->DispCoordValue->isChecked())    // - если показ значений графика активен (стоит галочка)
    {
        ui->DispCoordValue->setChecked(false);        // - отключаем показ значений графика
    }
}
/*
 -----------------------------------------------------------------------------------------------
 функция вызывается при движении курсора мыши по полю графика, графически отображает
 изменение положения линии границы в зависимости от положения курсора мыши,
 после фиксации положения 1 линии создает и добавляет на график 2 линию
 -----------------------------------------------------------------------------------------------
*/
void grafik::SlotMouseMove(QMouseEvent *event)
{
    if(!(QApplication::mouseButtons()))
    {

        if(border_line_num != 0 && border_lines[border_line_num] == nullptr)      //  - если 2 линия еще не создана
        {
            border_lines[border_line_num] = new QCPCurve(ui->widget->xAxis,ui->widget->yAxis);
            addPlottable(border_lines[border_line_num]);
            qDebug()<<"created 2 element";
        }

        QVector<double> x(2),y(2);
        // меняем положение линии границы, беря текущие координаты положения курсора мыши
        if(ui->ZoomOnX->isChecked())                                       //  - если масштабирование по оси х
        {
            QString str = QString::number(ui->widget->xAxis->pixelToCoord(event->pos().x()));
            ui->X_Cord->setText(str);
            ui->Y_Cord->setText("");
            x[0] = ui->widget->xAxis->pixelToCoord(event->pos().x());
            y[0] = (ui->widget->yAxis->range()).lower;
            x[1] = ui->widget->xAxis->pixelToCoord(event->pos().x());
            y[1] = (ui->widget->yAxis->range()).upper;
            border_lines[border_line_num]->setData(x,y);
        }
        else                                                                //  - если масштабирование по оси у
        {
            QString str = QString::number(ui->widget->yAxis->pixelToCoord(event->pos().y()));
            ui->Y_Cord->setText(str);
            ui->X_Cord->setText("");
            x[0] = (ui->widget->xAxis->range()).lower;
            y[0] = ui->widget->yAxis->pixelToCoord(event->pos().y());
            x[1] = (ui->widget->xAxis->range()).upper;
            y[1] = ui->widget->yAxis->pixelToCoord(event->pos().y());
            border_lines[border_line_num]->setData(x,y);
        }

        ui->widget->replot();
    }
}
/*
 -----------------------------------------------------------------------------------------------
 функция вызывается при нажатии на кнопку мыши, фиксирует линию границы масштабирования
 на месте положения курсора мыши, записывает координаты курсора мыши в массив границ масштабирования,
 меняет границы отображения графика (собственно масштабирует)
 -----------------------------------------------------------------------------------------------
*/
void grafik::SlotMousePress(QMouseEvent *event)
{
    scale_changed = true;
    if(ui->ZoomOnX->isChecked())
    {
        segment_borders[border_line_num] = ui->widget->xAxis->pixelToCoord(event->pos().x());
        border_line_num++;
        qDebug()<<"set line, border_line_num = "<<border_line_num<<" posx = "<<event->pos().x()<<" posy = "<<event->pos().y();
    }
    else
    {
        segment_borders[border_line_num] = ui->widget->yAxis->pixelToCoord(event->pos().y());
        border_line_num++;
        //qDebug()<<"set line, border_line_num = "<<border_line_num;
    }

    if(border_line_num == 2)            //  - если 2 линии уже расставлены на графике
    {
        if(segment_borders[0] > segment_borders[1])     //  - для инвариантности порядка фиксации линий границ
        {
            segment_borders[0] += segment_borders[1];
            segment_borders[1] = segment_borders[0] - segment_borders[1];
            segment_borders[0] = segment_borders[0] - segment_borders[1];
        }
        // меняем границы отображения графика
        if(ui->ZoomOnX->isChecked())
        {
            ui->widget->xAxis->setRange(segment_borders[0],segment_borders[1]);
            ui->widget->yAxis->setRange((ui->widget->yAxis->range()).lower,(ui->widget->yAxis->range()).upper);
        }
        else
        {
            ui->widget->xAxis->setRange((ui->widget->xAxis->range()).lower,(ui->widget->xAxis->range()).upper);
            ui->widget->yAxis->setRange(segment_borders[0],segment_borders[1]);
        }
        // удаляем линии границ, перестраиваем график и заново вызываем функцию состояния ячеек
        ui->widget->removePlottable(ui->widget->plottableCount() - 1);
        ui->widget->removePlottable(ui->widget->plottableCount() - 1);
        border_lines[0] = nullptr;
        border_lines[1] = nullptr;
        border_line_num = 0;
        ui->widget->replot();
        if(ui->ZoomOnX->isChecked())
        {
            on_ZoomOnX_stateChanged(1);
        }
        else
        {
            on_ZoomOnY_stateChanged(1);
        }
    }

}
/*
 -----------------------------------------------------------------------------------------------
 функция
 -----------------------------------------------------------------------------------------------
*/
void grafik::on_DispCoordValue_stateChanged(int index_of_graf)
{
    if(!ui->GrafList2->isVisible())
    {
        ui->GrafList2->setVisible(true);
    }
    //qDebug()<<"DispCoord Start count plot = "<<ui->widget->plottableCount();
    if(ui->DispCoordValue->isChecked())
    {
        if(!first_zoom)
        {
            disconnect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMove(QMouseEvent*)));      //  - отсоединяем сигналы
            disconnect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePress(QMouseEvent*)));
        }

        connect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMoveToShowGrafValue(QMouseEvent*)));
        connect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePressToShowGrafValue(QMouseEvent*)));
        connect(ui->GrafList2, SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeDisplayedGraf(int)));

        first_zoom = true;
        //if(border_line_num != 0 && border_lines[border_line_num] != nullptr)  // - поставь условие как сделано ниже, только вместо 0 1
        if(border_lines[1] != nullptr)
        {
            ui->widget->removePlottable(ui->widget->plottableCount() - 1);
            border_lines[1] = nullptr;
            border_line_num--;
            //qDebug()<<"----first count plot = ----"<<ui->widget->plottableCount();
        }

        if(border_lines[0] != nullptr && (ui->widget->plottableCount() > (quantity_of_grafs + 1)))    /* + 1 т.к есть еще 1 график пиков */ // - в некоторых случаях, при отсутствии экземпляра border line в списке отображаемых графиков, сам border line не равен nullptr!!!!
        {
            ui->widget->removePlottable(ui->widget->plottableCount() - 1);
            //qDebug()<<"----second count plot = ----"<<ui->widget->plottableCount();
        }
        ui->widget->replot();
        vertline = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
        vertline->setPen(QPen(QColor(201,53,42)));
        addPlottable(vertline);
        tracer = new QCPItemTracer(ui->widget);
        //qDebug()<<"1 count plot = "<<ui->widget->plottableCount();
        tracer->setGraph(ui->widget->graph(0));
        ui->GrafList2->setCurrentIndex(order_of_grafs[0]);                              //  - выбираем в выпадающем списке график, который хранится первым в массиве номеров графиков, т.к при повторном включении режима масштабирования индекс текущего элемента в списке не совпадает с графиком, к которому прикреплен tracer
        //qDebug()<<"2 count plot = "<<ui->widget->plottableCount();
        tracer->setPen(QPen(QColor(46,216,219)));
    }
    else
    {
        for(int i = 0; i < text_items_lst.count(); i++)         //  - удаляем информационные окна с координатами и линии (лучи) соединения точки с окном
        {
            delete text_items_lst.at(i);
            delete line_items_lst.at(i);
        }
        text_items_lst.clear();
        line_items_lst.clear();
        ui->GrafList2->setVisible(false);
        //qDebug()<<"вызвался деструктор!";
        qDebug()<<"first count plot = "<<ui->widget->plottableCount();
        disconnect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMoveToShowGrafValue(QMouseEvent*)));
        disconnect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePressToShowGrafValue(QMouseEvent*)));
        disconnect(ui->GrafList2,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeDisplayedGraf(int)));
        if(ui->widget->plottableCount() - (quantity_of_grafs + 1) != 0)        /* + 1 т.к есть еще 1 график пиков */
        {
            ui->widget->removePlottable(ui->widget->plottableCount() - 1);
            qDebug()<<"second count plot = "<<ui->widget->plottableCount();
        }
        vertline = nullptr;
        delete tracer;
        ui->widget->replot();
        if(ui->ZoomOnX->isChecked())
        {
            on_ZoomOnX_stateChanged(1);
        }
        if(ui->ZoomOnY->isChecked())
        {
            on_ZoomOnY_stateChanged(1);
        }
    }
}
/*
 -----------------------------------------------------------------------------------------------
 функция, в зависимости от выбранных графиков для построения (в выпадающем списке с галочками),
включает/отключает отображение соответствующих графиков в выпадающем списке строк в режиме
отображения координат точек графика
 -----------------------------------------------------------------------------------------------
*/
void grafik::ChangeNumberOfDisplayedGrafs()
{
    for(int i = 0; i < tpolar->nfun; i++)
    {
        model_for_sel_grfs_to_show_coord->item(i)->setEnabled(false);
    }
    for(int i = 0; i < quantity_of_grafs; i++)
    {
        model_for_sel_grfs_to_show_coord->item(order_of_grafs[i])->setEnabled(true);
    }
}
/*
 -----------------------------------------------------------------------------------------------
 функция вызывается при смене выбранного графика в выпадающем списке в режиме отображения координат,
 принимает номер выбранного графика из списка, находит номер этого графика на координатной плоскости
 и меняет у объекта tracer прикрепленный к нему график
 -----------------------------------------------------------------------------------------------
*/
void grafik::ChangeDisplayedGraf(int index_of_selected_graf)
{
    int graf_position = 0;
    for(int i = 0; i < quantity_of_grafs; i++)
    {
        if(index_of_selected_graf == order_of_grafs[i])
        {
            graf_position = i;
            break;
        }
    }
    delete tracer;
    tracer = new QCPItemTracer(ui->widget);
    tracer->setGraph(ui->widget->graph(graf_position));
    tracer->setPen(QPen(QColor(46,216,219)));
}
/*
 -----------------------------------------------------------------------------------------------
 функция
 -----------------------------------------------------------------------------------------------
*/
void grafik::SlotMouseMoveToShowGrafValue(QMouseEvent *event)
{
        double coordx = ui->widget->xAxis->pixelToCoord(event->pos().x());
        tracerMoveToShowGrafValue(coordx);
}


void grafik::tracerMoveToShowGrafValue(double coordx)
{
    if(!(QApplication::mouseButtons()))
    {
        QVector<double> x(2),y(2);
        x[0] = coordx;
        y[0] = ui->widget->yAxis->range().lower;
        x[1] = coordx;
        y[1] = ui->widget->yAxis->range().upper;
        vertline->setData(x,y);

        qDebug()<<"tracer pos1 = "<<tracer->position->key()<<" coordx = "<<coordx;
        tracer->setGraphKey(coordx);
        ui->widget->replot();
        qDebug()<<"tracer pos2 = "<<tracer->position->key()<<" coordx = "<<coordx;
        //QString freq_and_level = (tpolar->params[2].replace("freq",QString::number(tracer->position->key()))).replace("level",QString::number(tracer->position->value()));
        for(int i = 0; i < tpolar->params.size(); i++)
        {
            if(tpolar->params[i].contains("freq"))
            {
                QString freq_and_level = tpolar->params[i];
                model_for_displaying_info->item(i)->setText((freq_and_level.replace("freq",QString::number(tracer->position->key()))).replace("level",QString::number(tracer->position->value())));
                continue;
            }
        }
        ui->widget->replot();
    }
}
/*
 -----------------------------------------------------------------------------------------------
 функция
 -----------------------------------------------------------------------------------------------
*/
void grafik::SlotMousePressToShowGrafValue(QMouseEvent *event)
{
    int current_num_of_graf = ui->GrafList2->currentIndex();
    QCPItemText *textItem = new QCPItemText(ui->widget);
    double x = tracer->position->key();
    double y = tracer->position->value();
    textItem->setText(QString(" X: %1\n Y: %2").arg(x,0,'f',3).arg(y,0,'f',3));
    textItem->position->setCoords(QPointF(x, y));
    textItem->setFont(QFont(font().family(), 8));
    textItem->setPen(QPen(color_grf[current_num_of_graf],1.3));
    textItem->setBrush(color_inform_window_background[current_num_of_graf]);
    textItem->setTextAlignment(Qt::AlignLeft);
    QCPItemLine *beam = new QCPItemLine(ui->widget);
    double tracer_x = tracer->position->key(), tracer_y = tracer->position->value();
    beam->start->setParentAnchor(textItem->bottom);
    beam->end->setCoords(tracer_x, tracer_y);
    beam->setHead(QCPLineEnding(QCPLineEnding::esDisc,3));
    beam->setPen(QPen(color_grf[current_num_of_graf]));

    text_items_lst.append(textItem);
    line_items_lst.append(beam);

    disconnect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMoveToShowGrafValue(QMouseEvent*)));
    disconnect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePressToShowGrafValue(QMouseEvent*)));
    connect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMoveToDragInfoWindow(QMouseEvent*)));
    connect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePressToDragInfoWindow(QMouseEvent*)));
}
/*
 -----------------------------------------------------------------------------------------------
 функция
 -----------------------------------------------------------------------------------------------
*/
void grafik::SlotMouseMoveToDragInfoWindow(QMouseEvent *event)
{
    double x = ui->widget->xAxis->pixelToCoord(event->pos().x());
    double y = ui->widget->yAxis->pixelToCoord(event->pos().y());
    text_items_lst.last()->position->setCoords(x, y);
    ui->widget->replot();
}
/*
 -----------------------------------------------------------------------------------------------
 функция
 -----------------------------------------------------------------------------------------------
*/
void grafik::SlotMousePressToDragInfoWindow(QMouseEvent *event)
{
    double x = ui->widget->xAxis->pixelToCoord(event->pos().x());
    double y = ui->widget->yAxis->pixelToCoord(event->pos().y());
    text_items_lst.last()->position->setCoords(x, y);
    ui->widget->replot();

    connect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMoveToShowGrafValue(QMouseEvent*)));
    connect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePressToShowGrafValue(QMouseEvent*)));
    disconnect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(SlotMouseMoveToDragInfoWindow(QMouseEvent*)));
    disconnect(ui->widget,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(SlotMousePressToDragInfoWindow(QMouseEvent*)));
}
/*
 -----------------------------------------------------------------------------------------------
 функция
 -----------------------------------------------------------------------------------------------
*/
void grafik::closeEvent(QCloseEvent *event)
{
    //emit closeGrafik();
    // event->accept();
}
/*
 -----------------------------------------------------------------------------------------------
 функция отключения всех инструментов и сброса всех параметров к состоянию по умолчанию
 -----------------------------------------------------------------------------------------------
*/
void grafik::on_Reset_clicked()
{
    ui->ZoomOnX->setChecked(false);
    ui->ZoomOnY->setChecked(false);
    ui->DispCoordValue->setChecked(false);
    ui->MovingAverage->setChecked(false);
    ui->FindPeaks->setChecked(false);
    for (int i = 0; i < tpolar->nfun; i++)
    {
        model_for_selecting_grafs->item(i)->setData(Qt::Checked, Qt::CheckStateRole);
        order_of_grafs[i] = i;
    }
    reset = true;
    scale_changed = false;
    ShowGraf();
}

void grafik::on_MovingAverage_stateChanged(int arg)
{
    if(!(ui->sizeOfAveraging->isVisible()))
    {
        ui->sizeOfAveraging->setVisible(true);
    }

    size_of_averaging = ui->sizeOfAveraging->value();
    int i = 0, j = 0, k;
    if(ui->MovingAverage->isChecked())
    {
        float average_point, artificial_point;
        for (;i < tpolar->nfun; i++)
        {

            for (j = 0; j < tpolar->num_of_points; j++)
            {
                k = 0;
                average_point = tpolar->data_y[i][j];
                for (; k <= size_of_averaging; k++)
                {
                    if((j - k < 0) || (j + k >= tpolar->num_of_points))
                    {
                        break;
                    }
                    if(j > 0 && k > 0)        // - j должен быть > 0, чтобы не выйти за границы массива в первом условии
                    {
//                        if(i == 0 && j > 383 && size_of_averaging > 0)
//                        {
//                            qDebug()<<" tpolar->data_y = "<<tpolar->data_y[i][j];
//                            qDebug()<<" data_grafs[i][j - 1] = "<<data_grafs[i][j - 1];
//                            qDebug()<<"average_dev = "<<5 * average_deviation[i];
//                        }
                        if(tpolar->data_y[i][j] - data_grafs[i][j - 1] > 5 * average_deviation[i])
                        {
                            artificial_point = tpolar->data_y[i][j] - ((tpolar->data_y[i][j] - data_grafs[i][j - 1]) * 2/(1 + 10 / qExp(0.35 * k)));
                            if(2 * artificial_point < tpolar->data_y[i][j - k] + tpolar->data_y[i][j + k])
                            {
                                average_point += tpolar->data_y[i][j - k];
                                average_point += tpolar->data_y[i][j + k];
                                //qDebug()<<"average_point now = "<<average_point/(2*k + 1);
                                continue;
                            }
                            average_point += artificial_point * 2;
                            //qDebug()<<"average_point now = "<<average_point/(2*k + 1);
                        }
                        else
                        {
                            average_point += tpolar->data_y[i][j - k];
                            average_point += tpolar->data_y[i][j + k];
                        }
                    }

                }
                data_grafs[i][j] = average_point/(2*(k - 1) + 1);            // - k - 1 т.к К увеличился на 1 в предыдущем цикле
            }
        }
        if(ui->FindPeaks->isChecked())
        {
           on_FindPeaksButton_clicked();
        }
        else
        {
            ShowGraf();
        }
    }
    else
    {
        for (; i < tpolar->nfun; i++)
        {
            for (j = 0; j < tpolar->num_of_points; j++)
            {
                data_grafs[i][j] = tpolar->data_y[i][j];
            }
        }
        if(!scale_changed)                            //  - если был изменен масштаб графика с помощью ф-ций ZoomOnX/ZoomOnY, то он не будет сбрасываться
        {
            reset = true;
        }
        ui->sizeOfAveraging->setVisible(false);
        if(ui->FindPeaks->isChecked())
        {
           on_FindPeaksButton_clicked();
        }
        else
        {
            ShowGraf();
        }
    }
}

void grafik::on_zoomplus_clicked()
{

   cur_min = cur_min+yshift;   // - или пусть берет текущее значение, чтобы не слетал масштаб
   cur_max = cur_max-yshift;
   yshift = (cur_max-cur_min)*0.25;
   ui->widget->yAxis->setRange(cur_min,cur_max);
   ui->widget->replot();
}

void grafik::on_zoomminus_clicked()
{

    cur_min = cur_min-yshift;
    cur_max = cur_max+yshift;
    yshift = (cur_max-cur_min)*0.25;
    ui->widget->yAxis->setRange(cur_min,cur_max);
    ui->widget->replot();
}

void grafik::on_resetbut_clicked()
{
    ui->widget->yAxis->setRange(yMin,yMax);
    cur_min = yMin;
    cur_max = yMax;
    yshift = (cur_max-cur_min)*0.25;
    scale_changed = false;

    if(ui->ZoomOnX->isChecked() && (border_line_num == 1))
    {
        QVector<double> x(2),y(2);
        x[0] = segment_borders[0];
        x[1] = segment_borders[0];
        y[0] = ui->widget->yAxis->range().lower;
        y[1] = ui->widget->yAxis->range().upper;
        border_lines[0]->setData(x,y);

        if(border_lines[1] != nullptr)
        {
            border_lines[1]->setData(x,y);
        }
    }
    ui->widget->replot();
}

void grafik::on_up_y_scale_but_clicked()
{
    cur_min = cur_min+yshift;
    cur_max = cur_max+yshift;;
    yshift = (cur_max-cur_min)*0.25;
    ui->widget->yAxis->setRange(cur_min,cur_max);
    ui->widget->replot();
}

void grafik::on_down_y_scale_but_clicked()
{
    cur_min = cur_min-yshift;
    cur_max = cur_max-yshift;;
    yshift = (cur_max-cur_min)*0.25;
    ui->widget->yAxis->setRange(cur_min,cur_max);
    ui->widget->replot();
}

void grafik::on_zoomplus_x_clicked()
{
    xcur_min = xcur_min+xshift;
    xcur_max = xcur_max-xshift;
    xshift = (xcur_max-xcur_min)*0.25;
    ui->widget->xAxis->setRange(xcur_min,xcur_max);
    ui->widget->replot();
}

void grafik::on_zoomminus_x_clicked()
{
    xcur_min = xcur_min-xshift;
    xcur_max = xcur_max+xshift;
    xshift = (xcur_max-xcur_min)*0.25;
    ui->widget->xAxis->setRange(xcur_min,xcur_max);
    ui->widget->replot();
}


void grafik::on_right_x_scale_but_clicked()
{
    xcur_min = xcur_min+xshift;
    xcur_max = xcur_max+xshift;
    xshift = (xcur_max-xcur_min)*0.25;
    ui->widget->xAxis->setRange(xcur_min,xcur_max);
    ui->widget->replot();
}


void grafik::on_left_x_scale_but_clicked()
{
    xcur_min = xcur_min-xshift;
    xcur_max = xcur_max-xshift;
    xshift = (xcur_max-xcur_min)*0.25;
    ui->widget->xAxis->setRange(xcur_min,xcur_max);
    ui->widget->replot();
}


void grafik::on_resetbut_x_clicked()
{
    ui->widget->xAxis->setRange(xMin,xMax);
    xcur_min = xMin;
    xcur_max = xMax;
    xshift = (xcur_max-xcur_min)*0.25;
    scale_changed = false;

    if(ui->ZoomOnY->isChecked() && (border_line_num == 1))
    {
        QVector<double> x(2),y(2);
        x[0] = ui->widget->xAxis->range().lower;
        x[1] = ui->widget->xAxis->range().upper;
        y[0] = segment_borders[0];
        y[1] = segment_borders[0];
        border_lines[0]->setData(x,y);

        if(border_lines[1] != nullptr)
        {
            border_lines[1]->setData(x,y);
        }
    }
    ui->widget->replot();
}

void grafik::on_FindPeaks_stateChanged(int arg1)
{
    if(ui->FindPeaks->isChecked())
    {
        ui->FindPeaksWidget->setVisible(true);
    }
    else
    {
        ui->FindPeaksWidget->setVisible(false);
        ShowGraf();
    }
}


void grafik::on_FindPeaksButton_clicked()
{
    float *max, freq_of_max = ui->IntervalBetweenPeaks->text().toFloat();
    int *max_pos, max_count = ui->NumOfPeaks->text().toInt();
    int sign1,sign2, max_size = 0;
    int numgrf = order_of_grafs[0];

    for(int i = 1; i < tpolar->num_of_points - 1; i++)
    {
        //qDebug()<<"i - 1 = "<<data_grafs[numgrf][i - 1]<<" i = "<<data_grafs[numgrf][i]<<" i + 1 = "<<data_grafs[numgrf][i + 1];
        sign1 = (data_grafs[numgrf][i] - data_grafs[numgrf][i - 1])/abs(data_grafs[numgrf][i] - data_grafs[numgrf][i - 1]);
        sign2 = (data_grafs[numgrf][i + 1] - data_grafs[numgrf][i])/abs(data_grafs[numgrf][i + 1] - data_grafs[numgrf][i]);
        if(sign1 == 1 && sign2 == -1)
        {
            max_size++;
        }
    }

    qDebug()<<"max_size = "<<max_size<<"max_count = "<<max_count;
    if(max_count > max_size)
    {
        max_count = max_size;
        ui->NumOfPeaks->setText(QString::number(max_size));
    }
    else if(max_count <= 0)
    {
        max_count = 1;
        ui->NumOfPeaks->setText("1");
    }
    max = new float [max_size];
    max_pos = new int [max_size];

    for(int i = 1, j = 0; i < tpolar->num_of_points - 1; i++)
    {
        sign1 = (data_grafs[numgrf][i] - data_grafs[numgrf][i - 1])/abs(data_grafs[numgrf][i] - data_grafs[numgrf][i - 1]);
        sign2 = (data_grafs[numgrf][i + 1] - data_grafs[numgrf][i])/abs(data_grafs[numgrf][i + 1] - data_grafs[numgrf][i]);
        if(sign1 == 1 && sign2 == -1)
        {
            max[j] = data_grafs[numgrf][i];
            max_pos[j] = i;
            j++;
        }
    }

//    QString str;
//    for (int i =0; i < max_size; i++)
//    {
//        str += QString::number(max[i]);
//    }
//    QMessageBox::about(this,"",str);
    bool permutation_done = true;
    float var_max, var_max_pos;
    for(int i = 0; i < max_size && permutation_done; i++)
    {
        permutation_done = false;
        for(int j = 0; j < max_size - i - 1; j++)
        {
            if(max[j] < max[j + 1])
            {
                var_max = max[j];
                max[j] = max[j + 1];
                max[j + 1] = var_max;
                var_max_pos = max_pos[j];
                max_pos[j] =max_pos[j + 1];
                max_pos[j + 1] = var_max_pos;
                permutation_done = true;
            }
        }
    }

    float *desired_peaks = new float[max_count];
    int *desired_peaks_pos = new int[max_count];
    desired_peaks[0] = max[0];
    desired_peaks_pos[0] = max_pos[0];
    bool less_than_freq_of_max = false;
    int desired_max_size = 1, all_max_size = 1;
    for(int i = 1; all_max_size < max_count; i++)
        {
            if(i < max_size)
            {
                less_than_freq_of_max = false;
                for (int j = 0; j < desired_max_size; j++)
                {
//                    qDebug()<<"max_pos[i] = "<<max_pos[i];
//                    qDebug()<<"max_pos[j] = "<<max_pos[j];
//                    qDebug()<<"result = "<<abs(max_pos[i] - max_pos[j]);
                    if(abs(max_pos[i] - desired_peaks_pos[j]) * tpolar->samp_freq <= freq_of_max)
                    {
                        less_than_freq_of_max = true;
                        break;
                    }
                }
                if(!(less_than_freq_of_max))
                {
                    desired_peaks[desired_max_size] = max[i];
                    desired_peaks_pos[desired_max_size] = max_pos[i];
                    desired_max_size++;
                    all_max_size++;
                }
            }
            else                                                            // - можно убрать else (и all_max_size) т.е просто не заполнять весь массив т.к далее в цикле используется desired_max_size
            {
                desired_peaks[all_max_size] = 0;
                desired_peaks_pos[all_max_size] = 0;
                all_max_size++;
            }
        }
    /*for(int i = 1; i < max_count; i++)
    {
        less_than_freq_of_max = false;
        for (int j = 0; j < i; j++)
        {
            if(abs(max_pos[i] - max_pos[j]) * tpolar->init_x <= freq_of_max)
            {
                less_than_freq_of_max = true;
                desired_peaks[i] = 0;
                desired_peaks_pos[i] = 0;
                break;
            }
        }
        if(!(less_than_freq_of_max))            // - или desired_peaks[i] != 0 (-1), тогда можно убрать less_than
        {
            desired_peaks[i] = max[i];
            desired_peaks_pos[i] = max_pos[i];
            max_size++;
        }
    }*/
    qDebug()<<"done! 3";
    data_peaks = QVector<double>(desired_max_size);
    data_peaks_pos = QVector<double>(desired_max_size);
    for (int i = 0; i < desired_max_size; i++)
    {
        if(desired_peaks[i] != 0)
        {
            data_peaks[i] = desired_peaks[i];
            data_peaks_pos[i] = desired_peaks_pos[i] * tpolar->samp_freq;
        }
    }

    delete[] max;
    delete[] max_pos;
    delete[] desired_peaks;
    delete[] desired_peaks_pos;

    ShowGraf();
}

void grafik::on_SaveDataGrafs_txt_clicked()
{
    int num = tpolar->num_of_points;
    QString chart_data_table_str = "";
    QStringList chart_data_lst;

    chart_data_table_str += "X\t\t";
    for (int i = 0; i < tpolar->nfun; i++)
    {
        chart_data_table_str += tpolar->grfsnames[i]  + "\t\t";
    }
    chart_data_table_str += "\n";

    for (int i = 0; i < num; i++)
    {
        chart_data_table_str += QString::number(tpolar->init_x + tpolar->samp_freq * i) + "\t\t";
        for (int j = 0; j < tpolar->nfun; j++)
        {
            float val = tpolar->data_y[j][i];
            if(val == -300)
            {
                chart_data_table_str += "-300\t\t";
                continue;
            }
            if(val < 0 || val > 100)
            {
                chart_data_table_str += QString::number(val,'f',3) + "\t\t";
                continue;
            }
            chart_data_table_str += QString::number(val,'f',4) + "\t\t";
        }
        chart_data_table_str += "\n";
    }

    chart_data_lst = chart_data_table_str.split("\n",Qt::SkipEmptyParts);
    QString fname = "file"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
    QString path = QFileDialog::getSaveFileName(0,"сохранить как",QCoreApplication::applicationDirPath() + "\\" + fname, QObject::tr("TXT *.txt;; ALL *.*"));

    if(path.isEmpty())
    {
        path = fname;
    }
    QFile file(path);

    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::critical(this,"ошибка", "файл не создан!");
    }
    else
    {
        QTextStream stream(&file);
        for (int i = 0; i < num; i++)
        {
            stream<<chart_data_lst[i] + "\n";
        }

    }

    file.close();
}

/*
 -----------------------------------------------------------------------------------------------
 функция для добавления на рабочую область вспомогательных элементов (линии для масштабирования и
 отображения координат). В качестве параметра принимает указатель на добавляемый объект, добавляет его
 и удаляет его из легенды
 -----------------------------------------------------------------------------------------------
*/
void grafik::addPlottable(QCPAbstractPlottable *plottable)
{
    ui->widget->addPlottable(plottable);
    ui->widget->legend->removeItem(ui->widget->legend->itemCount() - 1);
}



