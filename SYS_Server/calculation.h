#ifndef CALCULATION_H
#define CALCULATION_H

#include "global.h"
#include "opencv2/opencv.hpp"
#include <QObject>
#include <QThreadPool>

class GetImages;
class QThread;
struct taskInfo;
class QThreadPool;

class Calculation : public QObject {
    Q_OBJECT
public:
    explicit Calculation(TaskInfo *curtask, QObject *parent = nullptr);
    ~Calculation();
    void taskStart();
    void taskStop();
    void getProgress();
    void taskExec(const ImagesPerSec &Sec);          // 图像处理开始响应槽
    std::vector<std::vector<SandInfo>> *getResult(); // 返回结果集合指针

signals:
    void progressShow(const ImagesPerSec &Sec, int processedImgNum,
                      int totalImgNum); //
    void sendProgress(const QByteArray &);
    void startGetImg();
    void stopGetImg();

private:
    GetImages *imgSender;
    std::vector<cv::Mat> matVec;                    // 每帧图片集合
    std::vector<QImage>
        showImgbuf;                                 // 显示图片缓存                    // 待处理图片
    std::list<std::vector<cv::Mat> *> pendingMat;   // 待处理缓存区
    std::vector<std::vector<SandInfo>> calcResults; // 结果集合
    int processedImgNum = 0;
    int totalImgNum = 0;
    int frameRate;
    int taskImgNum;
    int progress[2];
    QThread *getImgThread;
    QThreadPool *m_threadPool;
};

#endif // CALCULATION_H
