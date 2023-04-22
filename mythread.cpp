#include "mythread.h"


RunScript::RunScript(QThread *parent): QThread(parent)
{



}

int RunScript::isInNotUpKeys(int vKey){

    for (int i=0;i<notUpKeys.size();i++){

        if (notUpKeys[i]==vKey){
            return i;
        }
    }

    return -1;
}
void RunScript::upALLKeys(int idexKeyboard,vector<JB_KEYBOARD> keyboardList){

    for(int i=0;i<=idexKeyboard;i++){
        if(keyboardList[i].type=1){
            //如果没有按过则加入
            if(isInNotUpKeys(keyboardList[i].vmkey)==-1){
                notUpKeys.push_back(keyboardList[i].vmkey);
            }

        }
    }
    for (int i=0;i<notUpKeys.size();i++){

        lanrenAPI::keyboardUp(notUpKeys[i]);
    }
    //清空数组
    notUpKeys.clear();



}
void RunScript::run(){
    //这里写模拟过程
    for(int i=0;i<runTime;i++){//多少次
        vector<JB_MOUSE>   moseList;
        vector<JB_KEYBOARD>   keyboardList;

        for(int j=0;j<jbList.size();j++){
            if (isRecall==false){
                break;
            }
            //没勾选的不运行
            if  (isListCurrenClick_g(j)==false){
                continue;
            }
            //选中运行的脚本
            setListCurrentIndex_g(j);
            splitMouseAndKeyboardDatas(moseList,keyboardList,jbList[j].jbText);
            int lengthMouse=moseList.size();
            int lengthKeyboard=keyboardList.size();

            //明天继续  接下来就是模拟操作;
            int idexMouse=0;
            int idexKeyboard=0;
            bool jumpMouse=false;//忽略鼠标开关
            bool jumpKeyboard=false;//忽略键盘开关
            bool okMouse=false;//鼠标条件通过开关 反向
            bool okKeyboard=false;//键盘条件通过开关 反向

            LARGE_INTEGER endTime;
            LARGE_INTEGER startTimeMouse;
            LARGE_INTEGER startTimeKeyboard;


            QueryPerformanceCounter(&startTimeKeyboard);
            startTimeMouse=startTimeKeyboard;
            while (lengthKeyboard-1>=idexKeyboard || lengthMouse-1>=idexMouse) {

                if(jumpMouse==true && jumpKeyboard==true){
                    break;

                }
                //开关
                if (isRecall==false){

                    //这里把还没弹起来的按键弹起来
                    if(lengthKeyboard>0){

                        if(idexKeyboard>lengthKeyboard-1){

                            idexKeyboard=lengthKeyboard-1;
                        }
                        upALLKeys(idexKeyboard,keyboardList);



                    }
                    break;
                }

                if(lengthMouse-1<idexMouse){
                    jumpMouse=true;
                    okMouse=true;
                }
                if(lengthKeyboard-1<idexKeyboard){
                    jumpKeyboard=true;
                    okKeyboard=true;
                }

                //获取当前cpu计时
                QueryPerformanceCounter(&endTime);

                //鼠标 -------------------
                if(jumpMouse==false){

                    okMouse=endTime.QuadPart-startTimeMouse.QuadPart<moseList[idexMouse].sleepTime;

                }

                if(okMouse==false){

                    startTimeMouse=endTime;
                    //qDebug()<<" okMouse"<<okMouse<<" vmkey:"<<moseList[idexMouse].vmkey<<" sleepTime"<<moseList[idexMouse].sleepTime<<" idexKeyboard"<<idexMouse;

                    if (moseList[idexMouse].type!=9){


                        if (moseList[idexMouse].type==0){
                            //相对移动鼠标
                            lanrenAPI::mouseMoveR(moseList[idexMouse].dx,moseList[idexMouse].dy);

                        }else{
                            if(moseList[idexMouse].vmkey==4){
                                lanrenAPI::mouseRoll(moseList[idexMouse].rollingDistance);

                            }else{
                                lanrenAPI::mouseClick(moseList[idexMouse].vmkey,moseList[idexMouse].type);
                            }
                        }
                    }
                    idexMouse++;
                }

                //键盘 ------------------------
                if(jumpKeyboard==false){
                    okKeyboard=endTime.QuadPart-startTimeKeyboard.QuadPart<keyboardList[idexKeyboard].sleepTime;
                }


                if(okKeyboard==false){

                    startTimeKeyboard=endTime;
                    //qDebug()<<"okKeyboard:"<<okKeyboard<<" vmkey:"<<keyboardList[idexKeyboard].vmkey<<" sleepTime"<<keyboardList[idexKeyboard].sleepTime<<" idexKeyboard"<<idexKeyboard;


                    if (keyboardList[idexKeyboard].type!=9){

                        if (keyboardList[idexKeyboard].type==1){

                            lanrenAPI::keyboardDown(keyboardList[idexKeyboard].vmkey);
                        }else{
                            lanrenAPI::keyboardUp(keyboardList[idexKeyboard].vmkey);
                        }


                    }

                    idexKeyboard++;


                }




            }
        }
    }

    isRecall=false;
    upState();


}

