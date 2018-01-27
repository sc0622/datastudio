#include "precomp.h"
#include "icdchart_view.h"
#include "icdcore/icd_table.h"
#include "icdworker/icdworker.h"
#include "private/icdchart_view_p.h"
#include "icdwidget/icdwidget_global.h"
#include "jchart/jchart_view.h"

namespace Icd {

// class ICDChartView

ChartView::ChartView(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ChartViewPrivate(this))
{
    Q_D(ChartView);
    d->init();
}

ChartView::~ChartView()
{
    Q_D(ChartView);
    delete d;
}

JChart::View *ChartView::chartView() const
{
    Q_D(const ChartView);
    return d->chartView;
}

void ChartView::changeWorker(const QString &domain, const Icd::WorkerPtr &worker)
{
    Q_D(ChartView);
    int rowCount = d->chartView->rowCount();
    int columnCount = d->chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *chart = d->chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            auto updateData = [=](SeriesData *data, const QString &domain){
                if (!data) {
                    return;
                }
                data->worker = worker;
                if (worker) {
                    const Icd::TablePtr table = worker->workerRecv()->table();
                    if (!table) {
                        data->dataItem = Icd::ItemPtr(nullptr);
                        return;
                    }
                    data->dataItem = JHandlePtrCast<Icd::Item, Icd::Object>(
                                table->itemByDomain(domain.section('/', 3).toStdString()));
                } else {
                    data->dataItem = Icd::ItemPtr(nullptr);
                }
            };

            if (chart->chartType() == JChart::ChartTypeBitMap) {
                const QString identity = chart->identity();
                if (identity.startsWith(domain)) {
                    ChartData *chartData = dynamic_cast<ChartData *>(chart->userData());
                    if (!chartData || !chartData->bitmapData) {
                        continue;
                    }
                    updateData(chartData->bitmapData, identity);
                }
            } else {
                int seriesCount = chart->seriesCount();
                for (int i = 0; i < seriesCount; ++i) {
                    JChart::AbstractSeries *series = chart->seriesAt(i);
                    const QString identity = series->identity();
                    if (identity.startsWith(domain)) {
                        SeriesData *seriesData = dynamic_cast<SeriesData *>(series->userData());
                        updateData(seriesData, identity);
                    }
                }
            }
        }
    }
}

void ChartView::focusItem(const QString &domain)
{
    Q_D(ChartView);
    int rowCount = d->chartView->rowCount();
    int columnCount = d->chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *chart = d->chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            if (chart->chartType() == JChart::ChartTypeBitMap) {
                if (chart->identity() == domain) {
                    d->chartView->focusChart(row, column);
                    return;
                }
            } else {
                int seriesCount = chart->seriesCount();
                for (int i = 0; i < seriesCount; ++i) {
                    JChart::AbstractSeries *series = chart->seriesAt(i);
                    if (series->identity() == domain) {
                        d->chartView->focusChart(row, column);
                        return;
                    }
                }
            }
        }
    }
}

void ChartView::removeItem(const QString &domain)
{
    Q_D(ChartView);
    int rowCount = d->chartView->rowCount();
    int columnCount = d->chartView->columnCount();
    for (int row = rowCount - 1; row >= 0; --row) {
        for (int column = columnCount - 1; column >= 0; --column) {
            JChart::Chart *chart = d->chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            if (chart->chartType() == JChart::ChartTypeBitMap) {
                if (chart->identity().startsWith(domain)) {
                    d->chartView->removeChart(row, column);
                }
            } else {
                bool modify = false;
                int seriesCount = chart->seriesCount();
                for (int i = seriesCount - 1; i >= 0; --i) {
                    JChart::AbstractSeries *series = chart->seriesAt(i);
                    if (series->identity().startsWith(domain)) {
                        chart->removeSeries(i);
                        modify = true;
                    }
                }

                if (modify) {
                    seriesCount = chart->seriesCount();
                    if (seriesCount == 0) {
                        d->chartView->removeChart(row, column);
                    } else if (seriesCount == 1) {
                        chart->setLegendVisible(false);
                        ChartData *chartData = dynamic_cast<ChartData *>(chart->userData());
                        if (chartData && chartData->bitmapData) {
                            d->setChartTitle(chart, 0, chartData->bitmapData->item);
                        }
                    }
                }
            }
        }
    }
}

void ChartView::removeChart(const QString &domain)
{
    Q_D(ChartView);
    int rowCount = d->chartView->rowCount();
    int columnCount = d->chartView->columnCount();
    for (int row = rowCount - 1; row >= 0; --row) {
        for (int column = columnCount - 1; column >= 0; --column) {
            JChart::Chart *chart = d->chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            if (chart->identity().startsWith(domain)) {
                chart->disconnect();
                d->chartView->removeChart(row, column);
            }
        }
    }
}

void ChartView::clearCharts()
{
    Q_D(ChartView);
    d->chartView->clearChart();
}

bool ChartView::xAxisSync() const
{
    Q_D(const ChartView);
    return d->xAxisSync;
}

bool ChartView::isRunning() const
{
    Q_D(const ChartView);
    return d->isRunning();
}

void ChartView::showYLabel(bool checked)
{
    Q_D(ChartView);
    if (checked != d->showYLabel) {
        d->showYLabel = checked;
        int rowCount = d->chartView->rowCount();
        int columnCount = d->chartView->columnCount();
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                JChart::Chart *chart = d->chartView->chartAt(row, column);
                if (!chart) {
                    continue;
                }
                chart->setAxisVisible(JChart::yLeft, checked);
            }
        }
    }
}

