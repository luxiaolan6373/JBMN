#include "lanrenapi.h"


lanrenAPI::lanrenAPI()
{


}
lanrenAPI::~lanrenAPI()
{


}

std::map<int, QString> lanrenAPI::keyMap = {
    {1, "Ctrl键状态"},
    {2, "Shift键状态"},
    {3, "Break键"},
    {4, "Alt键状态"},
    {8, "退格键"},
    {9, "Tab键"},
    {13, "回车键"},
    {16, "Shift键"},
    {17, "Ctrl键"},
    {18, "Alt键"},
    {19, "Pause键"},
    {20, "CapsLock键"},
    {27, "Esc键"},
    {32, "空格键"},
    {33, "PageUp键"},
    {34, "PageDown键"},
    {35, "End键"},
    {36, "Home键"},
    {37, "左光标键"},
    {38, "上光标键"},
    {39, "右光标键"},
    {40, "下光标键"},
    {45, "Ins键"},
    {46, "Del键"},

//    {48, "键0"},
//    {49, "键1"},
//    {50, "键2"},
//    {51, "键3"},
//    {52, "键4"},
//    {53, "键5"},
//    {54, "键6"},
//    {55, "键7"},
//    {56, "键8"},
//    {57, "键9"},

    {48, "0键"},
    {49, "1键"},
    {50, "2键"},
    {51, "3键"},
    {52, "4键"},
    {53, "5键"},
    {54, "6键"},
    {55, "7键"},
    {56, "8键"},
    {57, "9键"},

    {65, "A键"},
    {66, "B键"},
    {67, "C键"},
    {68, "D键"},
    {69, "E键"},
    {70, "F键"},
    {71, "G键"},
    {72, "H键"},
    {73, "I键"},
    {74, "J键"},
    {75, "K键"},
    {76, "L键"},
    {77, "M键"},
    {78, "N键"},
    {79, "O键"},
    {80, "P键"},
    {81, "Q键"},
    {82, "R键"},
    {83, "S键"},
    {84, "T键"},
    {85, "U键"},
    {86, "V键"},
    {87, "W键"},
    {88, "X键"},
    {89, "Y键"},
    {90, "Z键"},

    {112, "F1键"},
    {113, "F2键"},
    {114, "F3键"},
    {115, "F4键"},
    {116, "F5键"},
    {117, "F6键"},
    {118, "F7键"},
    {119, "F8键"},
    {120, "F9键"},
    {121, "F10键"},
    {122, "F11键"},
    {123, "F12键"},
    {124, "F13键"},
    {125, "F14键"},
    {126, "F15键"},
    {127, "F16键"},

    {144, "NumLock键"},
    {145, "ScrollLock键"},
    {186, "分号键"},
    {187, "等号键"},
    {188, "逗号键"},
    {189, "减号键"},
    {190, "圆点键"},
    {191 ,"除号键"},
    {192, "反撇号键"},
    {219, "左中括号键"},
    {220, "右中括号键"},
    {221, "斜杠键"},
    {222, "单引号键"},

};


bool lanrenAPI::setWindowsMousePenetrate(HWND hwnd,bool setPenetrate){
    bool ret;
    if(setPenetrate){
        int a=GetWindowLongA(hwnd,GWL_EXSTYLE);
        a=a|32|524288;
        ret=SetWindowLongA(hwnd,GWL_EXSTYLE,a);

    }else{
        ret=SetWindowLongA(hwnd,GWL_EXSTYLE,524288);
    }
    return ret;

}

WINBOOL lanrenAPI::hookRawInputData(HWND hwnd, int ridev){
    RAWINPUTDEVICE rawinputdevice;
    rawinputdevice.dwFlags=RIDEV_INPUTSINK;//不在激活状态也接收数据
    rawinputdevice.usUsage=ridev;
    rawinputdevice.usUsagePage=HID_USAGE_PAGE_GENERIC;//通用桌面控件
    rawinputdevice.hwndTarget=hwnd;
    return RegisterRawInputDevices(&rawinputdevice,1,sizeof(rawinputdevice));
}

QString lanrenAPI::getNowTimeText(){

    time_t t = time(nullptr);
      struct tm* now = localtime(&t);
      char str[50];
      strftime(str, sizeof(str), "%m月%d日%H:%M:%S", now);//%Y年
      return (QString)str;
}

