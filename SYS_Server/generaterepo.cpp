#include "generaterepo.h"
#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPdfWriter>

GenerateRepo::GenerateRepo() {
    generateReslt = new Info;
}

GenerateRepo::~GenerateRepo() {
    delete generateReslt;
}

// 传入任务信息和任务结果，
// 进行统计分析、生成PDF、并返回结构体分析结果
Info *GenerateRepo::countMain(TaskInfo &info, const std::vector<std::vector<SandInfo>> &sandsResult, QByteArray *pdf) {
    generateReslt->sampleType = QString::fromStdString(info.sampleType);
    generateReslt->sampleSource = QString::fromStdString(info.sampleSource);
    generateReslt->taskID = QString::fromStdString(info.taskId);
    generateReslt->testingUnit = QString::fromStdString(info.testingUnit);
    generateReslt->inspector = QString::fromStdString(info.inspector);
    generateReslt->testDate = QString("%1-%2-%3")
                                  .arg(generateReslt->taskID.first(4), generateReslt->taskID.mid(4, 2), generateReslt->taskID.mid(6, 2));
    generateReslt->bulkDensity = info.bulkDensity;
    generateReslt->apparentDensity = info.apparentDensity;

    getArray(sandsResult); // 生成粒度、圆度、球度vector
    countGranularity();    // 粒度统计
    countSphericity();     // 球度统计
    countRoundness();      // 圆度统计
    drawPdf(pdf);

    return generateReslt;
}
// 结果遍历,分别得到粒度、圆度、球度vector
void GenerateRepo::getArray(const std::vector<std::vector<SandInfo>> &sandsResult) {
    for (unsigned int secCount = 0; secCount < sandsResult.size(); ++secCount) {
        for (unsigned int sandCount = 0; sandCount < sandsResult[secCount].size(); ++sandCount) {
            particleGranularity.push_back(sandsResult[secCount][sandCount].granularity);
            particleRoundness.push_back(sandsResult[secCount][sandCount].roundness);
            particleSphericity.push_back(sandsResult[secCount][sandCount].sphericity);
        }
    }
    generateReslt->particleNumTotal = particleGranularity.size();
}

// 粒度统计
void GenerateRepo::countGranularity() {

    std::vector<double> distribRate;                                   // 各区间分布占比
    int totalNum = particleGranularity.size();                         // 颗粒总数目
    std::sort(particleGranularity.begin(), particleGranularity.end()); // 对粒度vector排序,默认升序
    int distribNum[8] = {0};                                           // 各区间颗粒数，局部数组全初始化
    double sum = 0;                                                    // 粒度总和
    for (int var = 0; var < totalNum; ++var) {
        sum += particleGranularity[var];
        if (particleGranularity[var] < gDInterval[0]) {
            ++distribNum[0];
        } else if (particleGranularity[var] < gDInterval[1] && particleGranularity[var] >= gDInterval[0]) {
            ++distribNum[1];
        } else if (particleGranularity[var] < gDInterval[2] && particleGranularity[var] >= gDInterval[1]) {
            ++distribNum[2];
        } else if (particleGranularity[var] < gDInterval[3] && particleGranularity[var] >= gDInterval[2]) {
            ++distribNum[3];
        } else if (particleGranularity[var] < gDInterval[4] && particleGranularity[var] >= gDInterval[3]) {
            ++distribNum[4];
        } else if (particleGranularity[var] <= gDInterval[5] && particleGranularity[var] >= gDInterval[4]) {
            ++distribNum[5];
        } else if (particleGranularity[var] <= gDInterval[6] && particleGranularity[var] > gDInterval[5]) {
            ++distribNum[6];
        } else if (particleGranularity[var] > gDInterval[6]) {
            ++distribNum[7];
        }
    }
    for (int i = 0; i < 8; ++i) {
        QString rate = QString::number(double(distribNum[i]) / totalNum * 100, 'f', 2);
        distribRate.push_back(rate.toDouble());
    }
    // 进行保留2位小数舍尾修正
    double rateSum = 0;
    for (double var : distribRate)
        rateSum += var;
    if (rateSum != 100)
        distribRate[5] += (100 - rateSum);

    // 70-140目合格区间 212-425um
    generateReslt->granularityPass = distribRate[2] + distribRate[3] + distribRate[4] + distribRate[5];
    // 特征点位值
    generateReslt->d10 = QString::number(particleGranularity[int(totalNum * 0.1)], 'f', 2);
    generateReslt->d50 = QString::number(particleGranularity[int(totalNum * 0.5)], 'f', 2);
    generateReslt->d84 = QString::number(particleGranularity[int(totalNum * 0.84)], 'f', 2);
    generateReslt->d90 = QString::number(particleGranularity[int(totalNum * 0.9)], 'f', 2);
    generateReslt->d97 = QString::number(particleGranularity[int(totalNum * 0.97)], 'f', 2);
    generateReslt->maxGranularity = QString::number(particleGranularity[totalNum - 1], 'f', 2);
    generateReslt->minGranularity = QString::number(particleGranularity[0], 'f', 2);
    generateReslt->averageGranularity = QString::number(sum / totalNum, 'f', 2);
    generateReslt->medianGranularity = QString::number(particleGranularity[totalNum / 2], 'f', 2);
    // 保存分布到结果聚合类
    generateReslt->granularityRate = distribRate;
    double tmp = 0;
    for (unsigned var = 0; var < distribRate.size(); ++var) {
        tmp += distribRate[var];
        generateReslt->granularityAccu.push_back(tmp);
    }
}

