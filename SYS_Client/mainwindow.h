#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "global.h"
#include <QAbstractSocket>
#include <QMainWindow>

class QTcpSocket;
class Widget;
class TaskInfoForm;
class ProcessView;
class ResultViewer;
class LoadingDialog;
class QThreadPool;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showWindow(int stage);
    void clientinit(QString ip, int port);
    void windowReload();
    void startTask(TaskInfo info);
    void showStop();
    void queryStart();
    void queryStop();
    void taskEnd();
    //  void readDataThread();
    void readData();
    void parseMsg(QByteArray *buffer);

signals:
    void loginfail();
    //  void readFinish(QByteArray *buffer);

private:
    Ui::MainWindow *ui;
    Widget *login;
    TaskInfoForm *taskinfo = nullptr;
    ProcessView *processView = nullptr;
    ResultViewer *resultViewer = nullptr;
    LoadingDialog *loading = nullptr;
    QTcpSocket *client;
    QByteArray buffer;
    QString m_IP;
    int m_Port;
    // bool showFlag = false; // 是否在获取图片
    TaskInfo information;
    std::list<std::vector<QImage>> showImgBuffer;
    QTimer *queryTimer = nullptr;
    QByteArray pdfArray;

    void disconnect();
    void errorDisplay(QAbstractSocket::SocketError);
    void sendData(int type, QByteArray &message, int subType = 0);
};

#endif // MAINWINDOW_H
