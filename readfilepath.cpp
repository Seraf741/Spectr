#include "readfilepath.h"
#include "ui_readfilepath.h"
#include "QPushButton"
#include "QFileDialog"
#include "QDir"

ReadFilePath::ReadFilePath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReadFilePath)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),SLOT(okClicked()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),SLOT(close()));
    ui->lineEdit->setText("C:/Users/HP/Desktop/Диплом/Обработка/spectr/spectr_01_d24_07_2020_t03_49_25V_31_08_2020_08_27_10.gxt");
}

ReadFilePath::~ReadFilePath()
{
    delete ui;
}

void ReadFilePath::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(0,QObject::tr("выберите файл"),QDir::homePath(),QObject::tr("файл (*.gxt);; файл (*.txt);; файл (*.*) "));
    ui->lineEdit->setText(path);
}

void ReadFilePath::okClicked()
{
    emit FilePath(ui->lineEdit->text());
    close();
}

