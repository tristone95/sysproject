#ifndef CVPROCESS_H
#define CVPROCESS_H

#include "global.h"
#include "opencv2/opencv.hpp"
#include <QRunnable>

class CvProcess : public QRunnable {
public:
    explicit CvProcess(std::list<std::vector<cv::Mat> *> *matVec,
                       std::vector<std::vector<SandInfo>> *outputVec,
                       int *processedNum, int frameRate);
    ~CvProcess();
    void run() override;

private:
    int *processedImgNum;
    int m_frameRate;
    std::list<std::vector<cv::Mat> *> *m_matVec;
    std::vector<std::vector<SandInfo>> *m_outputVec;
    void rotateArbitrarilyAngle(cv::Mat &src, cv::Mat &dst, float angle);
    void getFeretDiameter(cv::Mat &contoursMat, double &min, double &max);

    void processImg(cv::Mat &mat, std::vector<std::vector<cv::Point>> &vContours);
    void calcParticle(SandInfo *sandInfo, std::vector<cv::Point> &vContour,
                      cv::Rect &rawRect, cv::Mat &tmpContoursImg);
};

#endif // CVPROCESS_H
