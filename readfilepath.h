#ifndef READFILEPATH_H
#define READFILEPATH_H

#include <QDialog>

namespace Ui {
class ReadFilePath;
}

class ReadFilePath : public QDialog
{
    Q_OBJECT

public:
    explicit ReadFilePath(QWidget *parent = nullptr);
    ~ReadFilePath();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ReadFilePath *ui;
signals:
    void FilePath(const QString &str);
private slots:
    void okClicked();

};

#endif // READFILEPATH_H
