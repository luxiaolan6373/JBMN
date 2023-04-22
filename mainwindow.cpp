#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMenu>
#include<QSettings>
#include<QInputDialog>
#include<QScrollBar>
#include<QProcessEnvironment>
MainWindow *pThis;//MainWindow 指针
//创建线程
RunScript* runscript;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    pThis=this;//将自身对象暴露给外部全局变量指针,这样在回调函数中就可以访问了
    upState = pThis->updataState;
    setListCurrentIndex_g = pThis->setListCurrentIndex;
    isListCurrenClick_g=pThis->isListCurrenClick;
    getListCurrenIndex_g=pThis->getListCurrenIndex;
    //禁止项目编辑
    ui->tw_process->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //水平表格头显示和隐藏
    ui->tw_process->horizontalHeader()->setVisible(false);
    //垂直表格头显示和隐藏
    ui->tw_process->verticalHeader()->setVisible(false);
    //设置窗口透明度:
    this->setWindowOpacity(0.8);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    //信号绑定
    connect(ui->bt_load,&QPushButton::clicked,this,&clicked_bt_load);
    connect(ui->bt_save,&QPushButton::clicked,this,&clicked_bt_save);
    connect(ui->bt_save_as,&QPushButton::clicked,this,&clicked_bt_save_as);
    connect(ui->bt_replace,&QPushButton::clicked,this,&clicked_bt_replace);
    connect(ui->cb_history,&QComboBox::activated,this,&activated_cb_history);

    //绑定一些特殊功能信号
    connect(this, &MainWindow::updataText_sig, this, &MainWindow::updataText_sig_call);
    connect(this, &MainWindow::updataJBList_sig, this, &MainWindow::updataJBList_sig_call);
    connect(this, &MainWindow::updataState_sig, this, &MainWindow::updataState_sig_call);
    connect(this, &MainWindow::setListCurrentIndex_sig, this, &MainWindow::setListCurrentIndex_sig_call);
    connect(ui->tw_process,&QTableWidget::doubleClicked,this,&MainWindow::doubleClicked_tw_process);

    //获取窗口的句柄
    HWND hwnd=(HWND)(this->winId());

    //hook原始输入数据 鼠标
    if (lanrenAPI::hookRawInputData(hwnd,HID_USAGE_GENERIC_MOUSE)==0){
        QMessageBox::information(this,"错误!","创建原始鼠标数据设备失败!");
    }
    //hook原始输入数据 键盘
    if (lanrenAPI::hookRawInputData(hwnd,HID_USAGE_GENERIC_KEYBOARD)==0){
        QMessageBox::information(this,"错误!","创建原始键盘数据设备失败!");
    }

    //建立消息机制,收到数据会通知
    lpPrevWndFunc=(WNDPROC)SetWindowLongPtr(hwnd,GWLP_WNDPROC,(LONG_PTR)&func_Message);



    // 初始化菜单和菜单项
    m_menu = new QMenu(ui->tw_process);
    m_actionReRecord = new QAction("重选录制", this);
    m_actionDeleteSelected = new QAction("删除选中", this);
    m_actionSelectOnlyThis = new QAction("只选这个", this);
    m_actionSelectStartThis = new QAction("从这开始", this);
    m_actionSelectAll = new QAction("全部选中", this);
    m_actionSelectNone = new QAction("全部不选", this);
    m_actionRename = new QAction("重命名", this);
    // 添加复选框到重选录制菜单项
    m_actionReRecord->setCheckable(true);
    // 将菜单项添加到菜单中
    m_menu->addAction(m_actionReRecord);
    m_menu->addAction(m_actionDeleteSelected);
    m_menu->addAction(m_actionSelectOnlyThis);
    m_menu->addAction(m_actionSelectStartThis);
    m_menu->addAction(m_actionSelectAll);
    m_menu->addAction(m_actionSelectNone);
    m_menu->addAction(m_actionRename);

    // 连接菜单项的槽函数
    connect(m_actionReRecord, &QAction::triggered, this, &MainWindow::onReRecord);
    connect(m_actionDeleteSelected, &QAction::triggered, this, &MainWindow::onDeleteSelected);
    connect(m_actionSelectOnlyThis, &QAction::triggered, this, &MainWindow::onSelectOnlyThis);
    connect(m_actionSelectStartThis, &QAction::triggered, this, &MainWindow::onSelectStartThis);
    connect(m_actionSelectAll, &QAction::triggered, this, &MainWindow::onSelectAll);
    connect(m_actionSelectNone, &QAction::triggered, this, &MainWindow::onSelectNone);
    connect(m_actionRename, &QAction::triggered, this, &MainWindow::onRename);

    // ...

    // 在ui->tw_process上右键弹出菜单
    ui->tw_process->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tw_process, &MainWindow::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);


