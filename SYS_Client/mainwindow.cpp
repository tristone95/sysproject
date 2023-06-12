#include "mainwindow.h"
#include "loadingdialog.h"
#include "pdfviewerdialog.h"
#include "processview.h"
#include "resultviewer.h"
#include "taskinfoform.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include <QComboBox>
#include <QPushButton>
#include <QTcpSocket>
// #include <QThreadPool>
#include <QTime>
#include <QTimer>
// #include <QtConcurrent>

Msg recvMsg = {0, 0, 0}; // 待解析消息头

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    client = new QTcpSocket(this);
    login = new Widget;
    login->show();
    connect(login, &Widget::login, this, &MainWindow::clientinit);
    connect(this, &MainWindow::loginfail, login, &Widget::relogin);

    connect(client, &QTcpSocket::readyRead, this, &MainWindow::readData);
    connect(client, &QTcpSocket::disconnected, this, &MainWindow::disconnect);
    connect(client, &QTcpSocket::errorOccurred, this, &MainWindow::errorDisplay);
}

MainWindow::~MainWindow() {
    delete login;
    delete client;
    delete ui;
}

void MainWindow::clientinit(QString ip, int port) {
    m_IP = ip;
    m_Port = port;
    client->connectToHost(m_IP, m_Port);
    // 等待,连接成功则connectsucced,否则连接失败
    client->waitForConnected();
    QByteArray tmp;
    sendData(1, tmp);
    QEventLoop loop;
    QTimer::singleShot(15000, &loop, &QEventLoop::quit);
    loop.exec();
    emit loginfail();
}

void MainWindow::windowReload() {
    ui->stackedWidget->setCurrentIndex(0);

    loading = new LoadingDialog;
    loading->setTipsText(tr("检测初始中，请稍等..."));
    loading->moveToCenter(this);
    loading->show();
    information = {true, 50, 99999999, 0, 0, "", "", "", "", ""};
    showImgBuffer.clear();
    pdfArray.clear();
    if (queryTimer) {
        delete queryTimer;
        queryTimer = nullptr;
    }
    if (taskinfo) {
        ui->stackedWidget->removeWidget(taskinfo);
        delete taskinfo;
        taskinfo = nullptr;
    }
    if (processView) {
        ui->stackedWidget->removeWidget(processView);
        delete processView;
        processView = nullptr;
    }
    if (resultViewer) {
        ui->stackedWidget->removeWidget(resultViewer);
        delete resultViewer;
        resultViewer = nullptr;
    }
    showWindow(1);
    loading->close();
    loading = nullptr;
}

// 开始任务响应槽
void MainWindow::startTask(TaskInfo info) {
    std::memcpy(&information, &info, sizeof(TaskInfo));
    QByteArray qinfo;
    qinfo.append((char *)&information, sizeof(TaskInfo));
    sendData(2, qinfo);
    processView->initialWin(information.manualControl, information.taskImgNum);
    ui->stackedWidget->setCurrentWidget(processView);
    queryStart();
    loading = new LoadingDialog;
    loading->setTipsText(tr("任务启动中，请稍等..."));
    loading->moveToCenter(this);
    loading->show();
}

// 停止采集拍摄
void MainWindow::showStop() {
    QByteArray tmp;
    sendData(4, tmp);
}

// 进度查询
void MainWindow::queryStart() {
    queryTimer = new QTimer;
    queryTimer->setTimerType(Qt::PreciseTimer);
    connect(queryTimer, &QTimer::timeout, this, [&] {
    QByteArray tmp;
    sendData(3, tmp); });
    queryTimer->start(1000);
}
// 计算完成响应
void MainWindow::queryStop() {
    queryTimer->stop();
}