// 球度统计
void GenerateRepo::countSphericity() {
    std::vector<double> distribRate;                                 // 各区间分布占比
    int totalNum = particleGranularity.size();                       // 颗粒总数目
    std::sort(particleSphericity.begin(), particleSphericity.end()); // 对粒度vector排序,默认升序
    int distribNum[10] = {0};                                        // 各区间颗粒数，局部数组全初始化
    double sum = 0;                                                  // 球度总和
    for (int var = 0; var < totalNum; ++var) {
        sum += particleSphericity[var];
        if (particleSphericity[var] < dInterval[0]) {
            ++distribNum[0];
        } else if (particleSphericity[var] <= dInterval[1] && particleSphericity[var] > dInterval[0]) {
            ++distribNum[1];
        } else if (particleSphericity[var] <= dInterval[2] && particleSphericity[var] > dInterval[1]) {
            ++distribNum[2];
        } else if (particleSphericity[var] <= dInterval[3] && particleSphericity[var] > dInterval[2]) {
            ++distribNum[3];
        } else if (particleSphericity[var] <= dInterval[4] && particleSphericity[var] > dInterval[3]) {
            ++distribNum[4];
        } else if (particleSphericity[var] <= dInterval[5] && particleSphericity[var] > dInterval[4]) {
            ++distribNum[5];
        } else if (particleSphericity[var] <= dInterval[6] && particleSphericity[var] > dInterval[5]) {
            ++distribNum[6];
        } else if (particleSphericity[var] <= dInterval[7] && particleSphericity[var] > dInterval[6]) {
            ++distribNum[7];
        } else if (particleSphericity[var] <= dInterval[8] && particleSphericity[var] > dInterval[7]) {
            ++distribNum[8];
        } else if (particleSphericity[var] <= dInterval[9] && particleSphericity[var] > dInterval[8]) {
            ++distribNum[9];
        }
    }
    for (int i = 0; i < 10; ++i) {
        QString rate = QString::number(double(distribNum[i]) / totalNum * 100, 'f', 2);
        distribRate.push_back(rate.toDouble());
    }
    // 进行保留2位小数舍尾修正,将舍去加到最高分布区间0.8-0.9
    double rateSum = 0;
    for (double var : distribRate)
        rateSum += var;
    if (rateSum != 100)
        distribRate[8] += (100 - rateSum);
    // 统计结果
    generateReslt->sphericity = sum / totalNum; // 平均粒径
    // 区间分布
    generateReslt->sphericityRate = distribRate;
    // 累积分布
    double tmp = 0;
    for (unsigned var = 0; var < distribRate.size(); ++var) {
        tmp += distribRate[var];
        generateReslt->sphericityAccu.push_back(tmp);
    }
}
// 圆度统计
void GenerateRepo::countRoundness() {
    std::vector<double> distribRate;                               // 各区间分布占比
    int totalNum = particleGranularity.size();                     // 颗粒总数目
    std::sort(particleRoundness.begin(), particleRoundness.end()); // 对粒度vector排序,默认升序
    int distribNum[10] = {0};                                      // 各区间颗粒数，局部数组全初始化
    double sum = 0;                                                // 球度总和
    for (int var = 0; var < totalNum; ++var) {
        sum += particleRoundness[var];
        if (particleRoundness[var] < dInterval[0]) {
            ++distribNum[0];
        } else if (particleRoundness[var] <= dInterval[1] && particleRoundness[var] > dInterval[0]) {
            ++distribNum[1];
        } else if (particleRoundness[var] <= dInterval[2] && particleRoundness[var] > dInterval[1]) {
            ++distribNum[2];
        } else if (particleRoundness[var] <= dInterval[3] && particleRoundness[var] > dInterval[2]) {
            ++distribNum[3];
        } else if (particleRoundness[var] <= dInterval[4] && particleRoundness[var] > dInterval[3]) {
            ++distribNum[4];
        } else if (particleRoundness[var] <= dInterval[5] && particleRoundness[var] > dInterval[4]) {
            ++distribNum[5];
        } else if (particleRoundness[var] <= dInterval[6] && particleRoundness[var] > dInterval[5]) {
            ++distribNum[6];
        } else if (particleRoundness[var] <= dInterval[7] && particleRoundness[var] > dInterval[6]) {
            ++distribNum[7];
        } else if (particleRoundness[var] <= dInterval[8] && particleRoundness[var] > dInterval[7]) {
            ++distribNum[8];
        } else if (particleRoundness[var] <= dInterval[9] && particleRoundness[var] > gDInterval[8]) {
            ++distribNum[9];
        }
    }
    for (int i = 0; i < 10; ++i) {
        QString rate = QString::number(double(distribNum[i]) / totalNum * 100, 'f', 2);
        distribRate.push_back(rate.toDouble());
    }
    // 进行保留2位小数舍尾修正,将舍去加到最高分布区间0.7-0.8
    double rateSum = 0;
    for (double var : distribRate)
        rateSum += var;
    if (rateSum != 100)
        distribRate[7] += (100 - rateSum);
    // 统计结果
    generateReslt->roundness = sum / totalNum;
    // 区间分布
    generateReslt->roundnessRate = distribRate;
    double tmp = 0;
    for (unsigned var = 0; var < distribRate.size(); ++var) {
        tmp += distribRate[var];
        generateReslt->roundnessAccu.push_back(tmp);
    }
}

