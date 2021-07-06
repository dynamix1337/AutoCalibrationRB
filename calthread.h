#ifndef CALTHREAD_H
#define CALTHREAD_H

#include <QApplication>
#include <QMainWindow>
#include <QSoundEffect>
#include <QThread>

#include "guitar_control.h"

#define AUDIO_SAMPLE_SIZE   10
#define VIDEO_SAMPLE_SIZE   10
#define HID_REPORT_SIZE     0x1C
#define URB_TIMEOUT         7 /* in ms */

class CalThread : public QThread
{
    Q_OBJECT
    void run();

public:
    CalThread();
    ~CalThread();

public slots:
    void PlaySoundEffect();

signals:
    void FlashWhite();
    void ReturnBlack();
    void DisplayText(QString);
    void progressBar_valueChanged(int);

private:
    QSoundEffect* my_chirpSound;
};

#endif // CALTHREAD_H
