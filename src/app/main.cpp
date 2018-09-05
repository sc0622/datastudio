#include "precomp.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QFontDatabase>

int main(int argc, char **argv)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#if QT_VERSION >= 0x050600
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);

    QApplication app(argc, argv);
    QApplication::setFont(QFont("Microsoft Yahei", 9));
    QApplication::setWindowIcon(QIcon(":/datastudio/image/app.png"));
    QCoreApplication::setOrganizationName("Smartsoft");
    QCoreApplication::setApplicationName("Data Studio");
    QCoreApplication::setApplicationVersion(QString("%1.%2.%3")
                                            .arg(JMain::appVerMajor())
                                            .arg(JMain::appVerMinor())
                                            .arg(JMain::appVerPatch()));
    // settings
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
    const QString appDir = QCoreApplication::applicationDirPath();
#ifdef DEBUG
    QString projectDir = QString(PROJECT_DIR);
    if (!QDir(projectDir).exists()) {
        projectDir = appDir + "/..";
    }
#else
    const QString projectDir = appDir + "/..";
#endif
    settings.setValue("APP_DIR", appDir);
    settings.setValue("THIS_DIR", QDir(appDir + "/..").canonicalPath());
    settings.setValue("CONFIG_DIR", QDir(projectDir + "/config").canonicalPath());
    settings.setValue("WORKSPACE_DIR", QDir(projectDir + "/example").canonicalPath());
    settings.endGroup();

    JMain::initTranslators();

    //QApplication::setApplicationDisplayName(QObject::tr("Data Studio"));
    QApplication::setApplicationDisplayName(QObject::tr("Data Studio")
                                            + ' ' + QCoreApplication::applicationVersion());

    int ret = JMain::execApp(&app);

    return ret;
}
