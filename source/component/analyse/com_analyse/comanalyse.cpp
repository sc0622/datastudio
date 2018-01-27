#include "precomp.h"
#include "comanalyse.h"
#include "comanalyseui.h"
#include "icdcore/icd_table.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComAnalyse(reinterpret_cast<IJAttempter *>(attemper)));
}

ComAnalyse::ComAnalyse(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
{

}

ComAnalyse::~ComAnalyse()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComAnalyse::loadInterface()
{
    bool result = true;

    // 订阅共享消息
    d_attempter->notifier().beginGroup(this)
            .append("treeItemUnloaded", &ComAnalyse::onTreeItemUnloaded)
            .append("unbindItem", &ComAnalyse::onUnbindItem)
            .append("setReplayState", &ComAnalyse::setReplayState)
            .endGroup();

    return result;
}

bool ComAnalyse::updateInterface()
{
    bool result = true;

    //
    if (d_ui) {
        QObject *object = 0;
        // x-axis-sync
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_chart_x_sync@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->setXAxisSync(object->property("checked").toBool());
        }
        // y-label
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_chart_y_label@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->showChartYLabel(object->property("checked").toBool());
        }
        // y-align
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_chart_y_align@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->showChartYAlign(object->property("checked").toBool());
        }
        // sync-track
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_chart_sync_track@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->setSyncTrack(object->property("checked").toBool());
        }
        // y-width
        object = (QObject *)jframeMainWindow()->queryObject(
                    "spin_chart_y_width@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->setYLabelLength(object->property("value").toInt());
        }
        // play
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_replay@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->toggleReplay(object->property("checked").toBool());
        }
        // update-interval
        object = (QObject *)jframeMainWindow()->queryObject(
                    "spin_flush_period@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->setReplayPeriod(object->property("value").toInt());
        }
        // chart column count
        object = (QObject *)jframeMainWindow()->queryObject(
                    "spin_chart_colcnt@group_chart_settings@page_analyse@*");
        if (object) {
            d_ui->setColumnCount(object->property("value").toInt());
        }
        // update-show-chartview
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_chartview@group_window@page_analyse@*");
        if (object) {
            d_ui->setVisible(object->property("checked").toBool());
        }
    }

    return result;
}

void ComAnalyse::releaseInterface()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("treeItemUnloaded")
            .remove("unbindItem")
            .remove("setReplayState")
            .endGroup();

    if (d_ui) {
        d_ui->releaseWork();
    }
}

void* ComAnalyse::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComAnalyse::componentName() const
{
    return QStringLiteral("com_analyse").toStdString();
}

std::string ComAnalyse::componentDesc() const
{
    return QStringLiteral("数据分析系统ICD结构树管理").toStdString();
}

void ComAnalyse::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("resetChart", &ComAnalyse::onResetChart)
            .append("dataSourceChanged", &ComAnalyse::onDataSourceChanged)
            .append("treeItemClick", &ComAnalyse::onTreeItemClicked)
            .append("currentItemChanged", &ComAnalyse::onCurrentItemChanged)
            .append("tableLoaded", &ComAnalyse::onTableLoaded)
            .endGroup();

    //TODO: Add your statements.
}

void ComAnalyse::detach()
{
    //TODO: Add your statements.

    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("resetChart")
            .remove("dataSourceChanged")
            .remove("treeItemClick")
            .remove("currentItemChanged")
            .remove("tableLoaded")
            .endGroup();
}