//    //加载配置
//    QSettings settings("setting.ini", QSettings::IniFormat);

//    QString cb_list=settings.value("cb_list").toString();
//    if (cb_list!=""){
//        ui->cb_history->addItems(cb_list=settings.value("cb_list").toStringList());
//    }






}

MainWindow::~MainWindow()

{
    delete ui;
}
void MainWindow::activated_cb_history(int index){
    QString fileName=ui->cb_history->currentText().split("|")[1];
    if (fileName==""){
        return ;
    }
    QFile file(fileName);
    qDebug()<<fileName;
    //设置打开方式
    file.open(QIODeviceBase::ReadOnly);
    QString ttt;
    while(!file.atEnd()){
        QByteArray array= file.readLine();
        QTextCodec * codec=QTextCodec::codecForName(GetCorrectUnicode(array).toLatin1());
        ttt+=codec->toUnicode(array);
    }
    ui->pte_jb->setPlainText(ttt);
    //关闭文件
    file.close();
    updataJBList();
}
void MainWindow::doubleClicked_tw_process(const QModelIndex &index){
    // 获取QPlainTextEdit控件的垂直滚动条
    QScrollBar *scrollBar = ui->pte_jb->verticalScrollBar();

    // 设置滚动条的值，使其滚动到第100行
    scrollBar->setValue(scrollBar->minimum() + jbList[index.row() ].idex * scrollBar->singleStep());
}

void MainWindow::clicked_bt_load(bool clicked){
    QString _document = QProcessEnvironment::systemEnvironment().value("USERPROFILE")+"\\Desktop";
    QString fileName= QFileDialog::getOpenFileName(this,"导入脚本",_document,tr("TEXT (*.txt)"));
    //读取内容  放到textEdit中
    //默认编码格式是 utf-8

    if (fileName==""){
        return ;
    }
    QFile file(fileName);
    //设置打开方式
    file.open(QIODeviceBase::ReadOnly);
    QString ttt;
    while(!file.atEnd()){
        QByteArray array= file.readLine();
        QTextCodec * codec=QTextCodec::codecForName(GetCorrectUnicode(array).toLatin1());
        ttt+=codec->toUnicode(array);
    }
    //ui->pte_jb->moveCursor(QTextCursor::End);
    //ui->pte_jb->insertPlainText(ttt);
    ui->pte_jb->setPlainText(ttt);
    //关闭文件
    file.close();
    QFileInfo info(fileName);
    ui->cb_history->addItem(info.fileName()+"|"+fileName);
    updataJBList();

}
void MainWindow::clicked_bt_save(bool clicked){

    QString  str=ui->pte_jb->toPlainText();
    char*  ch;
    QByteArray ba = str.toUtf8(); // must
    ch=ba.data();
    if (ui->cb_history->currentText()==""){
        clicked_bt_save_as(false);
        return;
    }
    QFile file(ui->cb_history->currentText().split("|")[1]);
    //写文件
    file.open(QIODevice::WriteOnly);
    file.write(ch);
    file.close();
    updataJBList();




}
void MainWindow::clicked_bt_save_as(bool clicked){
    QString _document = QProcessEnvironment::systemEnvironment().value("USERPROFILE")+"\\Desktop";
    QString fileName= QFileDialog::getSaveFileName(this,"另存脚本",_document,tr("TEXT (*.txt)"));
    if (fileName==""){
        return ;
    }
    QString  str=ui->pte_jb->toPlainText();
    char*  ch;
    QByteArray ba = str.toUtf8(); // must
    ch=ba.data();

    QFile file(fileName);
    QFileInfo info(fileName);
    ui->cb_history->addItem(info.fileName()+"|"+fileName);
    //写文件
    file.open(QIODevice::WriteOnly);
    file.write(ch);
    file.close();

}

