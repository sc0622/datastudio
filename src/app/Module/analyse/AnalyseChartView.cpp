#include "precomp.h"
#include "AnalyseChartView.h"

namespace Analyse {

ChartView::ChartView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_chartView = new Icd::ChartFileView(false, this);
    vertLayoutMain->addWidget(d_chartView);

    jnotify->on("analyse.toolbar.chart.update", this, [=](JNEvent &){
        d_chartView->updateData();
    });
    jnotify->on("analyse.toolbar.chart.clean", this, [=](JNEvent &){
        d_chartView->clearCharts();
    });
    jnotify->on("analyse.toolbar.chart.columnWidthSame", this, [=](JNEvent &){
        d_chartView->setColumnWidthSame();
    });
    jnotify->on("analyse.toolbar.chart.xAxisSync", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_chartView->setXAxisSync(checked);
        JMain::instance()->setOption("analyse", "option.chart.xAxisSync", checked);
    });
    jnotify->on("analyse.toolbar.chart.showYLabel", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_chartView->showYLabel(checked);
        JMain::instance()->setOption("analyse", "option.chart.showYLabel", checked);
    });
    jnotify->on("analyse.toolbar.chart.showYAlign", this, [=](JNEvent &event){
        const bool align = event.argument().toBool();
        d_chartView->showYAlign(align);
        JMain::instance()->setOption("analyse", "option.chart.showYAlign", align);
    });
    jnotify->on("analyse.toolbar.chart.syncTrack", this, [=](JNEvent &event){
        const bool enabled = event.argument().toBool();
        d_chartView->setSyncTrack(enabled);
        JMain::instance()->setOption("analyse", "option.chart.syncTrack", enabled);
    });
    jnotify->on("analyse.toolbar.chart.columnCount", this, [=](JNEvent &event){
        const int count = event.argument().toInt();
        d_chartView->setColumnCount(count);
        JMain::instance()->setOption("analyse", "option.chart.columnCount", count);
    });
    jnotify->on("analyse.toolbar.chart.yLabelWidth", this, [=](JNEvent &event){
        const int length = event.argument().toInt();
        d_chartView->setYLabelLength(length);
        JMain::instance()->setOption("analyse", "option.chart.yLabelWidth", length);
    });
    jnotify->on("analyse.toolbar.chart.flushToggle", this, [=](JNEvent &event){
        const bool running = event.argument().toBool();
        d_chartView->toggleReplay(running);
    });
    jnotify->on("analyse.tree.item.clicked", this, [=](JNEvent &event){
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
    jnotify->on("analyse.tree.item.unloaded", this, [=](JNEvent &event){
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
    jnotify->on("analyse.tree.item.unbind", this, [=](JNEvent &event){
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
    jnotify->on("analyse.tableLoaded", this, [=](JNEvent &event){
        QVariantList args = event.argument().toList();
        if (args.count() < 2) {
            return;
        }
        const QString filePath = args.at(0).toString();
        if (filePath.isEmpty()) {
            return;
        }
        struct Data { Icd::TablePtr table; } *data = jVariantFromVoid<struct Data>(args.at(1));
        if (!data || !data->table) {
            return;
        }

        d_chartView->addTable(filePath, data->table);
    });
}

ChartView::~ChartView()
{

}

bool ChartView::init()
{
    bool result = true;

    Json::Value option = JMain::instance()->option("analyse", "option.chart");
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

void ChartView::setrReplayEnabled(bool checked)
{
    d_chartView->toggleReplay(checked);
}

void ChartView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    QFont font = this->font();
    font.setPointSize(24);
    painter.setFont(font);
    painter.setPen(QColor(64, 64, 64, 100));
    QTextOption textOption(Qt::AlignCenter);
    painter.drawText(rect(), tr("Drag and drog left tree node to here"), textOption);
    painter.restore();

    QWidget::paintEvent(event);
}

}
