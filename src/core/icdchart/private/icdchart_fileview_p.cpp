#include "precomp.h"
#include "icdchart_fileview_p.h"
#include "jchart/jchart_inc.h"
#include "jchart/jchart.h"
#include "icdcore/icdcore_inc.h"
#include "qwt/qwt_plot.h"
#include "qwt/qwt_scale_map.h"

namespace Icd {

ChartFileViewPrivate::ChartFileViewPrivate(ChartFileView *q)
    : QObject(q)
    , J_QPTR(q)
    , xAxisSync(false)
    , showYLabel(true)
    , showYAlign(false)
    , yLabelLength(10)
    , timerReplay(nullptr)
    , syncTrack(true)
    , chartTheme(JChart::ChartThemeLight)
{

}

ChartFileViewPrivate::~ChartFileViewPrivate()
{
    //
}

void ChartFileViewPrivate::init(bool styled)
{
    Q_Q(ChartFileView);

    timerReplay = new QTimer(q);
    timerReplay->setInterval(20);

    QHBoxLayout *horiLayoutMain = new QHBoxLayout(q);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    chartView = new JChart::View(styled, q);
    q->setAcceptDrops(true);
    horiLayoutMain->addWidget(chartView);

    //
    enableTrackerChangedNotify(true);
    enableTrackerMakedNotify(true);
    enableTrackerMarkerClearedNotify(true);
    //
    QObject::connect(timerReplay, &QTimer::timeout, this, [=](){
        //
        const qint64 minimum = currentXTime.toMSecsSinceEpoch();
        const qint64 maximum = currentXTime.addSecs(30).toMSecsSinceEpoch();
        if (maximum >= xRange.second) {
            stopReplay();
            return;
        }
        //
        int rowCount = chartView->rowCount();
        int columnCount = chartView->columnCount();
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                JChart::Chart *chart = chartView->chartAt(row, column);
                if (!chart) {
                    continue;
                }
                chart->setAxisScale(JChart::xBottom, minimum, maximum,
                                    chart->axisStepSize(JChart::xBottom));
                chart->updateScale(JChart::yLeft);
            }
        }
        //
        currentXTime = currentXTime.addMSecs(timerReplay->interval());
    });

    //
    QTimer *timerTemp = new QTimer(q);
    timerTemp->setSingleShot(true);
    QObject::connect(timerTemp, &QTimer::timeout, q, [=](){
        chartView->setColumnWidthSame();
        timerTemp->deleteLater();
    });
    timerTemp->start(200);
}

void ChartFileViewPrivate::reset()
{
    // clear charts
    chartView->clearChart();
    fileTables.clear();
}