void MainWindow::clicked_bt_replace(bool clicked){



}
void MainWindow::updataJBList(){
    emit pThis->updataJBList_sig();
}
void MainWindow::updataJBList_sig_call()
{

    ui->tw_process->setVisible(false);
    jbList.clear();
    int index=0;
    QChar del_1 = '\n';
    QStringList result;

    JB_TYPE jb;
    jb.jbText="";
    jb.idex=0;
    jb.name="";
    //用换行符分割
    result=ui->pte_jb->toPlainText().split("\n");
    //lanrenAPI::splitText(ui->pte_jb->toPlainText(), del_1,result);

    int length=(int)result.length();
    for (int i=0;i<length;i++){

        //index=index+(result[i]+"\n").length();

        if(result[i].indexOf("##")==-1){

            jb.jbText=jb.jbText+result[i]+"\n";

            if(i==length-1){
                jbList.push_back(jb);
                jb.jbText="";
                break;
            }
            if(result[i+1].indexOf("##")!=-1){
                jbList.push_back(jb);
                jb.jbText="";
            }

        }else{
            jb.name=result[i];
            jb.idex=i;
        }

    }


    //将分好的组放入列表中

    ui->tw_process->setRowCount(jbList.size());
    ui->tw_process->setColumnCount(1);
    for (int i=0;i<jbList.size();i++){
        QString name=jbList[i].name;
        QTableWidgetItem * Item =new QTableWidgetItem(name.replace("##",""));
        Item->setCheckState(Qt::Checked);

        ui->tw_process->setItem(i, 0, Item);
        ui->tw_process->setRowHeight(i,30);

    }
    //自动调整大小
    ui->tw_process->setColumnWidth(0,180);

    ui->tw_process->setVisible(true);
}
void MainWindow::updataText(){
     emit pThis->updataText_sig();
}
void MainWindow::updataText_sig_call(){
    QString txt;
    for(int i=0;i<jbList.size();i++){

        txt+=jbList[i].name+"\n"+jbList[i].jbText;

    }
    ui->pte_jb->setPlainText(txt);
}
LRESULT CALLBACK  MainWindow::func_Message(HWND hwnd,UINT uMsg,  WPARAM wParam, LPARAM lParam)
{


    if (uMsg==WM_INPUT){

        UINT dwSize = 48;

        LONGLONG inTimeKeyboard;
        LONGLONG inTimeMouse;
        QString nowTimeText;
        QString keyMouse,typeMouse;
        QString ret;
        POINT lpPint;
        // 第一次调用函数传NULL获取需要的缓冲区大小  已经知道了就直接填48
        //GetRawInputData((HRAWINPUT)lParam, (UINT)RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        //分配指定的缓冲区大小
        LPBYTE lpbBuffer = new BYTE[dwSize];
        // 取数据到缓冲区
        GetRawInputData((HRAWINPUT)lParam, (UINT)RID_INPUT, (LPVOID)lpbBuffer, (PUINT)&dwSize, (UINT)sizeof(RAWINPUTHEADER));

        RAWINPUT* rawdata = (RAWINPUT *)lpbBuffer;
        if(rawdata->header.dwType==RIM_TYPEKEYBOARD){

            if (isRecall ==false){
                inTimeKeyboard=updateIntervalKeyboard();
            }

            switch(rawdata->data.keyboard.VKey){

            case(VK_F8):{//按下F8开始录制

                if(rawdata->data.keyboard.Message==WM_KEYUP){

                    if(isRec==false)
                    {
                        if(isRecall==true){
                            goto endCall;
                        }


                        isRec =true;
                        isRecall=false;
                        nowTimeText=lanrenAPI::getNowTimeText();
                        GetCursorPos(&lpPint);//获取当前鼠标相对于桌面的位置
                        if(isResetRec==false){

                            jbText=pThis->ui->pte_jb->toPlainText();
                            jbText+="##"+nowTimeText+"##"+"\n鼠标初始 "+QString::number(lpPint.x)+" "+QString::number(lpPint.y)+"\n";
                        }else{
                            jbText= "鼠标初始 "+QString::number(lpPint.x)+" "+QString::number(lpPint.y)+"\n";
                        }
                        pThis->updataState();
                        LARGE_INTEGER a;
                        QueryPerformanceCounter(&a);
                        intervalKeyboard=a.QuadPart;
                        intervalMose=a.QuadPart;

                    }else{
                        isRec =false;
                        isRecall=false;

                        jbText+="等待键 "+QString::number(updateIntervalKeyboard())+"\n";
                        jbText+="等待鼠 "+QString::number(updateIntervalMouse())+"\n";

                        if (isResetRec==false){
                            pThis->ui->pte_jb->setPlainText(jbText);
                        }else{

                            jbList[currenIndex].jbText=jbText;
                            pThis->updataText();
                        }

                        pThis->updataState();
                        pThis->updataJBList();
                        if(isResetRec==true){

                            pThis->setListCurrentIndex(currenIndex);
                            pThis->onSelectOnlyThis();
                        }
                    }

                }
                break;
            }
            case(VK_F9):{//按下F9开始回放

                if(rawdata->data.keyboard.Message==WM_KEYUP){
                    if(isRecall==false)
                    {
                        if(isRec==true){
                            goto endCall;
                        }

                        isRecall=true;
                        isRec=false;
                        pThis->updataState();
                        runTime=pThis->ui->led_time->text().toInt();
                        jbText=pThis->ui->pte_jb->toPlainText();
                        runscript=new RunScript;
                        //启动线程
                        runscript->start();




                    }else{
                        isRecall=false;
                        isRec=false;
                        pThis->updataState();
                    }
                }
                break;

            }//case end

            case(VK_F7):{//按下F7

                if(rawdata->data.keyboard.Message==WM_KEYUP){
                    pThis->ui->pte_jb->clear();
                    jbText="";
                    pThis->ui->tw_process->clear();
                }
                break;

            }//case end
            case(VK_F10):{//按下F10

                if(rawdata->data.keyboard.Message==WM_KEYUP){

                    jbText+="等待键 "+QString::number(inTimeKeyboard)+"\n---标记 弹起--\n";

                }else{
                    jbText+="等待键 "+QString::number(inTimeKeyboard)+"\n---标记 按下--\n";

                }
                break;

            }//case end

            case(VK_F11):{//按下F11
                if(rawdata->data.keyboard.Message==WM_KEYUP && isRec==true){

                    isRec=false;
                    isRecall=false;
                    pThis->updataState();
                }
                break;
            }//case end


            default:{
                if(isRec==false){
                    goto endCall;
                }


                ret="\n按键 "+lanrenAPI::keyIntToText(rawdata->data.keyboard.VKey)+" "+QString::number(rawdata->data.keyboard.Message-255);
                jbText+="等待键 "+QString::number(inTimeKeyboard)+ret+"\n";

                break;
            }
            }//switch end



        }else if(rawdata->header.dwType==RIM_TYPEMOUSE){
            if (isRecall ==false){
                inTimeMouse=updateIntervalMouse();
            }
            switch (rawdata->data.mouse.usButtonFlags){

            case(0):{

                keyMouse="0";
                typeMouse="0";
                break;
            }//case_end
            case(1):{
                keyMouse="1";
                typeMouse="1";
                break;
            }//case_end
            case(2):{
                keyMouse="1";
                typeMouse="2";
                break;
            }//case_end
            case(4):{
                keyMouse="2";
                typeMouse="1";
                break;
            }//case_end
            case(8):{
                keyMouse="2";
                typeMouse="2";
                break;
            }//case_end
            case(16):{
                keyMouse="3";
                typeMouse="1";
                break;
            }//case_end
            case(32):{
                keyMouse="3";
                typeMouse="2";
                break;
            }//case_end
            case(1024):{
                keyMouse="4";
                typeMouse="3";
                break;
            }//case_end
            default:

                goto endCall;
                break;

            }//switch_end

            if(isRec==false){
                goto endCall;
            }
            //转成有符号的
            short wheelDelta = static_cast<short>(rawdata->data.mouse.usButtonData);
            if (wheelDelta!=0){
                keyMouse="4";
                typeMouse="3";
            }

            ret="\n鼠标 "+keyMouse+" "+typeMouse+" "+QString::number(rawdata->data.mouse.lLastX)+" "+QString::number(rawdata->data.mouse.lLastY)+" "+QString::number(wheelDelta);
            jbText+="等待鼠 "+QString::number(inTimeMouse)+ret+"\n";
        }

    }

endCall:


    return CallWindowProcA (lpPrevWndFunc, hwnd, uMsg, wParam, lParam);
}
void MainWindow::setListCurrentIndex(int row){
    emit pThis->setListCurrentIndex_sig(row);
}
void MainWindow::setListCurrentIndex_sig_call(int row){
    try{
        pThis->ui->tw_process->setCurrentCell(row, 0);
    }
    catch(...){
        qDebug() << "未选中任何目标";
    }
}
int MainWindow::getListCurrenIndex(){
    int a;
    a=pThis->ui->tw_process->currentIndex().row();

    if (a<0){
        a=0;
    }
    return a;


}
bool MainWindow::isListCurrenClick(int row){
    int b;
    b=pThis->ui->tw_process->rowCount();

    if (b<=0){
        return false;
    }
     return pThis->ui->tw_process->item(row,0)->checkState();




}
void  MainWindow::updataState(){
    emit pThis->updataState_sig();
}
void  MainWindow::updataState_sig_call(){

    HWND hwnd=(HWND)(pThis->winId());
    if (isRec==false && isRecall==false){
        pThis->ui->lb_state->setText("待机中");
        pThis->setWindowOpacity(0.8);
        pThis->ui->lb_state->setStyleSheet("color: rgb(0, 255, 255);");
        lanrenAPI::setWindowsMousePenetrate(hwnd,false);

    }else if(isRec==true && isRecall==false){
        if(isResetRec){
            pThis->ui->lb_state->setText("重录中");
        }else{
            pThis->ui->lb_state->setText("录制中");
        }
        pThis->setWindowOpacity(0.6);
        pThis->ui->lb_state->setStyleSheet("color: rgb(0, 255, 0);");

        lanrenAPI::setWindowsMousePenetrate(hwnd,true);

    }else if(isRec==false && isRecall==true){
        pThis->ui->lb_state->setText("回放中");
        pThis->setWindowOpacity(0.6);
        pThis->ui->lb_state->setStyleSheet("color: rgb(255, 255, 0);");
        lanrenAPI::setWindowsMousePenetrate(hwnd,true);
    }else{
        isRecall=false;
        isRec=false;
        pThis->ui->lb_state->setText("待机中");
        pThis->setWindowOpacity(0.8);
        pThis->ui->lb_state->setStyleSheet("color: rgb(0, 255, 255);");
        lanrenAPI::setWindowsMousePenetrate(hwnd,false);

    }
}
//自动判断编码格式
QString MainWindow::GetCorrectUnicode(const QByteArray &ba)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode( ba.constData(), ba.size(), &state);
    if (state.invalidChars > 0)
    {
        text = QTextCodec::codecForName( "GBK" )->toUnicode(ba);
        return QString("GBK");
    }
    else
    {
        text = ba;
        return QString("UTF-8");
    }
}

