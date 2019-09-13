#-------------------------------------------------
#
# Project created by QtCreator 2015-07-08T00:40:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IP5
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    imagearea.cpp

HEADERS  += window.h \
    imagearea.h

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    IP5.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

