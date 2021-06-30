#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <chrono>
#include <iostream>
#include <unistd.h>
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QSoundEffect>
#include <QString>
#include <QThread>
#include <QWidget>

#include "guitar_control.h"

#define AUDIO_SAMPLE_SIZE   15
#define VIDEO_SAMPLE_SIZE   10
#define HID_REPORT_SIZE     0x1C
#define URB_TIMEOUT         7 /* in ms */

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CalThread : public QThread
{
    Q_OBJECT

public:
    void run();
    CalThread();
    ~CalThread();

public slots:
    void PlaySoundEffect();

signals:
    void finished();
    void Flash();

private:
    // store the sound effect, so we can reuse it multiple times
    QSoundEffect* my_chirpSound;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void Flash();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