//右键弹出菜单
void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tw_process->indexAt(pos);
    if (index.isValid()) {
        m_menu->exec(ui->tw_process->viewport()->mapToGlobal(pos));
    }
}
// 重选录制
void MainWindow::onReRecord()
{

    // TODO: 实现重选录制功能
    // 判断是否勾选了重选录制菜单项
       if(m_actionReRecord->isChecked())
       {
           // 执行勾选操作
           // ...
           isResetRec=true;
           onSelectOnlyThis();
           this->setWindowTitle(this->windowTitle()+" 选中重录状态");
           currenIndex=this->getListCurrenIndex();
       }
       else
       {
           // 执行取消勾选操作
           // ...
           isResetRec=false;
           this->setWindowTitle("懒人躺宝3.0");
       }
}

// 删除选中
void MainWindow::onDeleteSelected()
{
    // TODO: 实现删除选中功能
    int index=MainWindow::getListCurrenIndex();
    //索引往上移动
    if (index+2<=jbList.size()){
        jbList[index+1].idex=jbList[index].idex;
    }
    ui->tw_process->removeRow(index);
    jbList.erase(jbList.begin() + index);
    updataText();
}

// 只选这个
void MainWindow::onSelectOnlyThis()
{
    // TODO: 实现只选这个功能
    int index=MainWindow::getListCurrenIndex();
    for (int i=0;i<ui->tw_process->rowCount();i++){
        if(i==index){
            ui->tw_process->item(i,0)->setCheckState(Qt::Checked);

        }else{
            ui->tw_process->item(i,0)->setCheckState(Qt::Unchecked);

        }

    }

}
// 从这开始
void MainWindow::onSelectStartThis()
{
    // TODO: 实现只选这个功能
    int index=MainWindow::getListCurrenIndex();
    for (int i=0;i<ui->tw_process->rowCount();i++){
        if(i>=index){
            ui->tw_process->item(i,0)->setCheckState(Qt::Checked);

        }else{
            ui->tw_process->item(i,0)->setCheckState(Qt::Unchecked);

        }

    }

}