int lanrenAPI::keyTextToInt(QString keytext)
{
    auto it = std::find_if(keyMap.begin(), keyMap.end(), [keytext](const auto& pair) {
        return pair.second == keytext;
    });

    if (it != keyMap.end()) {
        int key = it->first;
        return key;
    } else {
        return 0;
    }

}

QString lanrenAPI::keyIntToText(int keyInt)
{
    // 通过键值查找键名
    return lanrenAPI::keyMap[keyInt];
}
int lanrenAPI::splitText(const QString& text, const QChar delimiter, QStringList& result) {
    int len = text.length(), cnt = 0, start = 0;
    for (int i = 0; i < len; i++) {
        if (text[i] == delimiter) {
            result.append(text.mid(start, i - start));
            start = i + 1;
        }
    }
    result.append(text.mid(start, len - start));
    return cnt;
}

UINT lanrenAPI::keyboardDown(UINT key)
{
    INPUT lpinput[1];

    lpinput[0].type=INPUT_KEYBOARD;
    lpinput[0].ki.wVk=key;
    lpinput[0].ki.dwFlags=0;

    return SendInput(1,lpinput,sizeof(lpinput));

}

UINT lanrenAPI::keyboardUp(UINT key)
{



    INPUT lpinput[1];
    lpinput[0].type=INPUT_KEYBOARD;
    lpinput[0].ki.wVk=key;
    lpinput[0].ki.dwFlags=KEYEVENTF_KEYUP;

    return SendInput(1,lpinput,sizeof(lpinput));
}


UINT lanrenAPI::mouseClick(UINT key, UINT type)
{
    INPUT lpinput[1];
    lpinput[0].type=INPUT_MOUSE;
    lpinput[0].mi.mouseData=0;
    lpinput[0].mi.time=0;
    lpinput[0].mi.dwExtraInfo=0;
    switch (key){

    case(1):
        if (type==1){
            lpinput[0].mi.dwFlags=MOUSEEVENTF_LEFTDOWN;
        }else{
            lpinput[0].mi.dwFlags=MOUSEEVENTF_LEFTUP;
        }
        break;
    case(2):
        if (type==1){
            lpinput[0].mi.dwFlags=MOUSEEVENTF_RIGHTDOWN;
        }else{
            lpinput[0].mi.dwFlags=MOUSEEVENTF_RIGHTUP;
        }
        break;

    case(3):
        if (type==1){
            lpinput[0].mi.dwFlags=MOUSEEVENTF_MIDDLEDOWN;
        }else{
            lpinput[0].mi.dwFlags=MOUSEEVENTF_MIDDLEUP;
        }
        break;
    }

    return SendInput(1,lpinput,sizeof(lpinput));

}

UINT lanrenAPI::mouseMoveR(UINT dx, UINT dy)
{

    INPUT lpinput[1];
    lpinput[0].type=INPUT_MOUSE;
    lpinput[0].mi.dx=dx;
    lpinput[0].mi.dy=dy;
    lpinput[0].mi.mouseData=0;
    lpinput[0].mi.time=0;
    lpinput[0].mi.dwExtraInfo=0;
    lpinput[0].mi.dwFlags=MOUSE_MOVED;
    return SendInput(1,lpinput,sizeof(lpinput));
}

UINT lanrenAPI::mouseMove(UINT x, UINT y)
{

    UINT nScreenWidth, nScreenHeight;
    nScreenWidth = (UINT)GetSystemMetrics(SM_CXSCREEN);
    nScreenHeight = (UINT)GetSystemMetrics(SM_CYSCREEN);
    INPUT lpinput[1];
    lpinput[0].type=INPUT_MOUSE;
    lpinput[0].mi.dx=65535.0f/(nScreenWidth-1)*x;
    lpinput[0].mi.dy=65535.0f/(nScreenHeight-1)*y;
    lpinput[0].mi.mouseData=0;
    lpinput[0].mi.time=0;
    lpinput[0].mi.dwExtraInfo=0;
    lpinput[0].mi.dwFlags=MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    return SendInput(1,lpinput,sizeof(lpinput));

}

UINT lanrenAPI::mouseRoll(short d)
{
    INPUT lpinput[1];
    lpinput[0].type=INPUT_MOUSE;
    lpinput[0].mi.mouseData=d;
    lpinput[0].mi.time=0;
    lpinput[0].mi.dwExtraInfo=0;
    lpinput[0].mi.dwFlags=MOUSEEVENTF_WHEEL;
    return SendInput(1,lpinput,sizeof(lpinput));

}

