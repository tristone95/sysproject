#ifndef TCPCOMMUNICATION_H
#define TCPCOMMUNICATION_H

#include "global.h"
#include <QObject>

class QTcpServer;
class QTcpSocket;
class QThread;
class Calculation;
class QTimer;

class TcpCommunication : public QObject {
    Q_OBJECT
public:
    explicit TcpCommunication();
    ~TcpCommunication();
    void acceptConnection();
    void reConnect();
    void disconnect();
    void errorDisplay();
    void parseMsg(QByteArray &buffer);
    void readData();
    void sendData(int type, QByteArray &message, int subType = 0);
    void taskEnd();
    void initialTask();
    void reportProgress();
    void progressOnly(QByteArray progress);
    void SendDataPerSec(int sec);
    void saveShowImg(const ImagesPerSec &Sec, int processedImgNum,
                     int totalImgNum);
    void stopGet();
    void sendResult();
signals:
    void taskStart();
    void taskStop();

private:
    QTcpServer *server;
    QTcpSocket *socket;
    int port;
    std::vector<QImage> showImgBuf;                 // 预览图片保存
    int prog[2] = {0, 0};                           // 处理进度保存,0存储已处理，1存储总张数
    QByteArray buffer;                              // socket消息存储
    QThread *taskThread;                            // 任务处理线程
    Calculation *task;                              // 图像处理对象
    QTimer *quaryTimer;                             // 进度查询定时器
    QByteArray m_pdf;                               // pdf报告
    ResultInfo m_result = {0, 0, 0, 0, 0, 0, 0, 0}; // 结果页数据
    bool autoStop = false;
};

#endif // TCPCOMMUNICATION_H
