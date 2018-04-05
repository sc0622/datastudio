﻿#include "precomp.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QFontDatabase>
#include "Common/Logging.h"

int main(int argc, char **argv)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#if QT_VERSION >= 0x050600
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);

    QApplication app(argc, argv);

    Logging::instance()->init();
    JMain::initTranslators();

    QApplication::setFont(QFont("Microsoft Yahei", 9));
    QApplication::setOrganizationName("Smartsoft");
    QApplication::setApplicationName("Data Studio");
    QApplication::setApplicationDisplayName(QObject::tr("Data Studio"));
    QApplication::setApplicationVersion(QString("%1.%2.%3")
                                        .arg(JMain::appVerMajor())
                                        .arg(JMain::appVerMinor())
                                        .arg(JMain::appVerPatch()));
    QApplication::setWindowIcon(QIcon(":/datastudio/image/app.png"));

    int ret = JMain::execApp(&app);

    return ret;
}
