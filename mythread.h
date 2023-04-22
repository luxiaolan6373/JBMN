#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QDebug>
#include <QThread>
#include"lanrenapi.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

class RunScript: public QThread
{
    Q_OBJECT
public:
    explicit RunScript(QThread *parent = nullptr);

signals:

private:

protected:
    void run() override;//override是重写父类的这个函数的意思
    void splitMouseAndKeyboardDatas(vector<JB_MOUSE> &moseList,vector<JB_KEYBOARD> &keyboardList,QString jbText);
    void upALLKeys(int idexKeyboard,vector<JB_KEYBOARD> keyboardList);
    int isInNotUpKeys(int vKey);

};

#endif // MYTHREAD_H
