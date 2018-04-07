#include "precomp.h"
#include "MonitorChartView.h"

namespace Monitor {

ChartView::ChartView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_chartView = new Icd::ChartView(false, this);
    vertLayoutMain->addWidget(d_chartView);

    jnotify->on("monitor.toolbar.chart.clean", this, [=](JNEvent &){
        d_chartView->clearCharts();
    });
    jnotify->on("monitor.toolbar.chart.columnWidthSame", this, [=](JNEvent &){
        d_chartView->setColumnWidthSame();
    });
    jnotify->on("monitor.toolbar.chart.xAxisSync", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_chartView->setXAxisSync(checked);
        JMain::instance()->setOption("monitor", "option.chart.xAxisSync", checked);
    });
    jnotify->on("monitor.toolbar.chart.showYLabel", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_chartView->showYLabel(checked);
        JMain::instance()->setOption("monitor", "option.chart.showYLabel", checked);
    });
    jnotify->on("monitor.toolbar.chart.showYAlign", this, [=](JNEvent &event){
        const bool align = event.argument().toBool();
        d_chartView->showYAlign(align);
        JMain::instance()->setOption("monitor", "option.chart.showYAlign", align);
    });
    jnotify->on("monitor.toolbar.chart.syncTrack", this, [=](JNEvent &event){
        const bool enabled = event.argument().toBool();
        d_chartView->setSyncTrack(enabled);
        JMain::instance()->setOption("monitor", "option.chart.syncTrack", enabled);
    });
    jnotify->on("monitor.toolbar.chart.columnCount", this, [=](JNEvent &event){
        const int count = event.argument().toInt();
        d_chartView->setColumnCount(count);
        JMain::instance()->setOption("monitor", "option.chart.columnCount", count);
    });
    jnotify->on("monitor.toolbar.chart.yLabelWidth", this, [=](JNEvent &event){
        const int length = event.argument().toInt();
        d_chartView->setYLabelLength(length);
        JMain::instance()->setOption("monitor", "option.chart.yLabelWidth", length);
    });
    jnotify->on("monitor.toolbar.tree.flushToggle", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_chartView->setRunning(checked);
        JMain::instance()->setOption("monitor", "option.chart.flushEnabled", checked);
    });
    jnotify->on("monitor.tree.item.clicked", this, [=](JNEvent &event){
        QStandardItem *item = jVariantFromVoid<QStandardItem>(event.argument());
        if (!item) {
            return;
        }
        const int itemType = item->type();
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        switch (itemType) {
        case Icd::TreeItemTypeRoot:
            break;
        case Icd::TreeItemTypeVehicle:
        case Icd::TreeItemTypeSystem:
        case Icd::TreeItemTypeTable:
        case Icd::TreeItemTypeDataItem:
        case Icd::TreeItemTypeItemTable:
            d_chartView->focusItem(domain);
            break;
        default:
            break;
        }
    });
    jnotify->on("monitor.tree.item.unloaded", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const int itemType = item->type();
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        switch (itemType) {
        case Icd::TreeItemTypeRoot:
            break;
        case Icd::TreeItemTypeVehicle:
        case Icd::TreeItemTypeSystem:
        case Icd::TreeItemTypeTable:
        case Icd::TreeItemTypeDataItem:
        case Icd::TreeItemTypeItemTable:
            d_chartView->removeItem(domain);
            break;
        default:
            break;
        }
    });
    jnotify->on("monitor.tree.channel.unbound", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        d_chartView->removeItem(domain);
    });
    jnotify->on("monitor.tree.channel.changed", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const QString channelId = args.at(1).toString();
        const Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(channelId.toStdString());
        if (!worker) {
            return;
        }
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        d_chartView->changeWorker(domain, worker);
    });
    jnotify->on("monitor.tree.item.unbind", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        d_chartView->removeItem(domain);
        item->setData(QVariant(), Icd::TreeBoundRole);
    });
    //
    auto setChartTheme = [=](const QString &theme){
        if (theme == "none") {
            d_chartView->setChartTheme(JChart::ChartThemeLight);
        } else if (theme == "blue") {
            d_chartView->setChartTheme(JChart::ChartThemeLight);
        } else if (theme == "dark") {
            d_chartView->setChartTheme(JChart::ChartThemeDark);
        } else if (theme == "light") {
            d_chartView->setChartTheme(JChart::ChartThemeLight);
        }
    };
    jnotify->on("global.theme.changed", this, [=](JNEvent &event){
        const QString theme = event.argument().toString();
        setChartTheme(theme);
    });
    setChartTheme(JMain::instance()->theme());
}

ChartView::~ChartView()
{
    d_chartView->setRunning(false);
}

bool ChartView::init()
{
    bool result = true;

    Json::Value option = JMain::instance()->option("monitor", "option.chart");
    if (option.isNull()) {
        return false;
    }
    // columnCount
    if (option.isMember("columnCount")) {
        d_chartView->setColumnCount(option["columnCount"].asInt());
    }
    // yLabelWidth
    if (option.isMember("yLabelWidth")) {
        d_chartView->setYLabelLength(option["yLabelWidth"].asInt());
    }

    return result;
}

void ChartView::setXAxisSync(bool checked)
{
    d_chartView->setXAxisSync(checked);
}

void ChartView::showYLabel(bool checked)
{
    d_chartView->showYLabel(checked);
}

void ChartView::showYAlign(bool checked)
{
    d_chartView->showYAlign(checked);
}

void ChartView::setSyncTrack(bool checked)
{
    d_chartView->setSyncTrack(checked);
}

void ChartView::setFlushEnabled(bool checked)
{
    d_chartView->setRunning(checked);
}

void ChartView::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.save();
    QFont font = this->font();
    font.setPointSize(24);
    painter.setFont(font);
    painter.setPen(QColor(128, 128, 80, 150));
    QTextOption textOption(Qt::AlignCenter);
    painter.drawText(rect(), tr("Drag and drog left tree node to here"), textOption);
    painter.restore();
}

}
