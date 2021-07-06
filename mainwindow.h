#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void FlashWhite();
    void ReturnBlack();
    void DisplayText(QString text);

private slots:
    void on_progressBar_valueChanged(int value);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

