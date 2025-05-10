#-------------------------------------------------
#
# Project created by QtCreator 2010-10-20T17:05:04
#
#-------------------------------------------------
cache()

CONFIG += sdk_no_version_check

DEFINES += QT_USE_QSTRINGBUILDER

QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS += -O3 -march=native

CONFIG += qt ##x86
CONFIG += x86_64
CONFIG += c++17

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4){
    QT += printsupport widgets
    QT += macextras
}

QT       += concurrent

QT -= network opengl sql svg xml xmlpatterns qt3support

DEFINES += __MACOSX_CORE__
DEFINES += __STYLESHEETS__
LIBS += -framework AppKit -framework CoreMidi -framework CoreAudio


TARGET = ObjectComposerXML
TEMPLATE = app

DYLDPATH = ../build-SoftSynths-Desktop_Qt_$${QT_MAJOR_VERSION}_$${QT_MINOR_VERSION}_$${QT_PATCH_VERSION}_clang_64bit-Debug/

!exists($${DYLDPATH}) {
    DYLDPATH = ../build-SoftSynths-Desktop_Qt_$${QT_MAJOR_VERSION}_$${QT_MINOR_VERSION}_$${QT_PATCH_VERSION}_clang_64bit_qt_qt$${QT_MAJOR_VERSION}-Debug/
}

LIBS += -L$${DYLDPATH} -lSoftSynthsClasses

include(ObjectComposerXML.pri)

INCLPATH = ../SoftSynths/
INCLUDEPATH += $${INCLPATH}RtAudioBuffer

SOURCES += main.cpp\
        mainwindow.cpp \
        $${INCLPATH}RtAudioBuffer/corebuffer.cpp


HEADERS  += mainwindow.h \
    $${INCLPATH}RtAudioBuffer/corebuffer.h

FORMS    += mainwindow.ui

DISTFILES += \
    ocgrey.png \
    ocicon.png \
    ocicon.icns

QMAKE_INFO_PLIST = Info.plist
ICON = ocicon.icns