void RunScript::splitMouseAndKeyboardDatas(vector<JB_MOUSE> &moseList,vector<JB_KEYBOARD> &keyboardList,QString jbText){

    QStringList result;
    JB_MOUSE moseData;
    JB_KEYBOARD keyboardData;
    int lx = 0;

    //全部清空
    moseList.clear();
    keyboardList.clear();
    //分割文本 用换行符分割
    result=jbText.split("\n");
    for(int i=0;i<result.length();i++){


        QStringList result_2;
        //用空格分割
        result_2=result[i].split(" ");

        if(result_2.length()<2){
            continue;
        }


        if(result_2[0]=="鼠标初始")
        {
            lanrenAPI::mouseMove(result_2[1].toInt(),result_2[2].toInt());
            lx =0;
        }else if(result_2[0]=="鼠标"){
            moseData.vmkey=result_2[1].toInt();
            moseData.type=result_2[2].toInt();
            moseData.dx=result_2[3].toInt();
            moseData.dy=result_2[4].toInt();
            moseData.rollingDistance=result_2[5].toInt();
            moseList.push_back(moseData);
            lx =0;
            //qDebug()<<moseData.vmkey<<moseData.type<<moseData.dx<<moseData.dy<<moseData.sleepTime<<moseData.rollingDistance;

        }else if(result_2[0]=="等待鼠"){
            lx = lx + 1;
            moseData.sleepTime=result_2[1].toInt();
            if(lx>=2){
                moseData.type=9;
                moseList.push_back(moseData);
                lx =0;
            }
        }else if(result_2[0]=="按键"){
            //通过名字获取虚拟键值
            keyboardData.vmkey=lanrenAPI::keyTextToInt(result_2[1]);
            keyboardData.type=result_2[2].toInt();
            keyboardList.push_back(keyboardData);
            lx =0;
            //qDebug()<<"按键"<<keyboardList[keyboardList.size()-1].vmkey<<keyboardList[keyboardList.size()-1].type<<keyboardList[keyboardList.size()-1].sleepTime<<result_2[1];
        }else if(result_2[0]=="等待键"){
            lx = lx + 1;
            keyboardData.sleepTime=result_2[1].toInt();
            if(lx>=2){
                keyboardData.vmkey=0;
                keyboardData.type=9;
                keyboardList.push_back(keyboardData);
                lx =0;
            }
        }else if(result_2[0]=="---标记"){
            keyboardData.vmkey=lanrenAPI::keyTextToInt(result_2[1]);
            keyboardData.type=result_2[2].toInt();
            keyboardList.push_back(keyboardData);
            lx =0;

        }

    }






}


