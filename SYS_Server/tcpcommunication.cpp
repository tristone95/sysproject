#include "tcpcommunication.h"
#include "calculation.h"
#include "generaterepo.h"
#include "global.h"
#include <QBuffer>
#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

struct Msg {
    int type;
    // 可变区，记录阶段或秒数或 第一段数据长度
    int subType;
    int dataLength;
};
TaskInfo information;
Msg recvMsg = {0, 0, 0}; // 接收消息

int taskStage = 1;       // 记录当前任务阶段
// 1：无任务 2：图片采集 3：颗粒计算 4：统计完成，结果展示
TcpCommunication::TcpCommunication() {
    port = 2468;
    server = new QTcpServer;
    if (!server->listen(QHostAddress::LocalHost, port)) {
        qDebug() << "listen failed,请重启服务器！";
        return;
    };
    showImgBuf.reserve(50);
    // vector预扩容
    qDebug() << "lisening";
    connect(server, &QTcpServer::newConnection, this,
            &TcpCommunication::acceptConnection);
}

TcpCommunication::~TcpCommunication() {
    socket->disconnectFromHost();
    server->close();
    server->deleteLater();
}

// 初始化任务信息
void TcpCommunication::initialTask() {
    // taskInfo
    std::cout << "Task: " << information.taskId << " Completed." << std::endl
              << "-----------------------------------------------------" << std::endl;
    taskStage = 1;
    showImgBuf.clear();
    prog[0] = prog[1] = 0;
    buffer.clear();
    m_pdf.clear();
    m_result = {0, 0, 0, 0, 0, 0, 0, 0};
    information = {true, 50, 99999999, 0, 0, "", "", "", "", ""};
    autoStop = false;
}
// 任务完成，清空指针释放旧任务空间,初始化变量
void TcpCommunication::taskEnd() {
    delete quaryTimer;
    // 线程退出
    taskThread->quit();
    taskThread->wait();
    // 销毁线程
    delete taskThread;
}

// 客户端请求响应，向客户端发送进度
void TcpCommunication::reportProgress() {
    QByteArray tmp;
    if (taskStage == 2) {
        QDataStream out(&tmp, QDataStream::WriteOnly);
        out.setVersion(QDataStream::Qt_6_2);
        out << prog[0] << prog[1];
        sendData(3, tmp);
    } else if (taskStage == 3) {
        tmp.append((char *)&prog, sizeof(int) * 2);
        sendData(4, tmp);
    }
    // 自动模式张数达标，采集完成
    if (autoStop && prog[1] >= information.taskImgNum &&
        prog[1] != 0) {
        stopGet(); // 停止获取图片，开始查询处理进度
        taskStage = 3;
        autoStop = false;
    }
}

// 从处理进程获取处理进度
void TcpCommunication::progressOnly(QByteArray progress) {
    // 更新成员变量
    std::memcpy(prog, progress.data(), progress.size());
    // 处理完成，任务结束，开始统计
    if (prog[0] >= information.taskImgNum && prog[0] != 0) {
        taskStage = 4;
        std::cout << "Calculation Completed." << std::endl;
        // 告知客户端完成
        QByteArray tmp;
        // 停止从线程更新进度
        quaryTimer->stop();
        // 开始进行统计计算
        GenerateRepo report;
        Info *resStruct = report.countMain(information, *task->getResult(), &m_pdf);
        m_result.granularityPass = resStruct->granularityPass;
        m_result.averageGranularity = resStruct->averageGranularity.toDouble();
        m_result.roundness = resStruct->roundness;
        m_result.sphericity = resStruct->sphericity;
        m_result.apparentDensity = resStruct->apparentDensity;
        m_result.bulkDensity = resStruct->bulkDensity;
        m_result.particleNumTotal = resStruct->particleNumTotal;
        m_result.totalSec = task->getResult()->size();
        std::cout << "Statistics completed." << std::endl;
        // 待计算完后通知客户端，避免未计算完就请求结果
        sendData(4, tmp, 1);
    }
}
// 响应客户端按秒结果请求
// msg.type 05
// msg.subtype 结构体集合长度
// msg.length  总长度
void TcpCommunication::SendDataPerSec(int sec) {
    auto resultAll = task->getResult();
    QByteArray data, imgs;
    data.append((char *)(*resultAll)[sec - 1].data(), (*resultAll)[sec - 1].size() * sizeof(SandInfo));
    QDataStream out(&imgs, QDataStream::WriteOnly);
    for (const SandInfo &sand : (*resultAll)[sec - 1]) {
        // 存入图片
        /*
         * bug记录
         * 在存入byteArray时程序总是崩溃，输出图片全是乱码，怀疑空间被回收
         * mat转QImage,Qimage共享使用的mat的data，mat栈回收导致image数据被回收
         * 解决办法：qimage.copy深拷贝拷贝data
        QString path = "../imgbuffer2/part/" + QString::number(index) + ".jpg";
        (*resultAll)[sec - 1][var].image->save(path); */
        out << *sand.image;
    }
    int data1Size = data.size();
    data.append(imgs);
    sendData(5, data, data1Size);
}

