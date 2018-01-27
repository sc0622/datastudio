#include "precomp.h"
#include "commgr_widget.h"
#include "private/commgr_widget_p.h"
#include "icdwidget/channel_widget.h"

// class ComMgrWidget

ComMgrWidget::ComMgrWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ComMgrWidgetPrivate(this))
{
    Q_D(ComMgrWidget);
    d->init();
}

ComMgrWidget::~ComMgrWidget()
{
    Q_D(ComMgrWidget);
    delete d;
}

void ComMgrWidget::updateUi()
{
    Q_D(ComMgrWidget);

    if (!loadIcdChannelConfig()) {
        return;   // load failure
    }

    //
    d->updateUi();
}

bool ComMgrWidget::loadIcdChannelConfig()
{
    // 打开框架布局配置文件
    QFile file(QString::fromStdString(jframeFacade()->frameGlobalPath()));
    if (!file.exists()) {
        const QString text = QStringLiteral("框架全局配置文件\"%1\"不存在！")
                .arg(file.fileName());
        QMessageBox::warning(jframeLayout()->mainWindow(), QStringLiteral("警告"), text);
        return false;   // 文件不存在
    }

    // 打开文件
    if (!file.open(QFile::ReadOnly)) {
        return false;   // 打开失败
    }

    // 解析文件
    QString errorMsg;
    int errorLine = 0, errorColumn = 0;
    QDomDocument document;
    if (!document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        const QString text =
                QStringLiteral("框架全局配置文件\"%1\"解析失败！\n错误描述：%2\n错误位置：（行号：%3，列号：%4）")
                .arg(file.fileName())
                .arg(errorMsg).arg(errorLine).arg(errorColumn);
        QMessageBox::warning(jframeLayout()->mainWindow(), QStringLiteral("警告"), text);
        return false;
    }

    // 关闭文件
    file.close();

    // 获取根节点
    QDomElement emRoot = document.documentElement();
    if (emRoot.isNull()) {
        return false;   // 获取失败
    }

    // 查找指定系统节点
    const QString appName = QString::fromStdString(jframeFacade()->apppName());
    QDomElement emApp;
    for (emApp = emRoot.firstChildElement("app");
         !emApp.isNull();
         emApp = emApp.nextSiblingElement("app")) {
        if (emApp.hasAttribute("name")) {
            if (emApp.attribute("name") == appName) {
                break;
            }
        }
    }

    //
    if (emApp.isNull()) {
        return false;
    }

    // 查找 icdChannel节点
    QDomElement emIcdChannel = emApp.firstChildElement("icdChannel");
    if (emIcdChannel.isNull()) {
        return false;
    }

    //
    if (!emIcdChannel.hasAttribute("filePath")) {
        return false;
    }

    //
    const std::string filePath = jframeFacade()->parsePath(
                emIcdChannel.attribute("filePath").toStdString());
    if (!Icd::WorkerPool::getInstance()->loadConfig(filePath)) {
        return false;
    }

    return true;
}
