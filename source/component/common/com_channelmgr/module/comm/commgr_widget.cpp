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
    // �򿪿�ܲ��������ļ�
    QFile file(QString::fromStdString(jframeFacade()->frameGlobalPath()));
    if (!file.exists()) {
        const QString text = QStringLiteral("���ȫ�������ļ�\"%1\"�����ڣ�")
                .arg(file.fileName());
        QMessageBox::warning(jframeLayout()->mainWindow(), QStringLiteral("����"), text);
        return false;   // �ļ�������
    }

    // ���ļ�
    if (!file.open(QFile::ReadOnly)) {
        return false;   // ��ʧ��
    }

    // �����ļ�
    QString errorMsg;
    int errorLine = 0, errorColumn = 0;
    QDomDocument document;
    if (!document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        const QString text =
                QStringLiteral("���ȫ�������ļ�\"%1\"����ʧ�ܣ�\n����������%2\n����λ�ã����кţ�%3���кţ�%4��")
                .arg(file.fileName())
                .arg(errorMsg).arg(errorLine).arg(errorColumn);
        QMessageBox::warning(jframeLayout()->mainWindow(), QStringLiteral("����"), text);
        return false;
    }

    // �ر��ļ�
    file.close();

    // ��ȡ���ڵ�
    QDomElement emRoot = document.documentElement();
    if (emRoot.isNull()) {
        return false;   // ��ȡʧ��
    }

    // ����ָ��ϵͳ�ڵ�
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

    // ���� icdChannel�ڵ�
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
