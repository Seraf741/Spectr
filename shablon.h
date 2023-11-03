#ifndef shablonH
#define shablonH
//#include <windows.h>
#define  FDCGMPAS 8
#define FN_0KT 10.
#include <QFileInfo>
#include <QDateTime>
#include <utime.h>
// ---------------------------------------------------------------------------
typedef struct {
	float ugol1;
	float ugol2;
} tSectorAngle;
typedef struct { // информация о заднных параметрах
	int tip; // режим что считаем
	float tn; // время начальное
	float tk; // время конечное
	float ttus; // текущуее время точки вычисления
	float tt; // текущуее время
	float tint; // время сдвига для последующей
	// точки вычисления
	float timl; // время усреднения
	float fn; // нижняя частота
	float fk; // верхняя частота
	int n; // размер бпф
	int ns; // перекрытие
	int no; // окно
	int l; // число усреднений
	int lstat; // усреднение для вур
	int nint; // число точек сдвига для последующей
	// точки вычисления
	float dugol; // шаг по углу для вур
	float tugol; // телесный угол для вур
	float ugol; // поворот по оси х
	float ugolz; ; // поворот по оси z
	int n_var_work; // текущий номер прохода
	int count_var_work; // число проходов
	float f1_3; // частота фильтрации
	int TipSignal; // тип сигнала 0-вещест 1-комплексный
	int Popr_compas; // поправка на компас 0-нет
	int compas; // компас  1-есть  0-нет
	int Kontur_out;
	int winspectr; // спектр посчитан
	int winsono; // сонограмма посчитана
	int winwur; // вур посчитана
	int winprochod; // проходная посчитана
	int wincontur; // номер контура для вур
	int winfiltr; // 1- 1/3 форма с частотами готова
	int winaniz; //
	int winspmax; // к-во окон
	int GrafCountAniz; // к-во графиков
	int size_out; // к-во выходных точек f
	int numt; // к-во выходных точек t
	int ntabl_contur;
	// HWND hwin[5]; // указатели на временные окна
	// HWND hwinSpec[5];  // указатели на спектральные окна
	int *faniz; //
	int chur; //
	tSectorAngle Sector;
	float *fnreg; //
	float *fvreg; //
	bool obrp; // обработка для вур с учетом знаков для потока
    QString snum;
    // float xstep[5]; //
	// float Sxstep[5];  //
	// int ModeNow;   //
} tworkParamstruct;
typedef tworkParamstruct *workParam_HANDLE;
typedef struct { // информация о входных сигналах
    QStringList *fname;           // имя файла
    QStringList *fname_compiter2; // имя файлов для второй итерации компаса
    QStringList *pfile; // имя файла описателя *.anp
    QStringList *pfilefch; // имя файла описателя *.fch
    QStringList *ancom; // коментарий
    QString namecompas; // имя файла  с данными компаса
    QString namecompas2; // имя файла  с данными второго  компаса
    QString formatsig; // формат сигнала
	float gain[4]; // усиление
	float resv[4]; // средняя чувствительность
	int channel[4]; // каналы
	int scaledb[4]; // шкала записи ачх
	int n_popr_fch[4];
	int compas_exit;
	float fd; // частота квантования входного сигнала
	float del; // шкала деления
	int nSignSmpl[4]; // число отсчетов в сигнале
	double fSignLen[4]; // длина сигнала,c
	float *modeP;
	float LbFlts[4]; // нижняя  граница полосы фильтрации
	float HbFlts[4]; // верхняя граница полосы фильтрации
	float LbFltsMax; // Max нижняя  граница полосы фильтрации
	float HbFltsMin; // Min верхняя граница полосы фильтрации
	float CentrFreq; // центральная частота
	float ref; // относительные db
	float tmi; // задержка
	int Povorot_Contur; // наклон оси z
	float T_Dist;
    QString dat; // дата
    QString tim; // время
	int SigFilter; // >0 сигнал фильтрованный
    QString *namedevvp; // тип измеряемого vp
	int numdevvp; // номер vp
    QString *namedevgg; // тип измеряемого gg
	int numdevgg; // номер gg
    QString path;
} tworkstruct;

