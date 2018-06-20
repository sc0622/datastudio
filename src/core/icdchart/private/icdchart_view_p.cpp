#include "precomp.h"
#include "icdchart_view_p.h"
#include "icdcore/icdcore_inc.h"
#include "jchart/jbitchart.h"
#include "jchart/jnumericchart.h"
#include "jchart/jnumericseries.h"
#include "jchart/jchart_view.h"

namespace Icd {

// class ChartViewPrivate

ChartViewPrivate::ChartViewPrivate(ChartView *q)
    : QObject(q)
    , J_QPTR(q)
    , running(false)
    , timerInterval(50)     // unit: ms
    , xAxisSync(false)
    , showYLabel(true)
    , showYAlign(false)
    , yLabelLength(10)
    , syncTrack(true)
    , chartTheme(JChart::ChartThemeDark)
{

}

void ChartViewPrivate::init(bool styled)
{
    Q_Q(ChartView);

    QHBoxLayout *horiLayoutMain = new QHBoxLayout(q);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    chartView = new JChart::View(styled, q);
    q->setAcceptDrops(true);
    horiLayoutMain->addWidget(chartView);

    //
    QObject::connect(chartView, &JChart::View::trackerChanged, this,
            [=](JChart::Chart *chart, const QPointF &pos, bool visible){
        onTrackerChanged(chart, pos, visible);
    }, Qt::QueuedConnection);
    QObject::connect(chartView, &JChart::View::trackerMarked, this,
            [=](JChart::Chart *chart, const QPointF &pos){
        onTrackerMarked(chart, pos);
    }, Qt::QueuedConnection);

    //
    QTimer *timerTemp = new QTimer(q);
    timerTemp->setSingleShot(true);
    QObject::connect(timerTemp, &QTimer::timeout, q, [=](){
        chartView->setColumnWidthSame();
        timerTemp->deleteLater();
    });
    timerTemp->start(200);
}

bool ChartViewPrivate::addDataItem(const WorkerPtr &worker, const Icd::ItemPtr &dataItem,
                                   QStandardItem *item, JChart::Chart *chart)
{
    if (!worker || !dataItem || !item) {
        return false;
    }

    //
    switch (dataItem->type()) {
    case Icd::ItemComplex:
    {
        //
        const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(dataItem);
        if (!complex) {
            return false;
        }

        //
        if (!addDataItem(worker, complex->table(), item, chart)) {
            return false;
        }

        return true;
    }
    case Icd::ItemFrame:
    {
        //
        const Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(dataItem);
        if (!frame) {
            return false;
        }

        //
        if (!addDataItem(worker, frame, item, chart)) {
            return false;
        }

        return true;
    }
    case Icd::ItemArray:
        return false;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        chart = nullptr;
        break;
    default:
        break;
    }

    //
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    //
    if (!chart) {
        // create chart
        chart = createChart(dataItem);
        if (!chart) {
            Q_ASSERT(false);    // logic error
            return false;       // create failure or not supported
        }
        chart->setChartTheme(JChart::ChartTheme(chartTheme));
        chart->setAxisVisible(JChart::yLeft, showYLabel);
        chart->setAxisAlign(JChart::yLeft, showYAlign);
        chart->setAxisLabelLength(JChart::yLeft, yLabelLength);
        chart->setXScaleDrawType(JChart::ScaleDrawTime);
        chart->setShiftType(JChart::ShiftByTime);
        chart->togglePlay(true);
        chartView->appendChart(chart);
        //
        QObject::connect(chart, &JChart::Chart::seriesRemoved, this, [=](int index){
            Q_UNUSED(index);
            if (chart->seriesCount() > 1) {
                chart->setTitle(QString());
                chart->setLegendVisible(true);
            } else {
                chart->setLegendVisible(false);
                if (chart->seriesCount() == 1) {
                    JChart::AbstractSeries *series = chart->seriesAt(0);
                    if (series) {
                        chart->setTitle(series->title());
                    } else {
                        chart->setTitle(QString());
                    }
                }
            }
        });
        //
        ChartData *chartData = new ChartData(chart);
        chartData->timer = new QTimer(chart);
        chartData->timer->setInterval(timerInterval);
        //chartData->timer->setTimerType(Qt::PreciseTimer);
        QObject::connect(chartData->timer, &QTimer::timeout, this, [=](){
            updateChart(chart);
        });
        chart->setUserData(chartData);
        if (chart->chartType() == JChart::ChartTypeBitMap) {
            chartData->item = item;
            chart->setIdentity(domain);
            setChartTitle(chart, 0, item);
            //
            SeriesData *seriesData = new SeriesData();
            seriesData->worker = worker;
            seriesData->dataItem = dataItem;
            seriesData->item = item;
            chartData->bitmapData = seriesData;
        } else {
            chart->setIdentity(QUuid::createUuid().toString());
        }

        //
        if (running) {
            chartData->timer->start();
        }
    }

    //
    if (chart->chartType() != JChart::ChartTypeBitMap) {
        const QString path = item->data(Icd::TreeItemPathRole).toString();
        JChart::AbstractSeries *series = chart->addSeries(domain, path.section('@', 0, 1));
        if (chart->seriesCount() > 1) {
            chart->setTitle("");
            chart->setLegendVisible(true);
        } else {
            setChartTitle(chart, 0, item);
        }

        //
        SeriesData *seriesData = new SeriesData();
        seriesData->worker = worker;
        seriesData->dataItem = dataItem;
        seriesData->item = item;
        series->setUserData(seriesData);
    }

    //
    item->setData(true, Icd::TreeBoundRole);

    return true;
}

bool ChartViewPrivate::addDataItem(const WorkerPtr &worker, const TablePtr &table,
                                   QStandardItem *item, JChart::Chart *chart)
{
    if (!worker || !table || !item) {
        return false;
    }

    int i = 0;
    const int rowCount = item->rowCount();
    const Icd::ItemPtrArray &dataItems = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend() && i < rowCount; ++citer, ++i) {
        const Icd::ItemPtr &dataItem = *citer;
        QStandardItem *itemData = item->child(i);
        if (!itemData) {
            continue;
        }
        //
        if (!addDataItem(worker, dataItem, itemData, chart)) {
            continue;
        }
    }

