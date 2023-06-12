#ifndef IMGWIDGET_H
#define IMGWIDGET_H

#include <QWidget>

class ImgWidget : public QWidget {
  Q_OBJECT
public:
  explicit ImgWidget(QWidget *parent = nullptr);
  void refresh();
  void changeShowFlag() { showFlag = !showFlag; };
  void pushBuf(const std::vector<QImage> &images);
signals:
protected:
  void paintEvent(QPaintEvent *event) override;

private:
  std::list<std::vector<QImage>> imgsBuf;
  QTimer *frameTimer;
  QPixmap buffer;
  int index = 0;
  bool showFlag = true;
};

#endif // IMGWIDGET_H
