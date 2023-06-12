#ifndef GENERATEREPO_H
#define GENERATEREPO_H
#include <QString>
#include <global.h>

// PDF数据集 聚合类
struct Info {
    QString sampleType;     // 样品类型
    QString sampleSource;   // 样品来源
    QString taskID;         // 样品编号
    QString testingUnit;    // 检测单位
    QString inspector;      // 检测人员
    QString testDate;       // 检测日期

    double granularityPass; // 粒径分布合格率
    double sphericity;      // 球度平均值
    double roundness;       // 圆度平均值
    double bulkDensity;     // 体密度(g/cm3)
    double apparentDensity; // 视密度(g/cm3)
    int particleNumTotal;   // 颗粒总数

    QString d10;            // 粒径分布10点位值
    QString d50;
    QString d84;
    QString d90;
    QString d97;
    QString maxGranularity;              // 最大粒径
    QString minGranularity;              // 最小粒径
    QString averageGranularity;          // 平均粒径
    QString medianGranularity;           // 中位粒径

    std::vector<double> granularityRate; // 粒径区间分布
    std::vector<double> granularityAccu; // 粒径累计分布

    std::vector<double> sphericityRate;  // 球度区间分布
    std::vector<double> sphericityAccu;  // 球度累计分布

    std::vector<double> roundnessRate;   // 圆度区间分布
    std::vector<double> roundnessAccu;   // 圆度累计分布
};

class GenerateRepo {
public:
    explicit GenerateRepo();
    ~GenerateRepo();
    Info *countMain(TaskInfo &info, const std::vector<std::vector<SandInfo>> &sandsResult, QByteArray *pdf); // 统计主函数
    void getArray(const std::vector<std::vector<SandInfo>> &sandsResult);                                    // 遍历结果verctor生成圆度粒度球度vector
    void countGranularity();
    void countSphericity();
    void countRoundness();
    void drawPdf(QByteArray *pdf); // pdf绘制
    // 表格绘制
    void drawForm(QPainter *painter, int y, int rows, int column, int cellHeight, QFont &font, QStringList &list, bool flag = true);
    // 统计图
    void drawChart(QPainter *painter, int pointx, int pointy, int width, int height,
                   std::vector<double> &vecDisRate, std::vector<double> &vecDisAccu,
                   std::vector<double> &vecDisInterval, bool flag);
    // 指标合格判定
    QString getCheckResul(double data, int checkCase);
    // 获取pdf bytearray

private:
    Info *generateReslt;
    int y; // 页面指针 记录页面已绘制长度，用于换页判断
    // 颗粒粒度、球度、圆度vector
    std::vector<double> particleGranularity;
    std::vector<double> particleSphericity;
    std::vector<double> particleRoundness;
    std::vector<double> gDInterval = {150, 212, 250, 300, 355, 425, 600};               // 40-70目粒度分布区间
    std::vector<double> dInterval = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}; // 40-70目球度、圆度分布区间
};

#endif // GENERATEREPO_H
