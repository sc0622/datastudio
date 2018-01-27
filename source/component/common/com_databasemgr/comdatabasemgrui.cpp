#include "precomp.h"
#include "comdatabasemgrui.h"
#include "sourcewidget.h"
#include "databasesettingwidget.h"
#include "xmlsettingwidget.h"

ComDatabaseMgrUi::ComDatabaseMgrUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    JSplitter *splitter = new JSplitter();
    splitter->setHandleWidth(4);
    splitter->setHandleColor(QColor(100, 100, 100, 0));
    splitter->setScales(QList<double>() << 1.5 << 1);
    vertLayoutMain->addWidget(splitter);

    d_sourceWidget = new SourceWidget(this);
    d_stackedWidget = new QStackedWidget(this);
    d_databaseWidget = new DatabaseSettingWidget(attempter, this);
    d_xmlWidget = new XmlSettingWidget(attempter, this);
    d_stackedWidget->addWidget(d_databaseWidget);
    d_stackedWidget->addWidget(d_xmlWidget);
    splitter->addWidget(d_sourceWidget);
    splitter->addWidget(d_stackedWidget);

    //
    connect(d_sourceWidget, SIGNAL(typeChanged(QString)),
            this, SLOT(slotSourceTypeChanged(QString)));
    connect(d_sourceWidget, SIGNAL(sourceChanged(bool &)),
            this, SLOT(slotSourceChanged(bool &)));
    connect(d_databaseWidget, SIGNAL(configChanged()),
        this, SLOT(slotConfigChanged()));
    connect(d_xmlWidget, SIGNAL(configChanged()),
                  this, SLOT(slotConfigChanged()));

    d_sourceWidget->initUi();
}

ComDatabaseMgrUi::~ComDatabaseMgrUi()
{

}

QString ComDatabaseMgrUi::sourceType() const
{
    return d_sourceWidget->sourceType();
}

QString ComDatabaseMgrUi::filePath() const
{
    return d_xmlWidget->xmlFile();
}

void ComDatabaseMgrUi::slotSourceTypeChanged(const QString &text)
{
    d_stackedWidget->setVisible(true);
    if (text == QStringLiteral("文件")) {
        d_xmlWidget->initUi();
        d_stackedWidget->setCurrentWidget(d_xmlWidget);
    } else if (text == "database") {
        d_databaseWidget->initUi();
        d_stackedWidget->setCurrentWidget(d_databaseWidget);
    } else {
        d_stackedWidget->setVisible(false);
    }
}

void ComDatabaseMgrUi::slotSourceChanged(bool &result)
{
    //
    switch (d_stackedWidget->currentIndex()) {
    case 0:
        d_databaseWidget->slotApplyButtonClicked();
        break;
    case 1:
        d_xmlWidget->slotApplyButtonClicked();
        break;
    default:
        break;
    }

    // 查询是否有数据变更
    int dataState = 0;
    d_attempter.notifier().sendMessage("queryNodeFlag@ICDNavigation",
                                       (JWPARAM)&dataState,
                                       (JLPARAM)&QString("dataState"));
    QWidget *widget = d_stackedWidget->currentWidget();
    if (widget == d_databaseWidget) {
        if (!d_databaseWidget->dataValid()) {
            return;
        }
        if (dataState > 0) {
            if (QMessageBox::No == QMessageBox::question(
                        this, QStringLiteral("修改数据源配置"),
                        QStringLiteral("有数据尚未保存，")
                        + QStringLiteral("继续操作将丢失这部分数据！是否继续？"))) {
                return;
            }
        }
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QString type = QString("database");
        // 更新数据库链接信息
        d_attempter.notifier()
            .sendMessage("dataSourceChanged", (JWPARAM)&type);
        QApplication::restoreOverrideCursor();
    } else if (widget == d_xmlWidget) {
        if (!d_xmlWidget->dataValid()) {
            return;
        }
        if (dataState > 0) {
            if (QMessageBox::No == QMessageBox::question(
                        this, QStringLiteral("修改数据源配置"),
                        QStringLiteral("有数据尚未保存，")
                        + QStringLiteral("继续操作将丢失这部分数据！是否继续？"))) {
                return;
            }
        }
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QString file = d_xmlWidget->xmlFile();
        QString type = QStringLiteral("文件");
        // 更新xml文件信息
        d_attempter.notifier().sendMessage("dataSourceChanged",
                                           (JWPARAM)&type, (JLPARAM)&file);
        QApplication::restoreOverrideCursor();
    }
    // 更新界面数据
    d_attempter.notifier().sendMessage("reloadUiData");
    result = true;
}

void ComDatabaseMgrUi::slotConfigChanged()
{
    d_sourceWidget->setApplyState(true);
}