void TcpCommunication::saveShowImg(const ImagesPerSec &Sec, int processedImgNum,
                                   int totalImgNum) {
    showImgBuf.clear();
    for (auto &img : *Sec.data)
        showImgBuf.emplace_back(std::move(img));
    delete Sec.data; // 释放 Sec.data空间
    prog[0] = processedImgNum;
    prog[1] = totalImgNum;
}

void TcpCommunication::stopGet() {
    taskStage = 3;
    emit taskStop();
    // 开启定时器,更新图片处理进度
    quaryTimer = new QTimer;
    quaryTimer->setTimerType(Qt::PreciseTimer);
    connect(quaryTimer, &QTimer::timeout, task, &Calculation::getProgress);
    connect(task, &Calculation::sendProgress, this,
            &TcpCommunication::progressOnly);
    quaryTimer->start(2000);
}

// 结果数据包包含pdf报告ByteArray及数据两部分
//  msg.type 06
//  msg.subtype 报告ByteArray长度
//  msg.length  总长度
void TcpCommunication::sendResult() {
    QByteArray data;
    data.append(m_pdf);
    int data1size = data.size();
    data.append((char *)&m_result, sizeof(m_result));
    sendData(6, data, data1size);
}

// 客户端新连接
void TcpCommunication::acceptConnection() {
    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &TcpCommunication::readData);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::errorOccurred, this,
            &TcpCommunication::errorDisplay);
    connect(socket, &QTcpSocket::disconnected, this,
            &TcpCommunication::disconnect);
    qDebug() << "client connect";
}

// 客户端重连，socket信号槽断连，需重新建立

// TCP缓存读取
void TcpCommunication::readData() {
    while (socket->bytesAvailable()) {
        if (!recvMsg.dataLength) {
            if (socket->bytesAvailable() < (qint64)sizeof(Msg))
                return;
            else {
                buffer = socket->read(sizeof(Msg));
                Msg *tmp = (Msg *)buffer.data();
                recvMsg.type = tmp->type;
                recvMsg.subType = tmp->subType;
                recvMsg.dataLength = tmp->dataLength;
                if (recvMsg.dataLength)
                    buffer.clear();
                else
                    parseMsg(buffer);
            }
        } else {
            int readAva = socket->bytesAvailable();
            if (readAva < recvMsg.dataLength) {
                buffer.append(socket->read(readAva));
                recvMsg.dataLength -= readAva;
            } else {
                buffer.append(socket->read(recvMsg.dataLength));
                parseMsg(buffer);
                recvMsg.dataLength = 0;
                buffer.clear();
            }
        }
    }
}
/*  很容易出现buffer.size()>recvMsg.dataLength导致不解析

qDebug() << buffer.size() << recvMsg.dataLength;
if (buffer.size() == recvMsg.dataLength) {
  parseMsg(buffer);
  buffer.clear();
  recvMsg.dataLength = 0;
} else
  return;*/

