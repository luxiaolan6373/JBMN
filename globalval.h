#ifndef GLOBALVAL_H
#define GLOBALVAL_H
#include <QMainWindow>
#include "windows.h"
#include <winuser.h>
#include<vector>

using namespace std;
// 定义一个函数指针类型
typedef void (*FunctionPointer)();
//自定义数据类型
struct JB_MOUSE {
    int vmkey;
    int type;
    int sleepTime;
    int dx;
    int dy;
    int rollingDistance;

};
struct JB_KEYBOARD {
    int vmkey;
    int type;
    int sleepTime;
};
struct JB_TYPE {
    QString name;
    QString jbText;
    int idex;
};



extern void (*upState)();//全局改变状态的函数
extern void (*setListCurrentIndex_g)(int row);//全局改变超级列表框选中项的函数
extern int (*getListCurrenIndex_g)();//获取当前选中项的函数
extern bool (*isListCurrenClick_g)(int row);//获取当前选中项是否勾选
extern WNDPROC lpPrevWndFunc;
extern LONGLONG intervalMose;//上一次鼠标时间
extern LONGLONG intervalKeyboard;//上一次键盘时间
extern int currenIndex;//当前重录选中的索引

extern bool isRecall;//是否回放中
extern bool isRec;//是否录制中
extern bool isResetRec;//是否处于重录状态
extern int runTime;
extern vector<int> notUpKeys;//存放还没他按起来的按键数组
extern QString jbText;//录制的脚本
extern vector<JB_TYPE> jbList;//脚本流程列表
#endif // GLOBALVAL_H