// 界面初始化
void MainWindow::showWindow(int stage) {
    switch (stage) {
    case 1: {
        taskinfo = new TaskInfoForm;
        ui->stackedWidget->addWidget(taskinfo);
        ui->stackedWidget->setCurrentWidget(taskinfo);
        connect(taskinfo, &TaskInfoForm::taskstart, this, &MainWindow::startTask);
    }
    case 2:
    case 3: {
        processView =
            new ProcessView(information.manualControl, information.taskImgNum);
        ui->stackedWidget->addWidget(processView);
        connect(processView, &ProcessView::stop, this, &MainWindow::showStop);
    }
    case 4: {
        resultViewer = new ResultViewer(this);
        ui->stackedWidget->addWidget(resultViewer);
    }
    }
    switch (stage) {
    case 1:
        show();
        break;
    case 2:
        /* 重连等待数据
      loading = new LoadingDialog;
      loading->setTipsText(tr("任务启动中，请稍等..."));
      loading->moveToCenter(this);
      loading->show();
        */
        // sendData();
    case 3:
        ui->stackedWidget->setCurrentWidget(processView);
        show();
        queryStart();
        break;
    case 4: {
        ui->stackedWidget->setCurrentWidget(resultViewer);
        QByteArray tmp;
        sendData(5, tmp, 1);
        sendData(6, tmp);
    } break;
    default:
        show();
        break;
    }
    if (login && !isHidden()) {
        login->close();
        login = nullptr;
    }
}
/*
void MainWindow::readDataThread()
{
    QFuture<void> result =
        QtConcurrent::run(m_thPool, &MainWindow::readData, this);
}
*/
void MainWindow::readData() {
    /*buffer = client->readAll();
    int readsize = 0;
    while (readsize != readsize) {
    }*/
    while (client->bytesAvailable()) {
        if (!recvMsg.dataLength) {
            if (client->bytesAvailable() < (qint64)sizeof(Msg))
                return;
            else {
                buffer = client->read(sizeof(Msg));
                Msg *tmp = (Msg *)buffer.data();
                recvMsg.type = tmp->type;
                recvMsg.subType = tmp->subType;
                recvMsg.dataLength = tmp->dataLength;
                // qDebug() << "receive" << recvMsg.type;
                if (recvMsg.dataLength)
                    buffer.clear();
                else
                    parseMsg(&buffer);
                // emit readFinish(&buffer);
            }
        } else {
            int readAva = client->bytesAvailable();
            if (readAva < recvMsg.dataLength) {
                buffer.append(client->read(readAva));
                recvMsg.dataLength -= readAva;
            } else {
                buffer.append(client->read(recvMsg.dataLength));
                parseMsg(&buffer);
                // emit readFinish(&buffer);
                recvMsg.dataLength = 0;
                buffer.clear();
            }
        }
    }
}
/*
buffer.append(client->readAll());
if (buffer.size() == recvMsg.dataLength) {
parseMsg(buffer);
buffer.clear();
recvMsg.dataLength = 0;
} else
return;*
}
}*/
// 发送消息类型
// 0 1 2 3 4 5 6 7 8 9
// 1：请求当前任务阶段
// 2：开始任务
// 3: 请求进度数据
// 4: 停止采集
// 5：请求颗粒数据     仅头
// 6: 请求报告及pdf        仅头
// 7:结束任务
// 接收消息类型
// 0 1 2 3 4 5 6 7 8 9