void ChartView::showYAlign(bool align)
{
    Q_D(ChartView);
    if (align != d->showYAlign) {
        d->showYAlign = align;
        int rowCount = d->chartView->rowCount();
        int columnCount = d->chartView->columnCount();
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                JChart::Chart *chart = d->chartView->chartAt(row, column);
                if (!chart) {
                    continue;
                }
                chart->setAxisAlign(JChart::yLeft, align);
                if (chart->chartType() == JChart::ChartTypeBitMap) {
                    chart->setLegendVisible(align);
                }
            }
        }
    }
}

void ChartView::setYLabelLength(int length)
{
    Q_D(ChartView);
    if (length != d->yLabelLength) {
        d->yLabelLength = length;
        int rowCount = d->chartView->rowCount();
        int columnCount = d->chartView->columnCount();
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                JChart::Chart *chart = d->chartView->chartAt(row, column);
                if (!chart) {
                    continue;
                }
                chart->setAxisLabelLength(JChart::yLeft, length);
            }
        }
    }
}

void ChartView::setColumnCount(int count)
{
    Q_D(ChartView);
    if (d->chartView->columnCount() != count) {
        clearCharts();
        d->chartView->setColumnCount(count);
        d->chartView->setColumnWidthSame();
    }
}

void ChartView::setColumnWidthSame()
{
    Q_D(ChartView);
    d->chartView->setColumnWidthSame();
}

void ChartView::setSyncTrack(bool enabled)
{
    Q_D(ChartView);
    if (enabled != d->syncTrack) {
        d->syncTrack = enabled;
        //
        if (!enabled) {
            JChart::Chart *chart = nullptr;
            if (d->chartView->chartCount() != 0) {
                chart = d->chartView->model()->chartAt(QCursor::pos());
            }
            //
            int rowCount = d->chartView->rowCount();
            int columnCount = d->chartView->columnCount();
            for (int row = 0; row < rowCount; ++row) {
                for (int column = 0; column < columnCount; ++column) {
                    JChart::Chart *_chart = d->chartView->chartAt(row, column);
                    if (!_chart) {
                        continue;
                    }
                    if (chart && _chart == chart) {
                        continue;
                    }
                    _chart->setTrackerPos(QPointF(-1, -1), false);
                }
            }
        }
    }
}

void ChartView::setXAxisSync(bool value)
{
    Q_D(ChartView);
    if (value != d->xAxisSync) {
        d->xAxisSync = value;
        d->updateXAxisSync();
        emit xAxisSyncChanged(value);
    }
}

void ChartView::setRunning(bool value)
{
    Q_D(ChartView);
    d->setRunning(value);
}

void ChartView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/monitor")) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ChartView::dropEvent(QDropEvent *event)
{
    //
    if (!event->mimeData()->hasFormat("icd/table-drag/monitor")) {
        return;
    }

    //
    QStandardItem *itemTable = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("itemTable").value<void *>());
    if (!itemTable) {
        return;
    }

    //
    QStandardItem *item = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("item").value<void *>());
    if (!item) {
        return;
    }

    //
    JAutoCursor autoCursor(Qt::BusyCursor);

    //
    Q_D(ChartView);
    JChart::Chart *chart = nullptr;
    if (d->chartView->chartCount() != 0) {
        Qt::KeyboardModifiers keys = event->keyboardModifiers();
        switch (keys) {
        case Qt::ControlModifier:
        case Qt::ShiftModifier:
            chart = d->chartView->model()->chartAt(event->pos());
            if (chart && chart->chartType() == JChart::ChartTypeBitMap) {
                chart = nullptr;
            }
            break;
        default:
            break;
        }
    }

    //
    QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        //
        bool result = false;
        if (!QMetaObject::invokeMethod(event->source(), "bindChannel", Qt::DirectConnection,
                                       Q_RETURN_ARG(bool, result),
                                       Q_ARG(QStandardItem *, itemTable))) {
            return;
        }
        if (!result) {
            return;
        }
    }

    //
    varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        return;     //
    }

    //
    const QString channelId = varChannelId.toString();
    if (channelId.isEmpty()) {
        return;
    }

    // get worker object
    Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (worker == 0) {
        return;
    }

    //
    const Icd::TablePtr tableRecv = worker->workerRecv()->table();

    //
    if (!tableRecv) {
        qWarning(QStringLiteral("工作组没有绑定接收端协议表 [通道：%1]！")
                 .arg(channelId).toLocal8Bit());
        return;
    }

    //
    const QString domain = event->mimeData()->property("domain").toString();

    //
    Icd::ObjectPtr dataObject = Icd::ObjectPtr(0);
    switch (item->type()) {
    case Icd::TreeItemTypeTable:
    {
        dataObject = tableRecv;
        break;
    }
    case Icd::TreeItemTypeDataItem:
    {
        // get dataItem object
        const Icd::ObjectPtr dataItem = tableRecv->itemByDomain(domain.section('/', 3).toStdString());
        if (!dataItem) {
            break;
        }
        //
        dataObject = dataItem;
        break;
    }
    case Icd::TreeItemTypeItemTable:
    {
        // get table object
        const Icd::TablePtr table = tableRecv->tableByDomain(domain.section('/', 3).toStdString());
        if (!table) {
            break;
        }
        //
        dataObject = table;
        break;
    }
    default:
        break;
    }

    //
    if (!dataObject) {
        return;
    }

    //
    QApplication::processEvents(QEventLoop::EventLoopExec);

    JAutoCursor cursor(Qt::BusyCursor);

    //
    if (!d->addDataItem(domain, worker, dataObject, item, chart)) {
        return;
    }
}

} // end of namespace Icd
