#ifndef PROCESSVIEW_H
#define PROCESSVIEW_H

#include <QWidget>

namespace Ui {
class ProcessView;
}
class QTimer;
class ProcessView : public QWidget {
    Q_OBJECT

public:
    explicit ProcessView(bool manual, int num = 0, QWidget *parent = nullptr);
    ~ProcessView();
    void initialWin(bool manual, int num);
    void upData(int progressNum, int totalNum, const std::vector<QImage> &imgs);
    void upData(int progressNum, int totalNum);
    /*
    void startImg();
    void stopImg(int num, bool manual); // 手动模式停止采集
     */
    void showImg();
signals:
    void stop();
private slots:
    void on_pushButton_clicked();

private:
    Ui::ProcessView *ui;
    std::list<std::vector<QImage>> showImgBuf;
    QTimer *refreshTimer;
    bool manualFlag;
    int index = 0;
};

#endif // PROCESSVIEW_H
