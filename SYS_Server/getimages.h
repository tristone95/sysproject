#ifndef GETIMAGES_H
#define GETIMAGES_H

#include "global.h"
#include <QObject>
class QTimer;
class GetImages : public QObject {
  Q_OBJECT
public:
  explicit GetImages(int m_frameRate, QObject *parent = nullptr);
  ~GetImages();
  void getFromBuf();
  void start(); // 开始槽
  void stop();  // 停止槽

signals:
  void sendImages(const ImagesPerSec &);

private:
  QTimer *m_Timer = nullptr;
  std::vector<QImage> *cambuffer;                // 模拟相机内存
  std::list<std::vector<QImage> *> imagesBuffer; // 程序缓存区
  int m_frameRate;
  int index=0;
};

#endif // GETIMAGES_H
