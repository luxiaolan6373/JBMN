
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include<QFileDialog>
#include<QFile>
#include<QMessageBox>
#include<QTextCodec>
#include<QFileInfo>
#include"lanrenapi.h"
#include"mythread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //自动判断编码格式
    QString GetCorrectUnicode(const QByteArray &ba);
    void clicked_bt_load(bool clicked);
    void clicked_bt_save(bool clicked);
    void clicked_bt_save_as(bool clicked);
    void clicked_bt_replace(bool clicked);
    void updataJBList();
    void updataText();
    static void setListCurrentIndex(int row);
    static int getListCurrenIndex();
    static bool isListCurrenClick(int row=-1);
    static void  updataState();

    static LRESULT CALLBACK  func_Message(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);





private:
    Ui::MainWindow *ui;
    HWND hwnd;
    QMenu* m_menu; // 菜单
    QAction* m_actionReRecord; // 重选录制
    QAction* m_actionDeleteSelected; // 删除选中
    QAction* m_actionSelectStartThis; // 只选这个
     QAction* m_actionSelectOnlyThis; // 只选这个
    QAction* m_actionSelectAll; // 全部选中
    QAction* m_actionSelectNone; // 全部不选
    QAction* m_actionRename; // 重命名
    void onDeleteSelected();
    void onSelectOnlyThis();
    void onSelectStartThis();
    void onSelectAll();
    void onSelectNone();
    void onRename();
    void onCustomContextMenu();
    void onReRecord();
    void onCustomContextMenuRequested(const QPoint &pos);
    //实现槽函数
    void setListCurrentIndex_sig_call(int row);
    void updataJBList_sig_call();
    void updataText_sig_call();
    void updataState_sig_call();
    void doubleClicked_tw_process(const QModelIndex &index);
    void activated_cb_history(int index);

signals:
    void setListCurrentIndex_sig(int row);
    void updataJBList_sig();
    void updataText_sig();
    void  updataState_sig();




};
static LONGLONG updateIntervalMouse();
static LONGLONG updateIntervalKeyboard();
#endif // MAINWINDOW_H