// 1:告知当前任务阶段 仅头
// 2:告知停止成功并返回总张数=subtype 仅头
// 3:发送图片及进度    仅头
// 4:发送进度 仅头
// 5：发送颗粒数据
// 6:发送报告
void MainWindow::parseMsg(QByteArray *buffer) {
    switch (recvMsg.type) {
    case 1: {
        if (recvMsg.subType != 1) {
            std::memcpy(&information, buffer->data(), buffer->size());
        }
        showWindow(recvMsg.subType);
    } break;
        // 停止成功
    case 2:
        information.taskImgNum = recvMsg.subType;
        // processView->stopImg(recvMsg.subType, true);
        break;
    case 3: {
        // 收到图片包再开始图片展示
        /*
      if (!showFlag)
      {
          processView->startImg();
          showFlag = true;
      }*/
        QDataStream in(buffer, QDataStream::ReadOnly);
        in.setVersion(QDataStream::Qt_6_2);
        int processNum, totalNum;
        in >> processNum >> totalNum;

        std::vector<QImage> images;
        /*
         for (int var = 0; var < information.frameRate; ++var)
         {
             QImage img;
             in >> img;
             images.emplace_back(img);
         }
         */
        processView->upData(processNum, totalNum, images);
        // 首次收到进度数据，关闭等待对话框
        if (loading) {
            loading->close();
            loading = nullptr;
        }
    } break;
    case 4:
        if (recvMsg.subType == 0) { // 处理进度更新
            int num[2];
            std::memcpy(&num, buffer->data(), sizeof(int) * 2);
            processView->upData(num[0], num[1]);
        } else if (recvMsg.subType == 1) { // 计算完成
            queryStop();
            // 请求结果
            QByteArray tmp;
            sendData(5, tmp, 1); // 请求第1s颗粒
            sendData(6, tmp);    // 请求结果数据及pdf报告
            loading = new LoadingDialog;
            loading->setTipsText(tr("处理结束，统计结果中..."));
            loading->moveToCenter(this);
            loading->show();
            // ui->stackedWidget->setCurrentWidget(resultViewer);
        }
        break;
    case 5: {
        // 请求颗粒计算数据
        int count = recvMsg.subType / sizeof(SandInfo);
        std::vector<SandInfo> vInfo(count);
        std::memcpy(vInfo.data(), buffer->data(), recvMsg.subType);
        QByteArray imgs;
        QDataStream in(&imgs, QDataStream::ReadOnly);
        in.setVersion(QDataStream::Qt_6_2);
        imgs.append(buffer->last(buffer->size() - recvMsg.subType));
        for (int var = 0; var < count; ++var) {
            QImage img;
            in >> img;
            vInfo[var].image = new QImage(std::move(img));
        }
        resultViewer->setDatas(vInfo);
        // 滚动条复位
        resultViewer->scrollTop();
        buffer->clear();
    } break;
    case 6: {
        pdfArray.append(buffer->data(), recvMsg.subType);
        ResultInfo rInfo;
        std::memcpy(&rInfo, buffer->last(buffer->size() - recvMsg.subType), buffer->size() - recvMsg.subType);
        resultViewer->updateResult(rInfo);
        resultViewer->setComboItems(rInfo.totalSec);
        // 重连情况下获取数据显示
        if (isHidden()) {
            resultViewer->initWindow(information);
            show();
            login->close();
            login = nullptr;
        }
        if (loading)
        // 收到颗粒消息，进入显示界面
        {
            loading->close();
            loading = nullptr;
            resultViewer->initWindow(information);
            ui->stackedWidget->setCurrentWidget(resultViewer);
        }
        // 关联combo与槽
        connect(resultViewer->getCombox(), &QComboBox::currentIndexChanged, this, [=](int index) {
            QByteArray tmp;
            sendData(5, tmp, index + 1);
        });
        // 关联查看pdf信号
        connect(resultViewer, &ResultViewer::showPdf, this, [this] {
            PdfViewerDialog pdfDia(pdfArray, QString::fromStdString(information.taskId), this);
            pdfDia.convertToImg();
            pdfDia.setImgs();
            pdfDia.exec();
        });
        //  关联重新检测btton与槽
        connect(resultViewer->getButton(), &QPushButton::clicked, this, [this] {
            QByteArray tmp;
            sendData(7, tmp);
        });
    } break;
    case 7:
        windowReload();
        break;
    default:
        break;
    }
}

void MainWindow::sendData(int type, QByteArray &message, int subType) {
    Msg header;
    header.type = type;
    header.subType = subType;
    header.dataLength = message.size();
    QByteArray headArray;
    headArray.resize(sizeof(Msg));
    std::memcpy(headArray.data(), &header, sizeof(Msg));
    // qDebug() << "Send  " << type;
    if (client->isWritable()) {
        client->write(headArray);
        if (header.dataLength)
            client->write(message);
        client->flush();
    }
}
void MainWindow::disconnect() {}

void MainWindow::errorDisplay(QAbstractSocket::SocketError) {
    // qDebug() << client->errorString();
}
