/*
 *通过
 *模拟从相机内存获取照片
 *
 */
#include "getimages.h"
#include <QDebug>
#include <QTimer>
GetImages::GetImages(int frameRate, QObject *parent)
    : QObject{parent} {
    m_frameRate = frameRate;
}

GetImages::~GetImages() {
    delete cambuffer;
    delete m_Timer;
}

void GetImages::start() {
    cambuffer = new std::vector<QImage>(m_frameRate);
    // 模拟相机存储内存
    for (int var = 0; var < m_frameRate; ++var) {
        QString path = "./imgbuffer/image" + QString::number(var) + ".jpg";
        (*cambuffer)[var] = QImage(path);
    }
    m_Timer = new QTimer;
    m_Timer->setTimerType(Qt::PreciseTimer);
    connect(m_Timer, &QTimer::timeout, this, &GetImages::getFromBuf);
    m_Timer->start(2000);
}

void GetImages::stop() {
    m_Timer->stop();
    qDebug() << "Stop Get Images.";
}

void GetImages::getFromBuf() {
    // qDebug() << "1sec";
    ImagesPerSec Sec;
    Sec.index = index++;
    // Sec.data在TcpCommunication::saveShowImg中销毁
    Sec.data = new std::vector<QImage>(m_frameRate);
    *(Sec.data) = *cambuffer; // 复制
    emit sendImages(Sec);     // 返回给预览图缓存区，跨线程拷贝
}
