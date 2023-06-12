#include "calculation.h"
#include "cvprocess.h"
#include "getimages.h"
#include <QDebug>
#include <QMutex>
#include <QThread>

QMutex matVecLock, taskResLock;

Calculation::Calculation(TaskInfo *curtask, QObject *parent)
    : QObject{parent} {

    frameRate = curtask->frameRate;
    taskImgNum = curtask->taskImgNum;
    processedImgNum = 0;
    totalImgNum = 0;
}

Calculation::~Calculation() {
    // 线程退出
    getImgThread->quit();
    getImgThread->wait();
    m_threadPool->waitForDone();
    // clock_t start, end;
    // start = clock();
    for (auto &sec : calcResults) {
        for (auto &sand : sec) {
            delete sand.image;
        }
    }
    // end = clock();
    // std::cout << (end - start) * 1.0 / CLOCKS_PER_SEC << std::endl;
    delete m_threadPool;
}

void Calculation::taskStart() {
    getImgThread = new QThread;
    imgSender = new GetImages(frameRate); // frameRate
    // 创建图片获取线程
    imgSender->moveToThread(getImgThread);
    // 线程结束，销毁对象
    connect(getImgThread, &QThread::finished, imgSender, &GetImages::deleteLater);
    // 对象释放，销毁线程
    connect(imgSender, &GetImages::destroyed, getImgThread, &QThread::deleteLater); // 释放线程

    connect(imgSender, &GetImages::sendImages, this, &Calculation::taskExec);
    connect(this, &Calculation::startGetImg, imgSender, &GetImages::start);
    connect(this, &Calculation::stopGetImg, imgSender, &GetImages::stop);
    getImgThread->start();

    // 创建线程池
    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(3);
    m_threadPool->setExpiryTimeout(-1); // 设置线程等待不超时
    // 开始采集图片
    emit startGetImg();
}
void Calculation::taskStop() {
    emit stopGetImg();
}
void Calculation::getProgress() {
    QByteArray data;
    data.append((char *)&processedImgNum, 4);
    data.append((char *)&totalImgNum, 4);
    emit sendProgress(data);
}

void Calculation::taskExec(const ImagesPerSec &Sec) {
    totalImgNum += frameRate;
    // Qimage to cv::Mat,用于图像处理,在处理后释放空间
    std::vector<cv::Mat> *mv = new std::vector<cv::Mat>;
    mv->reserve(frameRate);
    for (const QImage &img : *(Sec.data)) {
        // cvMat构造共享img内存,故用clone复制
        (*mv).push_back(cv::Mat(img.height(), img.width(), CV_8UC1,
                                (void *)img.constBits(), img.bytesPerLine())
                            .clone());
    }
    emit progressShow(Sec, processedImgNum, totalImgNum);
    pendingMat.push_back(mv);
    CvProcess *runnable =
        new CvProcess(&pendingMat, &calcResults, &processedImgNum, frameRate);
    runnable->setAutoDelete(true); // 线程结束自动删除runnable
    m_threadPool->start(runnable); // 入线程池
}

std::vector<std::vector<SandInfo>> *Calculation::getResult() {
    return &calcResults;
}