bool ChartFileViewPrivate::addDataItem(const FileBaseInfo &baseInfo, const ItemPtr &dataItem,
                                       QStandardItem *item, JChart::Chart *chart)
{
    if (!dataItem || !item) {
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
        if (!addDataItem(baseInfo, complex->table(), item, chart)) {
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
        if (!addDataItem(baseInfo, frame, item, chart)) {
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
    const QString domain = baseInfo.filePath + '/' + item->data(Icd::TreeItemDomainRole).toString();
    bool isNewChart = false;

    //
    if (!chart) {
        // create chart
        chart = createChart(dataItem);
        if (!chart) {
            Q_ASSERT(false);    // logic error
            return false;       // create failure or not supported
        }
        isNewChart = true;
        chart->setChartTheme(static_cast<JChart::ChartTheme>(chartTheme));
        chart->setAxisVisible(JChart::yLeft, showYLabel);
        chart->setAxisAlign(JChart::yLeft, showYAlign);
        chart->setAxisLabelLength(JChart::yLeft, yLabelLength);
        chart->setGridPen(QPen(QColor(60, 60, 60, 150), 0.4, Qt::DashLine));
        if (baseInfo.hasTimeFormat) {
            chart->setShiftType(JChart::ShiftByTime);
            chart->setXScaleDrawType(JChart::ScaleDrawTime);
        } else {
            chart->setShiftType(JChart::ShiftByCount);
            chart->setXScaleDrawType(JChart::ScaleDrawDefault);
        }
        chart->togglePlay(false);
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
        GroupChart *chartData = new GroupChart(chart);
        chart->setUserData(chartData);
        if (chart->chartType() == JChart::ChartTypeFileBitMap) {
            chart->setIdentity(domain);
            setChartTitle(chart, 0, item);
            //
            GroupSeries *seriesData = createGroupSeries(chart->chartType(), baseInfo, dataItem, item);
            if (!seriesData) {
                return false;
            }
            chartData->bitmapData = GroupSeriesPtr(seriesData);
        } else {
            chart->setIdentity(QUuid::createUuid().toString());
        }
    }
    GroupSeries *seriesData = createGroupSeries(chart->chartType(), baseInfo, dataItem, item);
    if (!seriesData) {
        return false;
    }
    //
    JChart::AbstractSeries *series = addSeries(chart, seriesData, domain);
    if (!series) {
        //
    }
    //
    item->setData(true, Icd::TreeBoundRole);

    // sync scale
    if (isNewChart) {
        //
        enableSyncScale(chart, true);
    }

    return true;
}

bool ChartFileViewPrivate::addDataItem(const FileBaseInfo &baseInfo, const TablePtr &table,
                                       QStandardItem *item, JChart::Chart *chart)
{
    if (!table || !item) {
        return false;
    }

    int i = 0;
    int count = item->rowCount();
    const Icd::ItemPtrArray &dataItems = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend() && i < count; ++citer, ++i) {
        const Icd::ItemPtr &dataItem = *citer;
        QStandardItem *itemData = item->child(i);
        if (!itemData) {
            continue;
        }
        //
        if (!addDataItem(baseInfo, dataItem, itemData, chart)) {
            continue;
        }
    }

    //
    if (count > 0) {
        item->setData(true, Icd::TreeBoundRole);
    }

    return true;
}

bool ChartFileViewPrivate::addDataItem(const FileBaseInfo &baseInfo, const FrameItemPtr &frame,
                                       QStandardItem *item, JChart::Chart *chart)
{
    if (!frame || !item) {
        return false;
    }

    int i = 0;
    int count = item->rowCount();
    const Icd::TablePtrMap &tables = frame->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend() && i < count; ++citer, ++i) {
        const Icd::TablePtr &table = citer->second;
        QStandardItem *itemTable = item->child(i);
        if (!itemTable) {
            continue;
        }
        //
        if (!addDataItem(baseInfo, table, itemTable, chart)) {
            continue;
        }
    }

    //
    if (count > 0) {
        item->setData(true, Icd::TreeBoundRole);
    }

    return true;
}

bool ChartFileViewPrivate::addDataItem(const FileBaseInfo &baseInfo, const ObjectPtr &dataObject,
                                       QStandardItem *item, JChart::Chart *chart)
{
    if (!dataObject || !item) {
        return false;
    }

    //
    switch (dataObject->objectType()) {
    case Icd::ObjectTable:
    {
        //
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table, Icd::Object>(dataObject);
        if (!table) {
            return false;
        }

        //
        if (!addDataItem(baseInfo, table, item, chart)) {
            return false;
        }

        break;
    }
    case Icd::ObjectItem:
    {
        //
        const Icd::ItemPtr dataItem = JHandlePtrCast<Icd::Item, Icd::Object>(dataObject);
        if (!dataItem) {
            return false;
        }

        //
        if (!addDataItem(baseInfo, dataItem, item, chart)) {
            return false;
        }

        break;
    }
    default:
        break;
    }

    return true;
}

GroupSeries *ChartFileViewPrivate::createGroupSeries(int chartType, const FileBaseInfo &baseInfo,
                                                     const ItemPtr &dataItem, QStandardItem *item) const
{
    GroupSeries *seriesData = new GroupSeries();
    seriesData->dataItem = dataItem;
    seriesData->item = item;
    seriesData->seriesInfo = SeriesInfoPtr(new SeriesInfo(baseInfo));

    const SeriesInfoPtr &seriesInfo = seriesData->seriesInfo;

    if (baseInfo.hasTimeFormat) {
        seriesInfo->step = 8;
        seriesInfo->xSize = 8;
    }

    seriesInfo->step += seriesInfo->xSize;
    seriesInfo->yOffset += seriesInfo->xSize;

    switch (chartType) {
    case JChart::ChartTypeFileNumeric:
    {
        switch (dataItem->type()) {
        case Icd::ItemBitMap:
        {
            Q_ASSERT(false);
            break;
        }
        case Icd::ItemBitValue:
        {
            const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem, Icd::Object>(dataItem);
            if (!bitItem) {
                break;
            }
            seriesInfo->bitStart = bitItem->bitStart();
            seriesInfo->bitCount = bitItem->bitCount();
            seriesInfo->ySize = static_cast<uchar>(std::ceil(dataItem->bufferSize()));
            break;
        }
        case Icd::ItemNumeric:
        {
            const Icd::NumericItemPtr numericItem = JHandlePtrCast<Icd::NumericItem, Icd::Item>(dataItem);
            if (!numericItem) {
                break;
            }
            seriesInfo->valueScale = numericItem->scale();
            seriesInfo->valueOffset = numericItem->offset();
            seriesInfo->numericTypeY = numericItem->numericType();
            seriesInfo->ySize = static_cast<uchar>(std::ceil(dataItem->bufferSize()));
            break;
        }
        default:
        {
            seriesInfo->ySize = static_cast<uchar>(std::ceil(dataItem->bufferSize()));
            break;
        }}

        seriesInfo->step += seriesInfo->ySize;

        break;
    }
    case JChart::ChartTypeFileBitMap:
    {
        switch (dataItem->type()) {
        case Icd::ItemBitMap:
        {
            Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem);
            seriesInfo->ySize = static_cast<uchar>(itemBit->typeSize());
            break;
        }
        default:
        {
            Q_ASSERT(false);
            break;
        }}

        seriesInfo->step += seriesInfo->ySize;

        break;
    }
    default:
        break;
    }

    // create buffer
    if (!createBuffer(seriesData)) {
        return nullptr;
    }

    return seriesData;
}

