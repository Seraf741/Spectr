#ifndef SPCTR_DATA_PROCESSING_H
#define SPCTR_DATA_PROCESSING_H

#include <QMainWindow>
#include <QtCore/qmath.h>
#include <QDialog>
#include "shablon.h"
#include <qapplication.h>
#include "QDebug"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //СЃС‚СЂСѓРєС‚СѓСЂР° РґР»СЏ С…СЂР°РЅРµРЅРёСЏ РґР°РЅРЅС‹С…, СЃС‡РёС‚Р°РЅРЅС‹С… РёР· С„Р°Р№Р»Р°
    typedef struct
    {
        int			nfun;     /* С‡РёСЃР»Рѕ РіСЂР°С„РёРєРѕРІ */
        float		k;
        float       init_x;       //РЅР°С‡Р°Р»СЊРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ С…
        float       samp_freq;   //СЂР°СЃСЃС‚РѕСЏРЅРёРµ РјРµР¶РґСѓ СЃРѕСЃРµРґРЅРёРјРё С‚РѕС‡РєР°РјРё РЅР° РѕСЃРё С…
        float       **data_y;   //РјР°СЃСЃРёРІ РґР»СЏ С…СЂР°РЅРµРЅРёСЏ С‡РёСЃРµР» РёР· dtx С„Р°Р№Р»Р°
        int         num_of_points; //С‡РёСЃР»Рѕ С‚РѕС‡РµРє РґР»СЏ РіСЂР°С„РёРєР°
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
        float		xmin;//РјРёРЅ РїРѕ РѕСЃРё С…
        float		dfi; //РїРѕР»РѕСЃР° (РїСЂРѕРјРµР¶СѓС‚РѕРє РїРѕ РѕСЃРё y)
        float		ref; // РѕС‚РЅРѕСЃРёС‚РµР»СЊРЅР°СЏ РєР°РєРѕР№ РІРµР»РёС‡РёРЅС‹ РёРґРµС‚ РёР·РјРµСЂРµРЅРёРµ (РґР»СЏ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ РІ Р”Р±)
        float		dlog;// РґР»СЏ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёСЏ РІ
        float		ugolz;
        float		ugolx;
        int			tipdb;
        int			radius;		/* СЂР°Р·РјРµСЂ СЃРёРіРЅР°Р»Р° */
        int			numgraf;	/* РЅРѕРјРµСЂ С„РёР»СЊС‚СЂР° */
        float		*addrold;
        float		*addr;
        bool        oktav_mode = 0;
        QString mode_exe;
        //TImage		*PolarGrafic;

        //TRichEdit	*Zagolovok;
        int			mode;
        //char		name[600];
        QString     name;
        QStringList grfsnames;  //  - РЅР°Р·РІР°РЅРёСЏ РіСЂР°С„РёРєРѕРІ
        QStringList params;    //  - РїР°СЂР°РјРµС‚СЂС‹ РїСЂРѕРіСЂР°РјРјС‹
    } tPolar;

public slots:
    void processingSpectrData(tworkParamstruct, tworkstruct);
    void transferChartData(float **data);

signals:
    void FileisOpen(const QString& path);             // СЃРёРіРЅР°Р» РІС‹Р·С‹РІР°РµС‚ С„СѓРЅРєС†РёСЋ, СЃРѕС…СЂР°РЅСЏСЋС‰СѓСЋ РґР°РЅРЅС‹Рµ РёР· С„Р°Р№Р»Р° РІ СЃС‚СЂСѓРєС‚СѓСЂСѓ
    void BuildGraf(MainWindow::tPolar &tpolar, bool);
    void FileReadyToRead(const QString& path);
    void ReadyToBuild();

private slots:
    void ReadFile(const QString &str);
    void on_OpenFile_triggered();
    void SaveDataFile(const QString& path);

    void on_BuildGraph_triggered(float **data);

private:
    Ui::MainWindow *ui;
    tworkParamstruct struct_work_param;
    tworkstruct struct_work;
    bool params_data_have_read = false;
    bool params_data_changed = false;

};
#endif // SPCTR_DATA_PROCESSING_H
