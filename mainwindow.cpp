#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPoint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFile>
#include <unistd.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    stkCursor(new CursorStack[NUM_BUTTON] ())
{
    cout << "construct MainWindow start." << endl;
    stkEnd = stkCursor + NUM_BUTTON;

    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::retry);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::send);
}

MainWindow::~MainWindow()
{
    cout << "clear MainWindow start." << endl;
    delete ui;
    delete [] ( stkEnd - NUM_BUTTON );
}

void MainWindow::retry(){
    cout << "retry start." << endl;
    delete [] ( stkEnd - NUM_BUTTON );
    stkCursor = new CursorStack[NUM_BUTTON] ();
    stkEnd = stkCursor + NUM_BUTTON;
    ui->textEdit->setText("");

    ofstream fs("pos.json");
    fs.write("",ios::trunc);
    fs.close();
}

void MainWindow::send(){
    cout << "send start." << endl;
    system("bash moveFile.sh");
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(stkCursor != stkEnd){
            auto pos = QCursor::pos();

            stkCursor->writeX(pos.rx());
            stkCursor->writeY(pos.ry());

            if(++stkCursor == stkEnd){
                writeData();
                displayData();
            }
        }
        else if(stkCursor >= stkEnd){
            cerr << "Pointer address overflows." << stkEnd - stkCursor << endl;
        }
    }
}

void MainWindow::writeData(){
    QJsonArray jsonArray;

    // remove old data
    ofstream fs("pos.json");
    fs.write("",ios::trunc);
    fs.close();
    cout << "remove old data OK." << endl;

    // open data
    QFile jsonFile(QDir::currentPath() + "/pos.json");
    if(!jsonFile.open(QIODevice::ReadWrite)){
        cerr << "pos.json open failed." << endl;
    }

    // get x & y
    for(int i = 0;i < NUM_BUTTON; ++i){
        QJsonObject jsonObj;

        jsonObj.insert("x", (stkEnd - NUM_BUTTON + i)->readX());
        jsonObj.insert("y", (stkEnd - NUM_BUTTON + i)->readY());

        jsonArray.append(jsonObj);
    }

    QJsonDocument jsonDoc;

    jsonDoc.setArray(jsonArray);
    if(jsonFile.write(jsonDoc.toJson()) == -1){
        cerr << "jsonFile write failed." << endl;
    }

    jsonFile.flush();
    jsonFile.close();
}

void MainWindow::displayData(){
    QFile jsonFile(QDir::currentPath() + "/pos.json");
    if(!jsonFile.open(QIODevice::ReadWrite)){
        cerr << "pos.json open failed." << endl;
    }

    QByteArray fileData = jsonFile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(fileData));
    QString result("positions: ");

    if(jsonDoc.isArray()){
        QJsonArray jsonArray = jsonDoc.array();

        foreach(const QJsonValue &value, jsonArray){
            QJsonObject jsonObj = value.toObject();

            result += "X: ";
            result += to_string(jsonObj.value("x").toInt()).c_str();
            result += "\tY: ";
            result += to_string(jsonObj.value("y").toInt()).c_str();
            result += "\n";
        }
    }
    jsonFile.close();

    ui->textEdit->setText(result);
}