bool ChartFileViewPrivate::createBuffer(const GroupSeries *seriesData) const
{
    if (!seriesData) {
        Q_ASSERT(false);
        return false;
    }

    const SeriesInfoPtr seriesInfo = seriesData->seriesInfo;
    const FileBaseInfo &baseInfo = seriesInfo->baseInfo;

    QFile file(baseInfo.filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    return createBuffer(seriesData, file);
}

bool ChartFileViewPrivate::createBuffer(const GroupSeries *seriesData, QFile &file) const
{
    const SeriesInfoPtr seriesInfo = seriesData->seriesInfo;
    const FileBaseInfo &baseInfo = seriesInfo->baseInfo;

    int tableStep = static_cast<int>(std::ceil(baseInfo.table->bufferSize()));
    if (baseInfo.hasTimeFormat) {
        tableStep += 8;
    }

    //
    int count = static_cast<int>(file.size() / tableStep);
    if (count <= 0) {
        return false;
    }

    seriesInfo->bufferSize = seriesInfo->step * count;
    seriesInfo->buffer = BufferPtr(new uchar[static_cast<unsigned int>(seriesInfo->bufferSize)],
            [](uchar *ptr){
        delete[] ptr;
    });

    int yOffset = seriesInfo->xSize + static_cast<int>(seriesData->dataItem->bufferOffset());
    uchar *fileBuffer, *buffer = seriesInfo->buffer.data();
    for (int i = 0; i < count; ++i, buffer += seriesInfo->step) {
        fileBuffer = file.map(baseInfo.headerSize + i * tableStep, tableStep);
        if (fileBuffer) {
            // x
            if (seriesInfo->xSize > 0) {
                memcpy(buffer, fileBuffer, seriesInfo->xSize);
            }
            // y
            memcpy((buffer + seriesInfo->yOffset), (fileBuffer + yOffset), seriesInfo->ySize);
            // unmap
            file.unmap(fileBuffer);
        } else {
            return false;
        }
    }

    return true;
}

JChart::Chart *ChartFileViewPrivate::createChart(const Icd::ItemPtr &dataItem)
{
    Q_Q(ChartFileView);
    if (!dataItem) {
        return nullptr;
    }

    JChart::Chart *newChart = nullptr;

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
        JChart::FileNumericChart *numericChart = new JChart::FileNumericChart(q);
        // init
        // specs

        newChart = numericChart;
        break;
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
        JChart::FileBitChart *bitChart = new JChart::FileBitChart(q);
        bitChart->setLegendVisible(showYAlign);
        bitChart->setBitsRange(itemBit->bitStart(), itemBit->bitStart() + itemBit->bitCount() - 1);
        const std::map<icd_uint64, std::string> &specs = itemBit->specs();
        for (std::map<icd_uint64, std::string>::const_iterator citer = specs.cbegin();
             citer != specs.cend(); ++citer) {
            JChart::AbstractSeries *series = bitChart->seriesAt(static_cast<int>(citer->first));
            if (series) {
                series->setTitle(QString::fromStdString(Icd::BitItem::nameOf(citer->second)));
            }
        }
        // init

        newChart = bitChart;
        break;
    }
    default:
        break;
    }

    return newChart;
}