//typedef struct { // информация о выбранных в UI параметрах из лога
//    bool aniz_check;
//    bool checkbox1;
//    bool checkbox_tab;
//    bool teneghtckeck;
//    bool Pr_Filtr;
//    bool Count_Kontur_Speed ;
//    bool Build_Kontur;
//    bool File_Build_Kontur;
//    bool Nkontur1;
//    bool Nkontur2;
//    bool In_Contur;
//    bool Out_Contur;
//    bool P_Dist;
//    int dfaz;
//    int dugol;
//    int f;
//    int lstat;
//    int n;
//    int nint;
//    int n0;
//    int ns;
//    int rtravers;
//    //int rega;
//    //int regw;
//    int sr;
//    int sector_ugol1;
//    int sector_ugol2;
//    int tn;
//    int tk;
//    int tr;
//    int ttr1;
//    int ttr2;
//    int ugol;
//    int ugol1;
//    int ugolz;
//    int v1;
//    int v2;
//    float *fn_aniz;
//    float *fv_aniz;
//    int f_aniz_size;
//    float *f1_3check;
//    int f1_3_size;
//    float fk;
//    float fn;
//    QString namedevvp;
//}tuistructure;
typedef tworkstruct *works_HANDLE;
/* typedef struct
 {
 float FrqCm;
 float RcvHd;
 float RcvRl;
 float RcvPi;
 } tCompHead; */
typedef struct {
    quint32 tmH;
    quint32 tmL;
    float CmHd; // азимут
	float CmRl; // крен
	float CmPi; // тангаж
} tCompAngle;
typedef struct {
	float fi;
	float time;
} tContur;

enum moderegim {
    SONOGRAMMA = 3,   //SPECTR | CIKL_TIME
    WUR = 10,         // ANIZ=8 | CIKL_TIME
    PROCHOD = 6,      //A_TIME | CIKL_TIME
    DIAGRAMMA = 4102, // A_TIME | CIKL_TIME | DIAGRAMMA_KONTUR
    SPECTR_MAX = 19,  // SPECTR |SP_MAX |CIKL_TIME
    FILTR_1_3_OKT = 512,
    ANIZATROPI = 264,        //CIKL_FR | ANIZ
    ANIZ_REAL = 8456,        //CIKL_FR | AN_REAL
    CHUR = 1288,             // USR_FR | CIKL_FR | ANIZ
    SPECTR_SECTOR = 3336,    //SECTOR | USR_FR | CIKL_FR | ANIZ
    SONOGRAMMA_SECTOR = 3338 //SECTOR | USR_FR | CIKL_FR | ANIZ |CIKL_TIME
};

enum modework {
	NO_WORK, WORK_CIKL, WORK_CIKLV, STOP_WORK
};

enum modeanalis {
	LINANAL = 0, OKT1_3 = 64
};

enum modecompas {
	COMPAS_NO, COMPAS_YES = 5
};

enum modefilt {
	FILTRSIG_NO = 0, FILTRSIG_YES = 128
};

enum modeusredn {
	LIN_USR, EXSP_USR
};

enum modsos {
	SPECTR = 1, CIKL_TIME = 2, A_TIME = 4,
	// вычисление А через мощность фильтра
		ANIZ = 8, SP_MAX = 16, KONTUR = 32, CIKL_FR = 256, USR_FR = 1024,
	SECTOR = 2048, DIAGRAMMA_KONTUR = 4096,
};

enum modezap {
	REAL, COMPLEX
};

enum moderesult {
	RESULT_NO, RESULT_YES
};

enum modenamfilt {
	FILTR_NO, FILTR_LOW, FILTR_1_3, FILTR_OKT
};

enum modeout {
	ABS_WX, ABS_WY, ABS_WZ, W_XY, W_XYZ, W_ZP, ABS_WZM, W_YZ, W_YZP, W_YX
};

enum modekan {
	KAN_X, KAN_Y, KAN_Z, KAN_P
};

enum mode_line_kontur {
	LINE_1_CONTUR = 1, LINE_2_CONTUR = 2
};

works_HANDLE Initw(int tip);
const int MAX_PATH_QT = 512;
void freework(works_HANDLE work);
void transpon_matr(QString FileName, int n, int m);
// ---------------------------------------------------------------------------
#endif
