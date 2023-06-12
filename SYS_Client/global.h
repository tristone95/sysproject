#ifndef GLOBAL_H
#define GLOBAL_H
#include <QImage>

struct TaskInfo {
    bool manualControl = true;  // 是否手动停止
    int frameRate = 50;
    int taskImgNum = 99999999;  // 指定照片数量
    double bulkDensity = 0;     // 体密度
    double apparentDensity = 0; // 视密度
    char taskId[32];            // 任务ID yyyymmddhhmmss 28字节
    char sampleType[24];        // 样品类型
    char sampleSource[80];      // 样品来源
    char testingUnit[80];       // 检测单位
    char inspector[16];         // 检测人员
};

// 颗粒信息参数
struct SandInfo {
    // 几何描述符
    double area;          // 暂存颗粒投影面积
    double perimeter;     // 暂存颗粒投影周长
    double minRectWidth;  // 最小外接矩形的宽
    double minRectLength; // 最小外接矩形的长
    double aEDiameter;    // 面积等效圆直径
    double pEDiameter;    // 周长等效圆直径
    double maxIDiameter;  // 最大内切圆直径
    double minCDiameter;  // 最小外接圆直径
    double maxFeret;      // 费雷特大径
    double minFeret;      // 费雷特小径
    // 比例描述符
    double rectangularity; // 矩形度
    double aspectRatio;    // 长宽比
    double compactness;    // 紧凑度
    double pIrregularity;  // 不规则度
    // 介观描述符
    double roundness;   // 颗粒圆度
    double sphericity;  // 颗粒球度
    double granularity; // 颗粒粒度,minRectWidth
    QImage *image;
};

struct Msg {
    int type;
    int subType;
    int dataLength;
};
struct ResultInfo {
    double granularityPass;    // 粒径分布合格率
    double averageGranularity; // 平均粒径
    double sphericity;         // 球度平均值
    double roundness;          // 圆度平均值
    double bulkDensity;        // 体密度(g/cm3)
    double apparentDensity;    // 视密度(g/cm3)
    int particleNumTotal;      // 颗粒总数
    int totalSec;              // 任务秒数
};
#endif                         // GLOBAL_H
