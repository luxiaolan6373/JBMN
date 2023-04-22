#include "globalval.h"


WNDPROC lpPrevWndFunc;
LONGLONG intervalMose=0;//上一次鼠标时间
LONGLONG intervalKeyboard=0;//上一次键盘时间
void (*setListCurrentIndex_g)(int row);
void (*upState)();
int (*getListCurrenIndex_g)();//获取当前选中项的函数
bool (*isListCurrenClick_g)(int row);//获取当前选中项是否勾选
bool isRecall=false;//是否回放中
bool isRec=false;//是否录制中
bool isResetRec=false;//是否处于重录状态
int currenIndex;//当前重录选中的索引
vector<int> notUpKeys;//存放还没他按起来的按键数组
int runTime=1;
QString jbText="";//录制的脚本
vector<JB_TYPE> jbList;//脚本流程列表
