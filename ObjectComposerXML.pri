#-------------------------------------------------
#
# Project created by QtCreator 2010-10-20T17:05:04
#
#-------------------------------------------------
cache()

#CONFIG += sdk_no_version_check

#DEFINES += QT_USE_QSTRINGBUILDER
#DEFINES += QT_USE_FAST_CONCATENATION
#DEFINES += QT_USE_FAST_OPERATOR_PLUS

#QMAKE_CXXFLAGS -= -O2
#QMAKE_CXXFLAGS += -O3 -march=native

#CONFIG += qt ##x86
#CONFIG += x86_64
#CONFIG += c++17

#QT += core gui
greaterThan(QT_MAJOR_VERSION, 4){
    QT += printsupport
}

QT += concurrent

#QT -= network opengl sql svg xml xmlpatterns qt3support

#macx {
#    DEFINES += __MACOSX_CORE__
#}
DEFINES += __STYLESHEETS__

#LIBS += -framework AppKit ##-framework CoreMidi -framework CoreAudio

include($$PWD/../SoftSynths/RtAudioBuffer/Libs.pri)
include($$PWD/../midi2wav/midi2wav.pri)
include($$PWD/../QiPhotoRubberband/QiPhotoRubberband.pri)
include($$PWD/../QFadingWidget/QFadingWidget.pri)
include($$PWD/../XMLTest/MusicXML.pri)
include($$PWD/../ProjectPage/projectpage.pri)
include($$PWD/../quazip/quazip.pri)
##include($$PWD/../SoftSyntsWidgets/SoftSynthsWidgets.pri)

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../QDomLite
INCLUDEPATH += $$PWD/../midi2wav
INCLUDEPATH += $$PWD/../EventHandlers
INCLUDEPATH += $$PWD/../XMLTest
INCLUDEPATH += $$PWD/../QGraphicsViewZoomer
INCLUDEPATH += $$PWD/../SoftSynthsWidgets

SOURCES += $$PWD/../EventHandlers/mouseevents.cpp \
    ##$$PWD/cdocumentloader.cpp \
    $$PWD/ocsymbolscollection.cpp \
    $$PWD/ocsignlist.cpp \
    $$PWD/ocscore.cpp \
    $$PWD/ocnotelist.cpp \
    $$PWD/csymbol.cpp \
    $$PWD/scoreviewxml.cpp \
    $$PWD/cscoredoc.cpp \
    $$PWD/cmusictree.cpp \
    $$PWD/cpropertywindow.cpp \
    $$PWD/cbarwindow.cpp \
    $$PWD/cstaffsdialog.cpp \
    $$PWD/qtoolbuttongrid.cpp \
    $$PWD/ocpiano.cpp \
    $$PWD/clayout.cpp \
    $$PWD/layoutviewxml.cpp \
    $$PWD/clayoutwizard.cpp \
    $$PWD/cscorewizard.cpp \
    $$PWD/ceditwidget.cpp \
    $$PWD/caccidentalspiano.cpp \
    $$PWD/ceditdialog.cpp \
    $$PWD/cpresetsdialog.cpp \
    $$PWD/cpatternlist.cpp \
    $$PWD/ceditsystem.cpp \
    $$PWD/qmacbuttons.cpp \
    $$PWD/ocnotetoolbox.cpp \
    $$PWD/ocplaycontrol.cpp \
    $$PWD/ctweaksystem.cpp \
    $$PWD/qapplearrowbutton.cpp \
    $$PWD/qfadingframe.cpp \
    $$PWD/qmacsplitter.cpp \
    $$PWD/qhoverbutton.cpp \
    $$PWD/qmacrubberband.cpp \
    $$PWD/qcustomframe.cpp \
    $$PWD/cmeasurecontrol.cpp \
    $$PWD/qbinarymemoryfile.cpp \
    $$PWD/qfontwidget.cpp \
    ##csidebartree.cpp \
    $$PWD/qmactreewidget.cpp \
    $$PWD/ocxmlwrappers.cpp


