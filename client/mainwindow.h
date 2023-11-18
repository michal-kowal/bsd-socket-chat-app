#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QTcpSocket>
#include <QByteArray>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "structures.h"
#include "dataHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

protected:
    QTcpSocket * socket {nullptr};
    void connection();
    void socketConnected();
    void receivePacket();
    void loginUser();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
