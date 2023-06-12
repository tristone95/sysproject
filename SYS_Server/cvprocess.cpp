/*
*此部分为图像处理核心代码，处于对原项目保密考虑，此处部分代码省略
*/
#define HAVE_OPENCV_IMGPROC
#include "cvprocess.h"
#include <QMutex>
#include <QTime>
#include <math.h>

const double physicalSize = 6.90;      // 像素物理尺寸
extern QMutex matVecLock, taskResLock; // 线程池信号量，读取图片和写入结果锁
CvProcess::CvProcess(std::list<std::vector<cv::Mat> *> *matVec,
                     std::vector<std::vector<SandInfo>> *outputVec,
                     int *processedNum, int frameRate) {
    m_frameRate = frameRate;
    m_matVec = matVec;
    m_outputVec = outputVec;
    processedImgNum = processedNum;
}

CvProcess::~CvProcess() {
}

void CvProcess::run() {
    // 同步 获取一帧图片
    try {

        matVecLock.lock();
        // 1秒图片Mat的vector 堆中
        std::vector<cv::Mat> *pendingMats =
            new std::vector<cv::Mat>(std::move(*(m_matVec->front())));
        // 销毁队首的堆中vector的空间
        delete m_matVec->front();
        m_matVec->pop_front();
        matVecLock.unlock();

        // 拷贝原图用于颗粒图像提取
        std::vector<cv::Mat> rawPics = *pendingMats;
        std::vector<SandInfo> vSandInfo; // 存储1帧中颗粒集合
        vSandInfo.reserve(200);          // 预分配空间
        // 以大图(mat)为单位处理
        int index = 0;
        for (cv::Mat &mat : *pendingMats) {
            std::vector<std::vector<cv::Point>> vContours; // 轮廓点 集合

            /************* 提取轮廓前图片处理**************/
            processImg(mat, vContours);


            // 根据轮廓筛除杂质

            // 剔除无颗粒图片(即当次循环)计算
            if (vContours.size() == 0)
                continue;

            //***** 对单颗粒轮廓(vContours[i])进行处理

            // 大图轮廓矩阵
            cv::Mat tmpContoursImg = cv::Mat::zeros(mat.rows, mat.cols, CV_8UC1);
            cv::drawContours(tmpContoursImg, vContours, -1, cv::Scalar(255, 255, 255),
                             1);

            // 照片中单颗粒处理
            for (unsigned int i = 0; i < vContours.size(); ++i) {
                SandInfo sandInfo;
                cv::Rect rawRect = cv::boundingRect(vContours[i]); // 获取轮廓矩形
                /*************裁剪单颗粒并保存转为Qimage用于客户端展示**************/
                cv::Mat rawSandImgMat;
                rawPics[index](rawRect).copyTo(rawSandImgMat);

                // 保存前有效性检测
                /*颗粒有效性检测  去除过大颗粒 */

                if (rawRect.width > 600 / 6.9 || rawRect.width < 100 / 6.9) {
                    continue; // 跳过该颗粒
                }
                /*颗粒有效性检测  去除失焦颗粒 */
                // 直方图过滤失焦及过曝颗粒
                // 存储单颗粒图片
                /************* 颗粒特征值计算**************/
                calcParticle(&sandInfo, vContours[i], rawRect, tmpContoursImg);
                vSandInfo.emplace_back(std::move(sandInfo));
            }
            ++index;
        }
        taskResLock.lock();
        m_outputVec->emplace_back(std::move(vSandInfo));
        *processedImgNum += m_frameRate;
        taskResLock.unlock();
        // 释放相机图片空间
        delete pendingMats;
        // end = clock();
        //  std::cout << (end - start) * 1.0 / CLOCKS_PER_SEC << std::endl;
    } catch (cv::Exception &exc) {
        std::cout << "cv::Mat:" << exc.msg << std::endl;
    } catch (...) {
        std::cout << "run error" << std::endl;
    }
}


//颗粒轮廓图旋转函数，用于计算费雷特直径
void CvProcess::rotateArbitrarilyAngle(cv::Mat &src, cv::Mat &dst,
                                       float angle) {
    try {

    } catch (cv::Exception &exc) {
        std::cout << "cv::Mat:" << exc.msg << std::endl;
    } catch (...) {
        std::cout << "rotate error" << std::endl;
    }
}

//费雷特直径计算函数获取费雷特大径与小径
void CvProcess::getFeretDiameter(cv::Mat &contourMat, double &min,
                                 double &max) {
    try {
        // 由轮廓向量构建轮廓矩阵

    } catch (cv::Exception &exc) {
        std::cout << "cv::Mat:" << exc.msg << std::endl;
    } catch (...) {
        std::cout << "getFeret error" << std::endl;
    }
}

void CvProcess::calcParticle(SandInfo *sandInfo,
                             std::vector<cv::Point> &vContour,
                             cv::Rect &rawRect, cv::Mat &tpContoursImg) {
    try {
        /*特征值计算之几何描述符*/
        // 面积周长

        // 最小外接矩形
        // 等效面积圆直径、等效周长圆直径
        // 外接圆直径   
        // 内切圆直径
        // 费雷特大径、小径
        getFeretDiameter(contourMat, sandInfo->minFeret, sandInfo->maxFeret);
        /*特征值计算之比例描述符*/

        /*特征值计算之介观描述符*/

    } catch (cv::Exception &exc) {
        std::cout << "cv::Mat:" << exc.msg << std::endl;
    } catch (...) {
        std::cout << "calc error" << std::endl;
    }
}

// 进行图片处理以获取颗粒轮廓
void CvProcess::processImg(cv::Mat &mat,
                           std::vector<std::vector<cv::Point>> &vContours) {
    try {
        //对mat进行初始处理部分代码，包括去除边缘、噪声消除及轮廓提取等处理
    } catch (cv::Exception &exc) {
        std::cout << "cv::Mat:" << exc.msg << std::endl;
    } catch (...) {
        std::cout << "processImg error" << std::endl;
    }
}
