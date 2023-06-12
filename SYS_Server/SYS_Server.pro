QT += network gui widgets
CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calculation.cpp \
    cvprocess.cpp \
    generaterepo.cpp \
    getimages.cpp \
        main.cpp \
    tcpcommunication.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    calculation.h \
    cvprocess.h \
    generaterepo.h \
    getimages.h \
    global.h \
    tcpcommunication.h


win32:LIBS += $$PWD/../../opencv/mybuild/lib/libopencv_*.a
INCLUDEPATH += $$PWD/../../opencv/mybuild/install/include
DEPENDPATH += $$PWD/../../opencv/mybuild/install/include