    //
    if (rowCount > 0) {
        item->setData(true, Icd::TreeBoundRole);
    }

    return true;
}

bool ChartViewPrivate::addDataItem(const WorkerPtr &worker, const Icd::FrameItemPtr &frame,
                                   QStandardItem *item, JChart::Chart *chart)
{
    if (!worker || !frame || !item) {
        return false;
    }

    int i = 0;
    const int rowCount = item->rowCount();
    const Icd::TablePtrMap &tables = frame->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend() && i < rowCount; ++citer, ++i) {
        const Icd::TablePtr &table = citer->second;
        QStandardItem *itemTable = item->child(i);
        if (!itemTable) {
            continue;
        }
        //
        if (!addDataItem(worker, table, itemTable, chart)) {
            continue;
        }
    }

    //
    if (rowCount > 0) {
        item->setData(true, Icd::TreeBoundRole);
    }

    return true;
}

bool ChartViewPrivate::addDataItem(const QString &domain, const WorkerPtr &worker,
                                   const Icd::ObjectPtr &dataObject, QStandardItem *item,
                                   JChart::Chart *chart)
{
    Q_UNUSED(domain);
    if (!worker || !dataObject || !item) {
        return false;
    }

    //
    switch (dataObject->objectType()) {
    case Icd::ObjectTable:
    {
        //
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table, Icd::Object>(dataObject);
        if (!table) {
            break;
        }

        //
        if (!addDataItem(worker, table, item, chart)) {
            break;
        }

        break;
    }
    case Icd::ObjectItem:
    {
        //
        const Icd::ItemPtr dataItem = JHandlePtrCast<Icd::Item, Icd::Object>(dataObject);
        if (!dataItem) {
            break;
        }

        //
        if (!addDataItem(worker, dataItem, item, chart)) {
            break;
        }

        break;
    }
    default:
        break;
    }

    return true;
}

JChart::Chart *ChartViewPrivate::createChart(const Icd::ItemPtr &dataItem)
{
    Q_Q(ChartView);
    if (!dataItem) {
        return false;
    }

    //
    switch (dataItem->type()) {
    case Icd::ItemHead:
    case Icd::ItemCounter:
    case Icd::ItemCheck:
    case Icd::ItemFrameCode:
    case Icd::ItemNumeric:
    case Icd::ItemBitValue:
    {
        // create
        JChart::NumericChart *chart = new JChart::NumericChart(q);
        chart->setChartTheme(JChart::ChartThemeDark);
        // specs
        return chart;
    }
    case Icd::ItemBitMap:
    {
        // convert
        Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem);
        if (itemBit == nullptr) {
            Q_ASSERT(false);    //
            return nullptr;
        }
        // create
        JChart::BitChart *chart = new JChart::BitChart(q);
        chart->setChartTheme(JChart::ChartThemeDark);
        chart->setLegendVisible(showYAlign);
        chart->setBitsRange(itemBit->bitStart(), itemBit->bitStart() + itemBit->bitCount() - 1);
        const std::unordered_map<icd_uint64, std::string> &specs = itemBit->specs();
        for (std::unordered_map<icd_uint64, std::string>::const_iterator citer = specs.cbegin();
             citer != specs.cend(); ++citer) {
            JChart::AbstractSeries *series = chart->seriesAt(citer->first);
            if (series) {
                series->setTitle(QString::fromStdString(Icd::BitItem::nameOf(citer->second)));
            }
        }
        return chart;
    }
    default:
        break;
    }

    //
    return nullptr;
}

bool ChartViewPrivate::findChart(JChart::Chart *chart)
{
    if (!chart) {
        return false;
    }

    const int rowCount = chartView->rowCount();
    const int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            if (chartView->chartAt(row, column) == chart) {
                return true;
            }
        }
    }

    return false;
}

