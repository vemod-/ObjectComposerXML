#include <QApplication>
#include "mainwindow.h"
#include <QSettings>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontShowIconsInMenus);
    a.setWindowIcon(QIcon(":/ocicon.png"));
    a.setApplicationName("ObjectComposerXML");
    a.setOrganizationName("Veinge Musik och Data");
    a.setOrganizationDomain("http://www.musiker.nu/objectstudio");
    QSettings s(a.applicationDirPath() + "/../Info.plist",QSettings::NativeFormat);
    s.setValue("LSMultipleInstancesProhibited",true);
    s.setValue("CFBundleIdentifier","com.http-www-musiker-nu-objectstudio.ObjectComposerXML");
    s.setValue("CFBundleIconFile","ocicon");
    MainWindow w;
    w.show();
    return a.exec();
}