// 绘制PDF
void GenerateRepo::drawPdf(QByteArray *pdf) {
    QBuffer pdfbuf(pdf);
    pdfbuf.open(QBuffer::WriteOnly);
    QPdfWriter pdfWriter(&pdfbuf);
    // 页面设置PageLayot
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setResolution(96);                                 // DPI
    pdfWriter.setPageMargins(QMarginsF(31.8, 25.4, 31.8, 25.4)); // 单位默认mm
    // Qpainter绘制层绑定

    QPainter painter(&pdfWriter);
    // 字体样式初始化
    // pointSize与pixelSize
    // pointSize:字体点大小，默认12point,1Point等于 1/72 英寸,在72dpi页面即1px
    // 优点：不随dpi改变而缩放
    // pixelSize:像素大小
    // QFont中二者只一个生效

    QFont font[3] = {
        QFont("宋体", 26, 800),        // 一级标题
        QFont("宋体", 10, 500),        // 正文字体
        QFont("宋体", 12, QFont::Bold) // 二级标题
    };
    // 获取视口，即pdf可绘制区域()
    // qDebug() << painter.viewport();
    int pdfWidth = painter.viewport().width();
    // int pdfHeigt = painter.viewport().height();

    // 标题
    y = 0;
    painter.setFont(font[0]);
    painter.drawText(QRect(0, y, pdfWidth, 64), Qt::AlignCenter, "石英砂性能测试报告");
    y += 64; // 标题
             // **********************信息区****************************

    painter.setFont(font[1]);
    painter.setPen(QPen(Qt::black, 1.3));
    painter.drawRect(QRect(0, y, pdfWidth - 1, 96));
    painter.drawText(QRect(50, y, 200, 32), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("样品名称:%1石英砂").arg(generateReslt->sampleType));
    painter.drawText(QRect(250, y, pdfWidth - 250, 32), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("样品来源:%1").arg(generateReslt->sampleSource));
    y += 32;
    painter.drawText(QRect(50, y, 200, 32), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("样品编号:%1").arg(generateReslt->taskID));
    painter.drawText(QRect(250, y, pdfWidth - 250, 32), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("检测单位:%1").arg(generateReslt->testingUnit));
    y += 32;
    painter.drawText(QRect(50, y, 200, 32), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("检测人员:%1").arg(generateReslt->inspector));
    painter.drawText(QRect(250, y, pdfWidth - 250, 32), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("检测日期:%1").arg(generateReslt->testDate));
    y += 32 + 12;

    QStringList list;
    list << "指标名称"
         << "标准要求"
         << "实测"
         << "单项结论";
    drawForm(&painter, y, 1, 4, 50, font[2], list);
    y += 50;
    list.clear();
    list << "  1.粒径分布"
         << "粒径分布在425-212μm\n之间大于90%" << QString::number(generateReslt->granularityPass, 'f', 2) << getCheckResul(generateReslt->granularityPass, 1);
    drawForm(&painter, y, 1, 4, 50, font[1], list, false);
    y += 50;
    list.clear();
    list << "  2.圆度"
         << ">=0.6" << QString::number(generateReslt->roundness, 'f', 2) << getCheckResul(generateReslt->roundness, 2);
    list << "  3.球度"
         << ">=0.6" << QString::number(generateReslt->sphericity, 'f', 2) << getCheckResul(generateReslt->sphericity, 3);
    list << "  4.体密度 g/cm³"
         << "/" << QString::number(generateReslt->bulkDensity, 'f', 2) << " ";
    list << "  5.视密度 g/cm³"
         << "/" << QString::number(generateReslt->apparentDensity, 'f', 2) << " ";
    drawForm(&painter, y, 4, 4, 32, font[1], list, false);
    y += 32 * 4;
    list.clear();

    //************************粒度分析结果********************
    list << "粒度分析结果";
    drawForm(&painter, y, 1, 1, 32, font[2], list);
    y += 32;
    list.clear();
    list << "粒径范围 μm"
         << "区间分布 %"
         << "累积分布 %";
    list << QString(">%1").arg(gDInterval[6])
         << QString::number(generateReslt->granularityRate[7], 'f', 2)
         << QString::number(generateReslt->granularityAccu[7], 'f', 2);
    list << QString("%1-%2").arg(gDInterval[5]).arg(gDInterval[6])
         << QString::number(generateReslt->granularityRate[6], 'f', 2)
         << QString::number(generateReslt->granularityAccu[6], 'f', 2);
    list << QString("%1-%2").arg(gDInterval[4]).arg(gDInterval[5])
         << QString::number(generateReslt->granularityRate[5], 'f', 2)
         << QString::number(generateReslt->granularityAccu[5], 'f', 2);
    list << QString("%1-%2").arg(gDInterval[3]).arg(gDInterval[4])
         << QString::number(generateReslt->granularityRate[4], 'f', 2)
         << QString::number(generateReslt->granularityAccu[4], 'f', 2);
    list << QString("%1-%2").arg(gDInterval[2]).arg(gDInterval[3])
         << QString::number(generateReslt->granularityRate[3], 'f', 2)
         << QString::number(generateReslt->granularityAccu[3], 'f', 2);
    list << QString("%1-%2").arg(gDInterval[1]).arg(gDInterval[2])
         << QString::number(generateReslt->granularityRate[2], 'f', 2)
         << QString::number(generateReslt->granularityAccu[2], 'f', 2);
    list << QString("%1-%2").arg(gDInterval[0]).arg(gDInterval[1])
         << QString::number(generateReslt->granularityRate[1], 'f', 2)
         << QString::number(generateReslt->granularityAccu[1], 'f', 2);
    list << QString("<%1").arg(gDInterval[0])
         << QString::number(generateReslt->granularityRate[0], 'f', 2)
         << QString::number(generateReslt->granularityAccu[0], 'f', 2);
    drawForm(&painter, y, 9, 3, 32, font[1], list);
    y += 9 * 32;
    // 粒径特征点位值区域
    int leftmargin = 12;
    int fPHeight = 24;
    int fPWidth = 150;
    painter.drawRect(0, y, 150, 232);
    painter.drawText(QRect(leftmargin, y, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("D10:%1").arg(generateReslt->d10));
    painter.drawText(QRect(leftmargin, y + fPHeight, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("D50:%1").arg(generateReslt->d50));
    painter.drawText(QRect(leftmargin, y + fPHeight * 2, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("D84:%1").arg(generateReslt->d84));
    painter.drawText(QRect(leftmargin, y + fPHeight * 3, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("D90:%1").arg(generateReslt->d90));
    painter.drawText(QRect(leftmargin, y + fPHeight * 4, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("D97:%1").arg(generateReslt->d97));
    painter.drawText(QRect(leftmargin, y + fPHeight * 5, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("最大粒径:%1").arg(generateReslt->maxGranularity));
    painter.drawText(QRect(leftmargin, y + fPHeight * 6, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("最小粒径:%1").arg(generateReslt->minGranularity));
    painter.drawText(QRect(leftmargin, y + fPHeight * 7, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("平均粒径:%1").arg(generateReslt->averageGranularity));
    painter.drawText(QRect(leftmargin, y + fPHeight * 8, fPWidth, fPHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     QString("中值粒径:%1").arg(generateReslt->medianGranularity));

    // 粒径分布图
    painter.drawRect(150, y, pdfWidth - 151, 232);
    drawChart(&painter, 150, y, pdfWidth - 150, 232, generateReslt->granularityRate, generateReslt->granularityAccu, gDInterval, 1);
    y += 232;

    //*******************球度分析结果************************
    pdfWriter.newPage();
    y = 0;
    y += 12; // 间隔
    list.clear();
    list << "球度分析结果";
    drawForm(&painter, y, 1, 1, 32, font[2], list);

    y += 32;
    list.clear();
    list << "球度范围"
         << "区间分布 %"
         << "累积分布 %";
    list << "0.9-1.0" << QString::number(generateReslt->sphericityRate[9], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[9], 'f', 2);
    list << "0.8-0.9" << QString::number(generateReslt->sphericityRate[8], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[8], 'f', 2);
    list << "0.7-0.8" << QString::number(generateReslt->sphericityRate[7], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[7], 'f', 2);
    list << "0.6-0.7" << QString::number(generateReslt->sphericityRate[6], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[6], 'f', 2);
    list << "0.5-0.6" << QString::number(generateReslt->sphericityRate[5], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[5], 'f', 2);
    list << "0.4-0.5" << QString::number(generateReslt->sphericityRate[4], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[4], 'f', 2);
    list << "0.3-0.4" << QString::number(generateReslt->sphericityRate[3], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[3], 'f', 2);
    list << "0.2-0.3" << QString::number(generateReslt->sphericityRate[2], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[2], 'f', 2);
    list << "0.1-0.2" << QString::number(generateReslt->sphericityRate[1], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[1], 'f', 2);
    list << "0.0-0.1" << QString::number(generateReslt->sphericityRate[0], 'f', 2)
         << QString::number(generateReslt->sphericityAccu[0], 'f', 2);
    drawForm(&painter, y, 11, 3, 32, font[1], list);
    y += 11 * 32;
    // 球度分布图
    painter.drawRect(0, y, pdfWidth - 1, 280);
    std::vector<double> axisXScale = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9}; // x轴刻度
    drawChart(&painter, 0, y, pdfWidth, 280, generateReslt->sphericityRate, generateReslt->sphericityAccu, axisXScale, 0);
    y += 280 + 12;

    //*****************************圆度分析结果*************************
    list.clear();
    list << "球度分析结果";
    drawForm(&painter, y, 1, 1, 32, font[2], list);
    y += 32;
    list.clear();
    list << "圆度范围"
         << "区间分布 %"
         << "累积分布 %";
    list << "0.9-1.0" << QString::number(generateReslt->roundnessRate[9], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[9], 'f', 2);
    list << "0.8-0.9" << QString::number(generateReslt->roundnessRate[8], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[8], 'f', 2);
    list << "0.7-0.8" << QString::number(generateReslt->roundnessRate[7], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[7], 'f', 2);
    list << "0.6-0.7" << QString::number(generateReslt->roundnessRate[6], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[6], 'f', 2);
    list << "0.5-0.6" << QString::number(generateReslt->roundnessRate[5], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[5], 'f', 2);
    list << "0.4-0.5" << QString::number(generateReslt->roundnessRate[4], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[4], 'f', 2);
    drawForm(&painter, y, 7, 3, 32, font[1], list);

    // 换页
    pdfWriter.newPage();
    y = 0;
    list.clear();
    list << "0.3-0.4" << QString::number(generateReslt->roundnessRate[3], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[3], 'f', 2);
    list << "0.2-0.3" << QString::number(generateReslt->roundnessRate[2], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[2], 'f', 2);
    list << "0.1-0.2" << QString::number(generateReslt->roundnessRate[1], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[1], 'f', 2);
    list << "0.0-0.1" << QString::number(generateReslt->roundnessRate[0], 'f', 2)
         << QString::number(generateReslt->roundnessAccu[0], 'f', 2);
    drawForm(&painter, y, 4, 3, 32, font[1], list);
    y += 4 * 32;
    // 球度分布图
    painter.drawRect(0, y, pdfWidth - 1, 280);
    drawChart(&painter, 0, y, pdfWidth, 280, generateReslt->roundnessRate, generateReslt->roundnessAccu, axisXScale, 0);
    y += 280;

    // 绘制结束，关闭buffe
    painter.end();
    // 在向对象中绘制时绘制完毕需要终止画刷，paintEvent中方可自动关闭
    pdfbuf.close();
}
// 画笔，垂直指针，行数，列数，单元格高度，字体，字符串，首列是否居中（默认居中，0左对齐）
void GenerateRepo::drawForm(QPainter *painter, int y, int rows, int columns, int cellHeight, QFont &font, QStringList &list, bool flag) {
    painter->setFont(font);
    int width = painter->viewport().width() - 1;
    int cellWidth = width / columns;
    for (int i = 0; i < rows; ++i) {
        int x = 0;
        for (int j = 0; j < columns; ++j) {

            if (!flag && j == 0) {
                painter->drawText(QRect(x, y, cellWidth, cellHeight), Qt::AlignLeft | Qt::AlignVCenter, list[i * columns + j]);
            } else {
                painter->drawText(QRect(x, y, cellWidth, cellHeight), Qt::AlignCenter, list[i * columns + j]);
            }
            painter->drawRect(QRect(x, y, cellWidth, cellHeight));
            x += cellWidth;
        }
        y += cellHeight;
    }
}
// 粒径统计图：画笔、x,y,宽（400）、高、区间分布、累计分布、分布区间（轴间刻度值）、是否为粒度
void GenerateRepo::drawChart(QPainter *painter, int pointx, int pointy, int width, int height,
                             std::vector<double> &vecDisRate, std::vector<double> &vecDisAccu, std::vector<double> &vecDisInterval, bool flag) {
    int size = vecDisRate.size();
    int pointSize = 4;
    painter->save();
    painter->setFont(QFont("宋体", 6));
    painter->setRenderHint(QPainter::Antialiasing); // 抗锯齿
    painter->setPen(QPen(Qt::black, 2));

    // 统计图content区域
    int padding = 20;              // 内边距
    int chartx = pointx + padding; // 顶点X
    int charty = pointy + padding; // 顶点Y
    int chartHeight = height - padding * 2;
    int chartWidth = width - padding * 2;

    painter->drawLine(chartx - 1, charty + chartHeight, chartx + chartWidth + 1, charty + chartHeight); // X轴
    painter->drawLine(chartx - 2, charty, chartx - 2, charty + chartHeight);                            // 区间分布轴
    painter->drawLine(chartx + chartWidth + 2, charty, chartx + chartWidth + 2, charty + chartHeight);  // 累计分布轴

    double maxY = 0;                                                                                    // 最大区间分布值                                                                         // 区间分布值对应区间
    for (int var = 0; var < size; ++var) {
        if (maxY < vecDisRate[var]) {
            maxY = vecDisRate[var];
        }
    }
    // 坐标轴比例系数，kx为1格宽度，ky为1分布高度
    double kx = double(chartWidth) / (vecDisInterval.size() + 1);
    double ky = double(chartHeight) / maxY;
    double kyAcc = double(chartHeight) / 100;
    // X轴刻度线绘制
    for (int var = 0; var < size - 1; ++var) {
        // 刻度
        painter->setPen(QPen(Qt::black, 1.5));
        painter->drawLine(chartx + kx * (var + 1), charty + chartHeight,
                          chartx + kx * (var + 1), charty + chartHeight - 3);

        painter->drawText(QRect(chartx + kx * (var + 0.5), charty + chartHeight, kx, 20),
                          Qt::AlignCenter, QString::number(vecDisInterval[var]));
        // 直线
        painter->setPen(QPen(QColor(0xd3d3d3), 1));
        painter->drawLine(chartx + kx * (var + 1), charty + chartHeight,
                          chartx + kx * (var + 1), charty);
    }
    // Y轴刻度线
    for (int var = 0; var < 10; ++var) {
        // 直线
        painter->setPen(QPen(QColor(0xd3d3d3), 1));
        painter->drawLine(chartx, charty + chartHeight * (0.9 - 0.1 * var),
                          chartx + chartWidth, charty + chartHeight * (0.9 - 0.1 * var));
        // 线在前可自动恢复字体颜色
        painter->setPen(QPen(Qt::black, 1.5));
        // 左刻度
        painter->drawLine(chartx, charty + chartHeight * (0.9 - 0.1 * var),
                          chartx + 3, charty + chartHeight * (0.9 - 0.1 * var));
        // 右刻度
        painter->drawLine(chartx + chartWidth - 3, charty + chartHeight * (0.9 - 0.1 * var),
                          chartx + chartWidth, charty + chartHeight * (0.9 - 0.1 * var));
        // 左文字
        painter->drawText(QRect(chartx - 24, charty + chartHeight * (0.9 - 0.1 * var) - 10, 20, 20),
                          Qt::AlignVCenter | Qt::AlignRight, QString::number(int(maxY * 0.1 * (var + 1))));
        // 右文字
        painter->drawText(QRect(chartx + chartWidth + 4, charty + chartHeight * (0.9 - 0.1 * var) - 10, 20, 20),
                          Qt::AlignVCenter | Qt::AlignLeft, QString::number(10 * (var + 1)));
    }
    painter->setPen(QPen(Qt::black, 1.3));

    // 区间分布
    // 图例文本
    painter->drawText(QRect(chartx, pointy, 70, padding), Qt::AlignLeft | Qt::AlignVCenter, "区间分布 %");
    if (flag) { // 粒度分布为折线图

        // 图例
        painter->setPen(QPen(QColor(0x32cd32), 1.3));
        painter->drawLine(chartx + 50, pointy + padding / 2, chartx + 90, pointy + padding / 2);
        // 第一段
        painter->drawLine(chartx, charty + chartHeight, chartx + kx * 0.5, charty + chartHeight - vecDisRate[0] * ky);
        painter->setPen(QPen(Qt::blue, pointSize));
        painter->drawPoint(pointx + kx * 0.5, pointy + chartHeight - vecDisRate[0] * ky);
        for (unsigned int var = 0; var < vecDisRate.size() - 1; ++var) {
            painter->setPen(QPen(QColor(0x32cd32), 1.3));
            painter->drawLine(chartx + kx * (var + 0.5), charty + chartHeight - vecDisRate[var] * ky,
                              chartx + kx * (var + 1.5), charty + chartHeight - vecDisRate[var + 1] * ky);
            painter->setPen(QPen(Qt::blue, pointSize));
            painter->drawPoint(chartx + kx * (var + 1.5), charty + chartHeight - vecDisRate[var + 1] * ky);
        }
    } else { // 圆度球度度为柱状图

        // 图例
        painter->setPen(QPen(Qt::blue, 1));
        painter->setBrush(Qt::FDiagPattern);
        painter->drawRect(chartx + 50, pointy + (padding - 10) / 2, 10, 10);
        // 柱状图
        // 使用负高度确保底端对齐
        for (unsigned int var = 0; var < vecDisRate.size(); ++var) {
            painter->drawRect(chartx + kx * var, charty + chartHeight, kx, -vecDisRate[var] * ky);
        }
        painter->setBrush(Qt::NoBrush);
    }

    // 累积分布折线
    // 图例
    painter->setPen(QPen(Qt::black, 1.3));
    painter->drawText(QRect(chartx + chartWidth - 70, pointy, 70, padding), Qt::AlignRight | Qt::AlignVCenter, "累积分布 %");
    painter->setPen(QPen(Qt::red, 1.3));
    painter->drawLine(pointx + chartWidth - 75, pointy + padding / 2, pointx + chartWidth - 30, pointy + padding / 2);
    // 第一段
    painter->drawLine(chartx, charty + chartHeight, chartx + kx * 0.5, charty + chartHeight - vecDisAccu[0] * kyAcc);
    painter->setPen(QPen(Qt::blue, pointSize));
    painter->drawPoint(chartx + kx * 0.5, charty + chartHeight - vecDisAccu[0] * kyAcc);
    for (unsigned int var = 0; var < vecDisAccu.size() - 1; ++var) {
        painter->setPen(QPen(Qt::red, 1.3));
        painter->drawLine(chartx + kx * (var + 0.5), charty + chartHeight - vecDisAccu[var] * kyAcc,
                          chartx + kx * (var + 1.5), charty + chartHeight - vecDisAccu[var + 1] * kyAcc);
        painter->setPen(QPen(Qt::blue, pointSize));
        painter->drawPoint(chartx + kx * (var + 1.5), charty + chartHeight - vecDisAccu[var + 1] * kyAcc);
    }
    // 最后一段
    painter->setPen(QPen(Qt::red, 1.3));
    painter->drawLine(chartx + chartWidth - 0.5 * kx, charty, chartx + +chartWidth, charty);
    /*
        // 粒径分布中值点
        if (flag) {
            painter->setPen(QPen(Qt::black, 1.3));
            painter->drawText(QRect(pointx + width / 2 + 15, pointy, 15, padding), Qt::AlignLeft | Qt::AlignVCenter, "d50");
            painter->setPen(QPen(QColor(0xff1493), 1.3, Qt::DashLine));
            painter->drawLine(pointx + width / 2 - 30, pointy + padding / 2, pointx + width / 2 + 10, pointy + padding / 2);
            painter->drawLine(chartx, charty + chartHeight / 2, chartx + chartWidth, charty + chartHeight / 2);
        };*/
    painter->restore(); // 恢复painter
}

QString GenerateRepo::getCheckResul(double data, int checkCase) {
    /*获取单项结论
     * checkCase
     *  1:粒径分布
     *  2：圆度
     *  3：球度*/
    QString s1 = "符合";
    QString s0 = "不符合";
    QString res = " ";
    switch (checkCase) {
    case 1:
        res = data >= 90 ? s1 : s0;
        break;
    case 2:
        res = data >= 0.6 ? s1 : s0;
        break;
    case 3:
        res = data >= 0.6 ? s1 : s0;
        break;
    default:
        res = "输入错误";
    }
    return res;
}