JChart::AbstractSeries *ChartViewPrivate::findSeries(const QString &domain) const
{
    const int rowCount = chartView->rowCount();
    const int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *chart = chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            int seriesCount = chart->seriesCount();
            for (int i = 0; i < seriesCount; ++i) {
                JChart::AbstractSeries *series = chart->seriesAt(i);
                if (series->identity() == domain) {
                    return series;
                }
            }
        }
    }

    return nullptr;
}

bool ChartViewPrivate::isRunning() const
{
    return running;
}

void ChartViewPrivate::setRunning(bool value)
{
    Q_Q(ChartView);
    if (value != isRunning()) {
        running = value;
        const int rowCount = chartView->rowCount();
        const int columnCount = chartView->columnCount();
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                JChart::Chart *chart = chartView->chartAt(row, column);
                if (!chart) {
                    continue;
                }
                ChartData *chartData = dynamic_cast<ChartData *>(chart->userData());
                if (!chartData) {
                    continue;
                }
                QTimer *timer = chartData->timer;
                if (!timer) {
                    continue;
                }
                if (value) {
                    if (!timer->isActive()) {
                        timer->start();
                    }
                } else {
                    if (timer->isActive()) {
                        timer->stop();
                        QCoreApplication::removePostedEvents(chart, QEvent::Timer);
                    }
                }

                chart->togglePlay(value);
            }

            emit q->runningChanged(value);
        }
    }
}

void ChartViewPrivate::setChartTitle(JChart::Chart *chart, int sectionOffset,
                                     QStandardItem *item)
{
    const QString path = item->data(Icd::TreeItemPathRole).toString();
    chart->setTitle(path.section('@', sectionOffset, sectionOffset)
                    + "<font size=2 style='font-style:italic;'>@"
                    + path.section('@', sectionOffset + 1) + "</font>");
}

void ChartViewPrivate::updateXAxisSync()
{
    if (!xAxisSync) {
        return;
    }

    // reset chartview
    updateScale();
}

void ChartViewPrivate::updateChart(JChart::Chart *chart)
{
    if (!chart) {
        return;
    }

    const int seriesCount = chart->seriesCount();
    if (!chart->isPlay() || seriesCount == 0) {
        return;
    }
    //
    if (chart->chartType() == JChart::ChartTypeBitMap) {
        ChartData *chartData = dynamic_cast<ChartData *>(chart->userData());
        if (!chartData || !chartData->bitmapData) {
            return;
        }

        JChart::BitChart *bitChart = qobject_cast<JChart::BitChart *>(chart);
        if (!bitChart) {
            return;
        }

        const Icd::BitItemPtr bitItem =
                JHandlePtrCast<Icd::BitItem, Icd::Item>(chartData->bitmapData->dataItem);
        if (!bitItem) {
            return;
        }

        bitChart->shiftSample(QPointF(QDateTime::currentMSecsSinceEpoch(),
                                      (((quint32)bitItem->data()) << bitItem->bitStart())));
    } else {
        for (int i = 0; i < seriesCount; ++i) {
            JChart::AbstractSeries *series = chart->seriesAt(i);
            SeriesData *seriesData = dynamic_cast<SeriesData *>(series->userData());
            if (!seriesData || !seriesData->dataItem) {
                continue;
            }
            switch (seriesData->dataItem->type()) {
            case Icd::ItemHead:
            case Icd::ItemCounter:
            case Icd::ItemCheck:
            case Icd::ItemFrameCode:
            case Icd::ItemNumeric:
            case Icd::ItemBitValue:
                series->shiftSample(QPointF(QDateTime::currentMSecsSinceEpoch(),
                                            seriesData->dataItem->data()));
                break;
            default:
                break;
            }
        }

        chart->updateScale(JChart::xBottom);
    }
}

void ChartViewPrivate::onTrackerChanged(JChart::Chart *chart, const QPointF &pos, bool visible)
{
    if (!chart || !syncTrack) {
        return;
    }

    const int rowCount = chartView->rowCount();
    const int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *_chart = chartView->chartAt(row, column);
            if (!_chart) {
                continue;
            }
            if (_chart != chart) {
                _chart->setTrackerPos(pos, visible);
            }
        }
    }
}

void ChartViewPrivate::onTrackerMarked(JChart::Chart *chart, const QPointF &pos)
{
    if (!chart || !syncTrack) {
        return;
    }

    const int rowCount = chartView->rowCount();
    const int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *_chart = chartView->chartAt(row, column);
            if (!_chart) {
                continue;
            }
            if (_chart != chart) {
                _chart->markTrackerPos(pos);
            }
        }
    }
}

void ChartViewPrivate::updateScale()
{
    const int rowCount = chartView->rowCount();
    const int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *chart = chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            chart->updateScale(JChart::xBottom);
            chart->updateScale(JChart::yLeft);
        }
    }
}

} // end of namespace Icd