JChart::AbstractSeries *ChartFileViewPrivate::addSeries(JChart::Chart *chart, GroupSeries *seriesData,
                                                        const QString &domain)
{
    if (!chart || !seriesData) {
        return nullptr;
    }

    const int seriesCount = chart->seriesCount();
    JChart::Chart *lastChart = chartView->lastChart(chart);
    QPair<qreal, qreal> axisXInterval;
    if (lastChart) {
        axisXInterval = lastChart->axisInterval(JChart::xBottom);
    } else if (seriesCount > 0) {
        axisXInterval = chart->axisInterval(JChart::xBottom);
    }

    const SeriesInfoPtr seriesInfo = seriesData->seriesInfo;

    const QString path = seriesData->item->data(Icd::TreeItemPathRole).toString();
    JChart::AbstractSeries *series = nullptr;
    switch (chart->chartType()) {
    case JChart::ChartTypeFileNumeric:
    {
        series = chart->addSeries(domain, path.section('@', 0, 1));
        JChart::FileNumericSeries *fileSeries = dynamic_cast<JChart::FileNumericSeries *>(series);
        if (!fileSeries) {
            break;
        }
        series->setIdentity(domain);
        series->setUserData(seriesData);
        if (chart->seriesCount() > 1) {
            chart->setTitle("");
        } else {
            setChartTitle(chart, 0, seriesData->item);
        }
        if (chart->seriesCount() >= 2) {
            chart->setLegendVisible(true);
        }
        // buffser
        fileSeries->setBuffer(seriesInfo->buffer.data(),
                              static_cast<size_t>(seriesInfo->bufferSize),
                              seriesInfo->step);
        // attributes
        fileSeries->setAttributes(seriesInfo->xSize, seriesInfo->ySize,
                                  seriesInfo->xOffset, seriesInfo->yOffset,
                                  seriesInfo->valueScale, seriesInfo->valueOffset,
                                  seriesInfo->numericTypeY,
                                  seriesInfo->bitStart, seriesInfo->bitCount);

        break;
    }
    case JChart::ChartTypeFileBitMap:
    {
        JChart::FileBitChart *bitChart = qobject_cast<JChart::FileBitChart *>(chart);
        if (!bitChart) {
            break;
        }
        // buffser
        bitChart->setBuffer(seriesInfo->buffer.data(), static_cast<size_t>(seriesInfo->bufferSize),
                            seriesInfo->step);
        // attributes
        bitChart->setAttributes(seriesInfo->xSize, seriesInfo->ySize,
                                seriesInfo->xOffset, seriesInfo->yOffset);
        break;
    }
    default:
        break;
    }

    // sync scale
    if ((lastChart && xAxisSync) || seriesCount > 0) {
        chart->setAxisScale(JChart::xBottom, axisXInterval.first, axisXInterval.second, 0);
    } else {
        chart->updateScale(JChart::xBottom);
    }

    return series;
}

bool ChartFileViewPrivate::findChart(JChart::Chart *chart)
{
    if (!chart) {
        return false;
    }

    int rowCount = chartView->rowCount();
    int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            if (chartView->chartAt(row, column) == chart) {
                return true;
            }
        }
    }

    return false;
}

