#include <QApplication>
#include "mainwindow.h"
#include <QSettings>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.setAttribute(Qt::AA_DontShowIconsInMenus);
    a.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    a.setWindowIcon(QIcon(":/ocicon.png"));
    a.setApplicationName(OCPresets().Application());
    a.setOrganizationName("Veinge Musik och Data");
    a.setOrganizationDomain(OCPresets().Organization());
    QSettings s(a.applicationDirPath() + "/../Info.plist",QSettings::NativeFormat);
    s.setValue("LSMultipleInstancesProhibited","True");
    s.setValue("CFBundleIdentifier","com.http-www-musiker-nu-objectstudio.ObjectComposerXML");
    s.setValue("CFBundleIconFile","ocicon");
    s.setValue("NSPrincipalClass","NSApplication");
    s.setValue("NSHighResolutionCapable","True");
    s.sync();
    MainWindow w;
    w.show();
    return a.exec();
}
