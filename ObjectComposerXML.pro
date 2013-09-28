#-------------------------------------------------
#
# Project created by QtCreator 2010-10-20T17:05:04
#
#-------------------------------------------------
cache()
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
CONFIG += qt x86

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += macextras printsupport widgets

QT -= network opengl sql svg xml xmlpatterns qt3support

include(../midi2wav/midi2wav.pri)
include(../QiPhotoRubberband/QiPhotoRubberband.pri)

INCLUDEPATH += ../midi2wav

TARGET = ObjectComposerXML
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    CommonClasses.cpp \
    CommonCounters.cpp \
    ocsymbolscollection.cpp \
    ocsignlist.cpp \
    ocscore.cpp \
    ocnotelist.cpp \
    csymbol.cpp \
    MidiFileClasses.cpp \
    scoreviewxml.cpp \
    cscoredoc.cpp \
    cmusictree.cpp \
    cpropertywindow.cpp \
    cbarwindow.cpp \
    cstaffsdialog.cpp \
    qtoolbuttongrid.cpp \
    ocpiano.cpp \
    clayout.cpp \
    layoutviewxml.cpp \
    clayoutwizard.cpp \
    cscorewizard.cpp \
    ceditwidget.cpp \
    caccidentalspiano.cpp \
    ceditdialog.cpp \
    cpresetsdialog.cpp \
    cpatternlist.cpp \
    ceditsystem.cpp \
    qmacbuttons.cpp \
    ocnotetoolbox.cpp \
    ocplaycontrol.cpp \
    ctweaksystem.cpp \
    qapplearrowbutton.cpp \
    qpicframe.cpp \
    qfadingwidget.cpp \
    qtransmenu.cpp \
    qfadingframe.cpp \
    qmacsplitter.cpp \
    qhoverbutton.cpp \
    qmacrubberband.cpp \
    qcustomframe.cpp \
    cmeasurecontrol.cpp \
    qbinarymemoryfile.cpp \
    qfontwidget.cpp \
    csidebartree.cpp \
    qmactreewidget.cpp

OBJECTIVE_SOURCES += cocoaprogressbar.mm

HEADERS  += mainwindow.h \
    CommonClasses.h \
    MidiFileClasses.h \
    CommonCounters.h \
    ocscore.h \
    ocsymbolscollection.h \
    ocsignlist.h \
    ocnotelist.h \
    csymbol.h \
    scoreviewxml.h \
    cscoredoc.h \
    cmusictree.h \
    cpropertywindow.h \
    cbarwindow.h \
    cstaffsdialog.h \
    qtoolbuttongrid.h \
    ocpiano.h \
    clayout.h \
    layoutviewxml.h \
    clayoutwizard.h \
    cscorewizard.h \
    ceditwidget.h \
    caccidentalspiano.h \
    ceditdialog.h \
    cpresetsdialog.h \
    cpatternlist.h \
    ceditsystem.h \
    qmacbuttons.h \
    ocnotetoolbox.h \
    ocplaycontrol.h \
    ctweaksystem.h \
    qapplearrowbutton.h \
    qpicframe.h \
    qfadingwidget.h \
    cocoaprogressbar.h \
    qtransmenu.h \
    qfadingframe.h \
    qmacsplitter.h \
    qhoverbutton.h \
    qmacrubberband.h \
    qcustomframe.h \
    cmeasurecontrol.h \
    qbinarymemoryfile.h \
    qfontwidget.h \
    csidebartree.h \
    qmactreewidget.h

FORMS    += mainwindow.ui \
    cscoredoc.ui \
    cmusictree.ui \
    cpropertywindow.ui \
    cbarwindow.ui \
    cstaffsdialog.ui \
    ocpiano.ui \
    ScoreViewXML.ui \
    layoutviewxml.ui \
    clayoutwizard.ui \
    cscorewizard.ui \
    ceditwidget.ui \
    caccidentalspiano.ui \
    ceditdialog.ui \
    cpresetsdialog.ui \
    cpatternlist.ui \
    ceditsystem.ui \
    qmacbuttons.ui \
    ocnotetoolbox.ui \
    ocplaycontrol.ui \
    ctweaksystem.ui \
    cmeasurecontrol.ui \
    qfontwidget.ui \
    csidebartree.ui

LIBS += -framework AppKit -framework CoreMidi -framework CoreAudio

DEFINES += __MACOSX_CORE__
DEFINES += __STYLESHEETS__

RESOURCES += \
    ObjectComposerXML.qrc

macx {
    # Name of the application signing certificate
    APPCERT = \"3rd Party Mac Developer Application: Veinge Musik och Data\"
    # Name of the installer signing certificate
    INSTALLERCERT = \"3rd Party Mac Developer Installer: Veinge Musik och Data\"

    # Bundle identifier for your application
    BUNDLEID = com.http-www-musiker-nu-objectstudio.ObjectComposerXML

    # Name of the entitlements file (only needed if you want to sandbox the application)
    ENTITLEMENTS = Entitlements.plist

    ICON = ocicon.icns
     QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_OBJECTIVE_CFLAGS_RELEASE =  $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_INFO_PLIST = Info.plist
    OTHER_FILES += Info.plist \
                 $${ENTITLEMENTS}
    codesign.depends  += all
    codesign.commands += macdeployqt $${TARGET}.app;

    # Remove unneeded frameworks (uncomment and change to suit your application)
    codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtDeclarative.framework;
    #codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtXml.framework;
    #codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtOpenGL.framework;
    #codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtDBus.framework;

    # Remove unneeded plug-ins (uncomment and change to suit your application)
    codesign.commands += rm -r $${TARGET}.app/Contents/PlugIns/accessible;

    # Extract debug symbols
    #codesign.commands += dsymutil $${TARGET}.app/Contents/MacOS/$${TARGET} -o $${TARGET}.app.dSYM;

    # Sign frameworks and plug-ins (uncomment and change to suit your application)
    #codesign -s "$APPCERT" -i $BUNDLEID $${TARGET}.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
    #codesign -s "$APPCERT" -i $BUNDLEID $${TARGET}.app/Contents/PlugIns/imageformats/libqjpeg.dylib

    # Sign the application bundle, using the provided entitlements
    codesign.commands += codesign -f -s $${APPCERT} -v --entitlements $${ENTITLEMENTS} $${TARGET}.app;
    product.depends += all

    # Build the product package
    product.commands += productbuild --component $${TARGET}.app /Applications --sign $${INSTALLERCERT} $${TARGET}.pkg;
    QMAKE_EXTRA_TARGETS += codesign product copyfiles
}
QMAKE_INFO_PLIST = Info.plist
ICON = ocicon.icns