// 数据解析函数

void TcpCommunication::parseMsg(QByteArray &buffer) {

    switch (recvMsg.type) {
    case 1: // 请求当前任务情况
    {
        QByteArray tmp;
        if (taskStage != 1)
            tmp.append((char *)&information, sizeof(information));
        sendData(1, tmp, taskStage);
    } break;
    case 2: // 开始测量
        taskStage = 2;
        // 设置任务信息
        std::memcpy(&information, buffer.data(), buffer.length());
        if (!information.manualControl)
            autoStop = true;
        /*qDebug() << information.manualControl << information.frameRate
                 << information.taskImgNum << information.bulkDensity
                 << information.apparentDensity
                 << QString::fromStdString(information.taskId)
                 << QString::fromStdString(information.sampleType)
                 << QString::fromStdString(information.sampleSource)
                 << QString::fromStdString(information.testingUnit)
                 << QString::fromStdString(information.inspector);*/
        buffer.clear();
        // 线程中开始测量任务
        taskThread = new QThread;
        task = new Calculation(&information);
        task->moveToThread(taskThread);
        connect(taskThread, &QThread::finished, task, &Calculation::deleteLater);
        connect(task, &Calculation::destroyed, taskThread, &QThread::deleteLater);

        connect(task, &Calculation::progressShow, this, &TcpCommunication::saveShowImg);
        connect(this, &TcpCommunication::taskStart, task, &Calculation::taskStart);
        connect(this, &TcpCommunication::taskStop, task, &Calculation::taskStop);
        taskThread->start();
        std::cout << "-----------------------------------------------------" << std::endl
                  << "Task: " << information.taskId << " start" << std::endl;
        emit taskStart();
        break;
    case 3:
        reportProgress();
        break;
    case 4: // 停止采集
    {
        stopGet();
        QByteArray tmp;
        sendData(2, tmp, prog[1]);
        information.taskImgNum = prog[1]; // 总张数设为现照片数
    } break;
    case 5:                               // 发送1s的颗粒数据
        SendDataPerSec(recvMsg.subType);
        break;
    case 6: // 发送结果及pdf(服务器统计完成主动发送)
        sendResult();
        break;
    case 7: {
        taskEnd();
        initialTask();
        QByteArray tmp;
        sendData(7, tmp);
    } break;
    default:
        break;
    }
}

// 接受消息类型
// 0 1 2 3 4 5 6 7 8 9
// 1：请求当前任务阶段
// 2：开始任务
// 3: 请求进度数据
// 4: 停止采集
// 5：请求颗粒数据
// 6: 请求任务结果及报告
// 7:结束任务
// 根据消息类型发送不同报文
// 0 1 2 3 4 5 6 7 8 9
// 1:告知当前任务阶段
// 2:告知停止成功并返回总张数
// 3:发送图片及进度
// 4:发送进度
//  subtype:0 发送进度
//  subtype:1 告知统计完成
// 5：发送颗粒数据
// 6:发送报告
// 7:任务空间回收成功
void TcpCommunication::sendData(int type, QByteArray &message, int subType) {
    Msg header;
    header.type = type;
    header.subType = subType;
    header.dataLength = message.size();
    QByteArray headArray;
    headArray.resize(sizeof(Msg));
    std::memcpy(headArray.data(), &header, sizeof(Msg));
    if (socket->isWritable()) {
        socket->write(headArray);
        if (header.dataLength)
            socket->write(message);
        socket->flush();
    } else {
        socket->disconnectFromHost();
        std::cout << "disconnect to client,because:" << socket->errorString().toStdString() << std::endl;
        return;
    }
}

void TcpCommunication::disconnect() { std::cout << "disconnnected" << std::endl; }

void TcpCommunication::errorDisplay() { std::cout << socket->errorString().toStdString() << std::endl; }