HEADERS  += $$PWD/../EventHandlers/mouseevents.h \
    $$PWD/../QGraphicsViewZoomer/qgraphicsviewzoomer.h \
    $$PWD/../SoftSynths/RtAudioBuffer/cthreadedfunction.h \
    $$PWD/CommonClasses.h \
    $$PWD/CommonCounters.h \
    ##$$PWD/cdocumentloader.h \
    $$PWD/czoomwidget.h \
    $$PWD/ocscore.h \
    $$PWD/ocsymbolscollection.h \
    $$PWD/ocsignlist.h \
    $$PWD/ocnotelist.h \
    $$PWD/csymbol.h \
    $$PWD/scoreviewxml.h \
    $$PWD/cscoredoc.h \
    $$PWD/cmusictree.h \
    $$PWD/cpropertywindow.h \
    $$PWD/cbarwindow.h \
    $$PWD/cstaffsdialog.h \
    $$PWD/qtoolbuttongrid.h \
    $$PWD/ocpiano.h \
    $$PWD/clayout.h \
    $$PWD/layoutviewxml.h \
    $$PWD/clayoutwizard.h \
    $$PWD/cscorewizard.h \
    $$PWD/ceditwidget.h \
    $$PWD/caccidentalspiano.h \
    $$PWD/ceditdialog.h \
    $$PWD/cpresetsdialog.h \
    $$PWD/cpatternlist.h \
    $$PWD/ceditsystem.h \
    $$PWD/qmacbuttons.h \
    $$PWD/ocnotetoolbox.h \
    $$PWD/ocplaycontrol.h \
    $$PWD/ctweaksystem.h \
    $$PWD/qapplearrowbutton.h \
    $$PWD/qtransmenu.h \
    $$PWD/qfadingframe.h \
    $$PWD/qmacsplitter.h \
    $$PWD/qhoverbutton.h \
    $$PWD/qmacrubberband.h \
    $$PWD/qcustomframe.h \
    $$PWD/cmeasurecontrol.h \
    $$PWD/qbinarymemoryfile.h \
    $$PWD/qfontwidget.h \
    ##csidebartree.h \
    $$PWD/qmactreewidget.h \
    $$PWD/ocbarmap.h \
    $$PWD/ocxmlwrappers.h \
    $$PWD/midifileclasses.h

FORMS    += $$PWD/cscoredoc.ui \
    $$PWD/cmusictree.ui \
    $$PWD/cpropertywindow.ui \
    $$PWD/cbarwindow.ui \
    $$PWD/cstaffsdialog.ui \
    $$PWD/ocpiano.ui \
    $$PWD/ScoreViewXML.ui \
    $$PWD/layoutviewxml.ui \
    $$PWD/clayoutwizard.ui \
    $$PWD/cscorewizard.ui \
    $$PWD/ceditwidget.ui \
    $$PWD/caccidentalspiano.ui \
    $$PWD/ceditdialog.ui \
    $$PWD/cpresetsdialog.ui \
    $$PWD/cpatternlist.ui \
    $$PWD/ceditsystem.ui \
    $$PWD/qmacbuttons.ui \
    $$PWD/ocnotetoolbox.ui \
    $$PWD/ocplaycontrol.ui \
    $$PWD/ctweaksystem.ui \
    $$PWD/cmeasurecontrol.ui \
    $$PWD/qfontwidget.ui ##\
    ##csidebartree.ui


RESOURCES += \
    $$PWD/OCResources.qrc

#macx {
#    # Name of the application signing certificate
#    APPCERT = \"3rd Party Mac Developer Application: Veinge Musik och Data\"
#    # Name of the installer signing certificate
#    INSTALLERCERT = \"3rd Party Mac Developer Installer: Veinge Musik och Data\"

#    # Bundle identifier for your application
#    BUNDLEID = com.http-www-musiker-nu-objectstudio.ObjectComposerXML

#    # Name of the entitlements file (only needed if you want to sandbox the application)
#    ENTITLEMENTS = Entitlements.plist

#    ICON = ocicon.icns
#     QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_OBJECTIVE_CFLAGS_RELEASE =  $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_INFO_PLIST = Info.plist
#    OTHER_FILES += Info.plist \
#                 $${ENTITLEMENTS}
#    codesign.depends  += all
#    codesign.commands += macdeployqt $${TARGET}.app;

#    # Remove unneeded frameworks (uncomment and change to suit your application)
#    codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtDeclarative.framework;
#    #codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtXml.framework;
#    #codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtOpenGL.framework;
#    #codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtDBus.framework;

#    # Remove unneeded plug-ins (uncomment and change to suit your application)
#    codesign.commands += rm -r $${TARGET}.app/Contents/PlugIns/accessible;

#    # Extract debug symbols
#    #codesign.commands += dsymutil $${TARGET}.app/Contents/MacOS/$${TARGET} -o $${TARGET}.app.dSYM;

#    # Sign frameworks and plug-ins (uncomment and change to suit your application)
#    #codesign -s "$APPCERT" -i $BUNDLEID $${TARGET}.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
#    #codesign -s "$APPCERT" -i $BUNDLEID $${TARGET}.app/Contents/PlugIns/imageformats/libqjpeg.dylib

#    # Sign the application bundle, using the provided entitlements
#    codesign.commands += codesign -f -s $${APPCERT} -v --entitlements $${ENTITLEMENTS} $${TARGET}.app;
#    product.depends += all

#    # Build the product package
#    product.commands += productbuild --component $${TARGET}.app /Applications --sign $${INSTALLERCERT} $${TARGET}.pkg;
#    QMAKE_EXTRA_TARGETS += codesign product copyfiles
#}

##DISTFILES += \
##    ocgrey.png \
##    ocicon.png \
##    ocicon.icns

##QMAKE_INFO_PLIST = Info.plist
##ICON = ocicon.icns