void *ComAnalyse::createWindow(void *parent, const std::string &objectName)
{

    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (d_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    d_ui = new ComAnalyseUi(*d_attempter);

    //
    d_ui->init();

    return qobject_cast<QWidget *>(d_ui);
}

bool ComAnalyse::commandSink(void *sender, const std::string &domain, const std::string &objectName,
                             const std::string &eventType, void *data)
{
    Q_UNUSED(domain);
    Q_UNUSED(objectName);
    Q_UNUSED(eventType);
    Q_UNUSED(data);

    QObject *objectSender = reinterpret_cast<QObject *>(sender);
    if (!objectSender) {
        return false;
    }

    //
    QString _domain = QString::fromStdString(domain);
    if (_domain.endsWith(QString::fromStdString(componentName()))) {
        //
        if (objectName == "action_chart_x_sync") {
            if (d_ui) {
                d_ui->setXAxisSync(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_chart_y_label") {
            if (d_ui) {
                d_ui->showChartYLabel(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_chart_y_align") {
            if (d_ui) {
                d_ui->showChartYAlign(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_chart_sync_track") {
            if (d_ui) {
                d_ui->setSyncTrack(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_update") {
            if (d_ui) {
                d_ui->updateData();
            }
            return true;
        } else if (objectName == "action_replay") {
            if (d_ui) {
                bool running = *(bool *)data;
                setReplayState(objectSender, running);
                d_ui->toggleReplay(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_chart_clear") {
            if (d_ui) {
                d_ui->clearCharts();
            }
            return true;
        } else if (objectName == "action_chart_colwidth") {
            if (d_ui) {
                d_ui->setColumnWidthSame();
            }
            return true;
        } else if (objectName == "spin_flush_period") {
            if (d_ui) {
                d_ui->setReplayPeriod(*(int *)data);
            }
        } else if (objectName == "spin_chart_colcnt") {
            if (d_ui) {
                d_ui->setColumnCount(*(int *)data);
            }
            return true;
        } else if (objectName == "spin_chart_y_width") {
            if (d_ui) {
                d_ui->setYLabelLength(*(int *)data);
            }
            return true;
        } else if (objectName == "action_tool_data_analyse") {
            QProcess::startDetached("\"" + QApplication::applicationDirPath()
                                    .append("/3rdpart/tools/DataAnalyse.exe\""));
        } else if (objectName == "action_tool_calc") {
            QProcess::startDetached("calc.exe");
        } else if (objectName == "check_show_chartview") {
            if (d_ui) {
                d_ui->setVisible(*(bool *)data);
            }
            return true;
        }
    }

    return false;
}

bool ComAnalyse::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //TODO: Add your statements.

    return false;
}

std::string ComAnalyse::observerId() const
{
    return componentName();
}

JLRESULT ComAnalyse::onResetChart(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    if (!d_ui->resetChart()) {
        return JFrame::ResultError;
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComAnalyse::onDataSourceChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //
    if (d_ui) {
        d_ui->resetChart();
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComAnalyse::onTreeItemClicked(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    if (!item) {
        return JFrame::ResultError;
    }

    //
    if (d_ui) {
        d_ui->onTreeItemClicked(item);
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComAnalyse::onCurrentItemChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    //
    QStandardItem *current = (QStandardItem *)wParam;
    //QStandardItem *previous = (QStandardItem *)lParam;
    if (!current) {
        return JFrame::ResultError;
    }

    //
    if (d_ui) {
        //
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComAnalyse::onTreeItemUnloaded(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    QStandardItem *itemTable = (QStandardItem *)lParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    if (d_ui) {
        d_ui->onTreeItemUnloaded(item, itemTable);
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComAnalyse::onUnbindItem(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    QStandardItem *itemTable = (QStandardItem *)lParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    if (d_ui) {
        d_ui->onUnbindItem(item, itemTable);
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComAnalyse::onTableLoaded(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);

    struct Data { Icd::TablePtr table; } *pData = (Data *)wParam;
    if (!pData) {
        return JFrame::ResultError;
    }

    if (!pData->table) {
        return JFrame::ResultError;
    }

    QString *pFilePath = (QString *)lParam;
    if (!pFilePath) {
        return JFrame::ResultError;
    }

    if (d_ui) {
        d_ui->onTableLoaded(*pFilePath, pData->table);
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComAnalyse::setReplayState(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    bool *state = (bool *)wParam;
    if (!state) {
        return JFrame::ResultError;
    }

    //
    QObject *object = (QObject *)jframeMainWindow()->queryObject(
                "action_replay@group_chart_settings@page_analyse@*");
    if (!object) {
        return JFrame::ResultError;
    }

    setReplayState(object, *state);

    return JFrame::ResultSuccess;
}

void ComAnalyse::setReplayState(QObject *target, bool checked)
{
    if (!target) {
        return;
    }

    bool previousBlock = target->blockSignals(true);

    QPixmap pixmap = QPixmap(QString(":/icd/common/images/")
                             + (checked ? "pause" : "play") + ".png")
            .scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    target->setProperty("icon", QIcon(pixmap));
    target->setProperty("iconText", checked
                        ? QStringLiteral("停止回放") : QStringLiteral("启动回放"));

    target->blockSignals(previousBlock);
}
