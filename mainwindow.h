#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stdio.h>
#include <stdlib.h>
#include <QMouseEvent>
#include <QCursor>
#include "cursorstack.h"

#define NUM_BUTTON 3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void getNowPos();
    void writeData();
    void displayData();

private slots:
    void retry();
    void send();

private:
    Ui::MainWindow *ui;
    CursorStack* stkCursor, *stkEnd;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

};
#endif // MAINWINDOW_H