// 全部选中
void MainWindow::onSelectAll()
{
    // TODO: 实现全部选中功能
    for (int i=0;i<ui->tw_process->rowCount();i++){

        ui->tw_process->item(i,0)->setCheckState(Qt::Checked);
    }
}

// 全部不选
void MainWindow::onSelectNone()
{
    // TODO: 实现全部不选功能
    for (int i=0;i<ui->tw_process->rowCount();i++){

        ui->tw_process->item(i,0)->setCheckState(Qt::Unchecked);
    }
}

// 重命名
void MainWindow::onRename()
{
    // TODO: 实现重命名功能

    bool bOk = false;
    int idex=MainWindow::getListCurrenIndex();
        QString sName = QInputDialog::getText(this,
                                             "重命名",
                                             "请输入姓新名字\nQQ群224684635",
                                             QLineEdit::Normal,
                                             ui->tw_process->item(idex,0)->text(),
                                             &bOk
                                             );

        if (bOk && !sName.isEmpty()) {
            ui->tw_process->item(idex,0)->setText(sName);
            jbList[idex].name="##" + sName+"##";
            MainWindow::updataText();

        }
    }





//更新鼠标间隔
LONGLONG  updateIntervalMouse(){
    static LARGE_INTEGER a;
    static LONGLONG ret;
    QueryPerformanceCounter(&a);
    ret= a.QuadPart-intervalMose;
    intervalMose=a.QuadPart;
    return ret;
}
//更新键盘间隔
LONGLONG updateIntervalKeyboard(){
    static LARGE_INTEGER a;
    static LONGLONG ret;
    QueryPerformanceCounter(&a);
    ret= a.QuadPart-intervalKeyboard;
    intervalKeyboard=a.QuadPart;
    return ret;
}
