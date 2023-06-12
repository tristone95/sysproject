cmake_minimum_required(VERSION 3.5)

project(sys_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PDFium_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pdfium)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(PDFium)
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR})
set(PROJECT_SOURCES
        global.h
        main.cpp
        widget.cpp
        widget.h
        widget.ui
        mainwindow.h
        mainwindow.cpp
        mainwindow.ui
        taskinfoform.h
        taskinfoform.cpp
        taskinfoform.ui
        processview.h
        processview.cpp
        processview.ui
        imgwidget.h
        imgwidget.cpp
        resultviewer.h
        resultviewer.cpp
        resultviewer.ui
        sandcard.h
        sandcard.cpp
        sandcard.ui
        sandinfodialog.h
        sandinfodialog.cpp
        sandinfodialog.ui
        loadingdialog.h
        loadingdialog.cpp
        loadingdialog.ui
        pdfviewerdialog.h
        pdfviewerdialog.cpp
        pdfviewerdialog.ui
        imgview.h
        imgview.cpp
        res.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(sys_client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET sys_client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(sys_client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(sys_client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(sys_client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(sys_client PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(sys_client PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(sys_client PUBLIC pdfium)
set_target_properties(sys_client PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS sys_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(sys_client)
endif()