JChart::AbstractSeries *ChartFileViewPrivate::findSeries(const QString &domain) const
{
    int rowCount = chartView->rowCount();
    int columnCount = chartView->columnCount();
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

void ChartFileViewPrivate::setChartTitle(JChart::Chart *chart, int sectionOffset,
                                         QStandardItem *item)
{
    const QString path = item->data(Icd::TreeItemPathRole).toString();
    chart->setTitle(path.section('@', sectionOffset, sectionOffset)
                    + "<font size=2 style='font-style:italic;'>@"
                    + path.section('@', sectionOffset + 1) + "</font>");
}

void ChartFileViewPrivate::updateXAxisSync()
{
    if (timerReplay->isActive() || !xAxisSync) {
        return;
    }

    // reset chartview
    updateScale();
}

void ChartFileViewPrivate::updateData()
{
    if (timerReplay->isActive()) {
        return;
    }

    //
    int rowCount = chartView->rowCount();
    int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *chart = chartView->chartAt(row, column);
            if (!chart || chart->seriesCount() == 0) {
                continue;
            }
            switch (chart->chartType()) {
            case JChart::ChartTypeFileBitMap:
            {
                JChart::FileBitChart *bitChart = qobject_cast<JChart::FileBitChart *>(chart);
                if (!bitChart) {
                    break;
                }
                const GroupChart *chartData = dynamic_cast<GroupChart *>(chart->userData());
                if (!chartData || !chartData->bitmapData) {
                    break;
                }
                //
                if (!createBuffer(chartData->bitmapData.get())) {
                    break;
                }
                // buffser
                const SeriesInfoPtr seriesInfo = chartData->bitmapData->seriesInfo;
                bitChart->setBuffer(seriesInfo->buffer.data(), static_cast<size_t>(seriesInfo->bufferSize),
                                    seriesInfo->step);

                break;
            }
            case JChart::ChartTypeFileNumeric:
            {
                int seriesCount = chart->seriesCount();
                for (int i = 0; i < seriesCount; ++i) {
                    JChart::AbstractSeries *series = chart->seriesAt(i);
                    JChart::FileNumericSeries *numericSeries = dynamic_cast<JChart::FileNumericSeries *>(series);
                    if (!numericSeries) {
                        continue;
                    }
                    //
                    const GroupSeries *seriesData = dynamic_cast<GroupSeries *>(series->userData());
                    if (!seriesData) {
                        continue;
                    }
                    //
                    if (!createBuffer(seriesData)) {
                        continue;
                    }
                    //
                    const SeriesInfoPtr seriesInfo = seriesData->seriesInfo;
                    numericSeries->setBuffer(seriesInfo->buffer.data(),
                                             static_cast<size_t>(seriesInfo->bufferSize), seriesInfo->step);
                }
                break;
            }
            default:
                Q_ASSERT(false);
                break;
            }
            // update
            chart->updateScale(JChart::xBottom);
            chart->updateScale(JChart::yLeft);
        }
    }
}

void ChartFileViewPrivate::startReplay()
{
    //
    if (timerReplay->isActive()) {
        return;
    }
#if 0   //TODO
    //
    int bufferStep = (int)std::ceil(table->bufferSize());
    if (hasTimeFormat) {
        bufferStep += 8;
    } else {
        //
    }
    int tableCount = 0;
    if (bufferStep > 0) {
        tableCount = bufferSize / bufferStep;
    } else {
        tableCount = 0;
    }
    if (tableCount == 0) {
        return;
    }

    //
    qint64 firstTime = *(qint64 *)(buffer);
    qint64 lastTime = *(qint64 *)(buffer + bufferStep * tableCount);
    xRange = QPair<qreal, qreal>(firstTime, lastTime);
    currentXTime = QDateTime::fromMSecsSinceEpoch(firstTime);

    //
    timerReplay->start();
#endif
}

void ChartFileViewPrivate::stopReplay()
{
    if (!timerReplay->isActive()) {
        return;
    }

    //
    timerReplay->stop();
    QApplication::removePostedEvents(this, QEvent::Timer);

    // reset chartview
    updateScale();

    //
    Q_Q(ChartFileView);
    emit q->replayFinished();
}

void ChartFileViewPrivate::onTrackerChanged(JChart::Chart *chart, const QPointF &pos, bool visible)
{
    if (!chart || !syncTrack) {
        return;
    }

    enableTrackerChangedNotify(false);

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

    enableTrackerChangedNotify(true);
}

void ChartFileViewPrivate::onTrackerMarked(JChart::Chart *chart, const QPointF &pos)
{
    if (!chart || !syncTrack) {
        return;
    }

    enableTrackerMakedNotify(false);

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

    enableTrackerMakedNotify(true);
}

