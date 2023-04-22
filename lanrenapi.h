#ifndef LANRENAPI_H
#define LANRENAPI_H
#include "windows.h"
#include <winuser.h>
#include <hidusage.h>
#include"globalval.h"
#include"QString"
#include <ctime>
#include <QStringList>
#include <map>



class lanrenAPI
{
public:
    lanrenAPI();
    ~lanrenAPI();

    //设置鼠标穿透
    static bool setWindowsMousePenetrate(HWND hwnd, bool setPenetrate=true);
    /*实时获取鼠标,键盘或其它设备的原始数据
     0x01	指针	HID_USAGE_GENERIC_POINTER
     0x02	鼠	HID_USAGE_GENERIC_MOUSE
     0x04	操纵杆	HID_USAGE_GENERIC_JOYSTICK
     0x05	游戏手柄	HID_USAGE_GENERIC_GAMEPAD
     0x06	键盘	HID_USAGE_GENERIC_KEYBOARD
     0x07	键盘	HID_USAGE_GENERIC_KEYPAD
     0x08	多轴控制器	HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER*/
    static WINBOOL hookRawInputData(HWND hwnd, int ridev=2);
    static QString getNowTimeText();
    static int keyTextToInt(QString keytext);
    static QString keyIntToText(int keyInt);
    static int splitText(const QString& text, const QChar delimiter, QStringList& result);
    //键盘按下
    static UINT keyboardDown(UINT key);
    //键盘弹起
    static UINT keyboardUp(UINT key);
    //鼠标点击  key 1 左键 2 右键 3 中键  type 1按下 2弹起
    static UINT mouseClick(UINT key,UINT type);
    //鼠标相对移动
    static UINT mouseMoveR(UINT dx,UINT dy);
    //鼠标绝对移动
    static UINT mouseMove(UINT x,UINT y);
    //鼠标滚轮滚动
    static UINT mouseRoll(short d);

private:

    static std::map<int, QString> keyMap;

};

#endif // LANRENAPI_H



























