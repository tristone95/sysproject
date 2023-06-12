#ifndef RESULTVIEWER_H
#define RESULTVIEWER_H

#include <QWidget>
#include <global.h>
class SandCard;
class QGridLayout;
class QComboBox;
class QPushButton;
namespace Ui {
class ResultViewer;
}

class ResultViewer : public QWidget {
    Q_OBJECT

public:
    explicit ResultViewer(QWidget *topWind = nullptr);
    ~ResultViewer();
    void initWindow(TaskInfo &info);
    void updateResult(ResultInfo &rInfo);
    void setDatas(std::vector<SandInfo> &vecInfo);
    void showSand();
    void setComboItems(int count);
    void scrollTop();
    QComboBox *getCombox();
    QPushButton *getButton();
signals:
    void showPdf();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButton_2_clicked();

private:
    QWidget *topWindow;
    Ui::ResultViewer *ui;
    std::vector<SandInfo> *sandsData;
    std::vector<SandCard *> showCard;
    bool dataFlag = false;   // 是否有数据标志位
    QGridLayout *cardLayout;
    bool layOutFlag = false; // 是否有布局标志位
    int cols;
};

#endif // RESULTVIEWER_H