void ChartFileViewPrivate::onTrackerMarkerCleared(JChart::Chart *chart)
{
    if (!chart || !syncTrack) {
        return;
    }

    enableTrackerMarkerClearedNotify(false);

    const int rowCount = chartView->rowCount();
    const int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *_chart = chartView->chartAt(row, column);
            if (!_chart) {
                continue;
            }
            if (_chart != chart) {
                _chart->clearMarker();
            }
        }
    }

    enableTrackerMarkerClearedNotify(true);
}

void ChartFileViewPrivate::onScaleDivChanged(int axisId, qreal minimum, qreal maximum)
{
    JChart::Chart *chart = qobject_cast< JChart::Chart*>(sender());
    if (!chart) {
        return;
    }

    if (!updateSyncScale(chart, axisId, minimum, maximum)) {
        //
        return;
    }
}

void ChartFileViewPrivate::updateScale()
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

bool ChartFileViewPrivate::syncNewChartScaleX(JChart::Chart *chart)
{
    if (!chart) {
        return false;
    }

    JChart::Chart *lastChart = chartView->lastChart(chart);
    if (!lastChart) {
        return true;
    }

    QPair<qreal, qreal> interval = lastChart->axisInterval(JChart::xBottom);
    chart->setAxisScale(JChart::xBottom, interval.first, interval.second, 0);

    int rowCount = chartView->rowCount();
    int columnCount = chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *_chart = chartView->chartAt(row, column);
            if (!_chart) {
                continue;
            }
            if (_chart == chart) {
                continue;
            }
            _chart->setAxisScale(JChart::xBottom, interval.first, interval.second, 0);
        }
    }

    return true;
}

bool ChartFileViewPrivate::updateSyncScale(JChart::Chart *chart,
                                           int axisId, qreal minimum, qreal maximum)
{
    //
    if (!chart || !xAxisSync || timerReplay->isActive()) {
        return false;
    }
    //
    switch (axisId) {
    case JChart::xBottom:
    {
        int rowCount = chartView->rowCount();
        int columnCount = chartView->columnCount();
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                JChart::Chart *_chart = chartView->chartAt(row, column);
                if (!_chart) {
                    continue;
                }
                if (_chart == chart) {
                    continue;
                }
                enableSyncScale(_chart, false);
                _chart->setAxisScale(axisId, minimum, maximum, 0);
                enableSyncScale(_chart, true);
            }
        }
        break;
    }
    default:
        break;
    }

    return true;
}

void ChartFileViewPrivate::enableSyncScale(JChart::Chart *chart, bool enabled)
{
    if (!chart) {
        return;
    }

    QObject::disconnect(chart, &JChart::Chart::scaleDivChanged,
                        this, &ChartFileViewPrivate::onScaleDivChanged);
    if (enabled) {
        QObject::connect(chart, &JChart::Chart::scaleDivChanged,
                         this, &ChartFileViewPrivate::onScaleDivChanged);
    }
}

void ChartFileViewPrivate::enableTrackerChangedNotify(bool enabled)
{
    QObject::disconnect(chartView, &JChart::View::trackerChanged,
                        this, &ChartFileViewPrivate::onTrackerChanged);
    if (enabled) {
        QObject::connect(chartView, &JChart::View::trackerChanged,
                         this, &ChartFileViewPrivate::onTrackerChanged, Qt::QueuedConnection);
    }
}

void ChartFileViewPrivate::enableTrackerMakedNotify(bool enabled)
{
    QObject::disconnect(chartView, &JChart::View::trackerMarked,
                        this, &ChartFileViewPrivate::onTrackerMarked);
    if (enabled) {
        QObject::connect(chartView, &JChart::View::trackerMarked,
                         this, &ChartFileViewPrivate::onTrackerMarked, Qt::QueuedConnection);
    }
}

void ChartFileViewPrivate::enableTrackerMarkerClearedNotify(bool enabled)
{
    QObject::disconnect(chartView, &JChart::View::trackerMarkerCleared,
                        this, &ChartFileViewPrivate::onTrackerMarkerCleared);
    if (enabled) {
        QObject::connect(chartView, &JChart::View::trackerMarkerCleared,
                         this, &ChartFileViewPrivate::onTrackerMarkerCleared, Qt::QueuedConnection);
    }
}

} // end of namespace Icd
