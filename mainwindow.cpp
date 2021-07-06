#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CalThread* worker = new CalThread();
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker, &CalThread::FlashWhite, this, &MainWindow::FlashWhite, Qt::BlockingQueuedConnection);
    connect(worker, &CalThread::ReturnBlack, this, &MainWindow::ReturnBlack, Qt::BlockingQueuedConnection);
    connect(worker, SIGNAL(DisplayText(QString)), this, SLOT(DisplayText(QString)), Qt::BlockingQueuedConnection);
    worker->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FlashWhite()
{
    this->setStyleSheet("background-color: white");
    ui->label->setStyleSheet("color: black;");
}

void MainWindow::ReturnBlack()
{
    this->setStyleSheet("background-color: black");
    ui->label->setStyleSheet("color: white;");
    ui->pushButton->setStyleSheet("QPushButton { background-color: light grey; }\n"
                      "QPushButton:enabled { background-color: light grey; }\n");
}

void MainWindow::DisplayText(QString text)
{
    ui->label->setText(text);
}
