#include "precomp.h"
#include "icdchart_fileview.h"
#include "private/icdchart_fileview_p.h"

namespace Icd {

ChartFileView::ChartFileView(QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new ChartFileViewPrivate(this))
{
    Q_D(ChartFileView);
    d->init(true);
}

ChartFileView::ChartFileView(bool styled, QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new ChartFileViewPrivate(this))
{
    Q_D(ChartFileView);
    d->init(styled);
}

ChartFileView::~ChartFileView()
{
    Q_D(ChartFileView);
    delete d;
}

void ChartFileView::focusItem(const QString &domain)
{
    Q_D(ChartFileView);
    const int rowCount = d->chartView->rowCount();
    const int columnCount = d->chartView->columnCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            JChart::Chart *chart = d->chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            if (chart->chartType() == JChart::ChartTypeFileBitMap) {
                if (chart->identity() == domain) {
                    d->chartView->focusChart(row, column);
                    return;
                }
            } else {
                int seriesCount = chart->seriesCount();
                for (int i = 0; i < seriesCount; ++i) {
                    JChart::AbstractSeries *series = chart->seriesAt(i);
                    if (!series) {
                        continue;
                    }
                    if (series->identity() == domain) {
                        d->chartView->focusChart(row, column);
                        return;
                    }
                }
            }
        }
    }
}

void ChartFileView::removeItem(const QString &domain)
{
    Q_D(ChartFileView);
    const int rowCount = d->chartView->rowCount();
    const int columnCount = d->chartView->columnCount();
    for (int row = rowCount - 1; row >= 0; --row) {
        for (int column = columnCount - 1; column >= 0; --column) {
            JChart::Chart *chart = d->chartView->chartAt(row, column);
            if (!chart) {
                continue;
            }
            if (chart->chartType() == JChart::ChartTypeFileBitMap) {
                if (chart->identity().startsWith(domain)) {
                    chart->disconnect();
                    d->chartView->removeChart(row, column);
                }
            } else {
                bool modify = false;
                int seriesCount = chart->seriesCount();
                for (int i = seriesCount - 1; i >= 0; --i) {
                    JChart::AbstractSeries *series = chart->seriesAt(i);
                    if (!series) {
                        continue;
                    }
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
                        GroupChart *chartData = dynamic_cast<GroupChart *>(chart->userData());
                        if (chartData && chartData->bitmapData) {
                            d->setChartTitle(chart, 0, chartData->bitmapData->item);
                        }
                    }
                } else {
                    seriesCount = chart->seriesCount();
                    if (seriesCount == 0) {
                        d->chartView->removeChart(row, column);
                    }
                }
            }
        }
    }
}

void ChartFileView::removeChart(const QString &domain)
{
    Q_D(ChartFileView);
    const int rowCount = d->chartView->rowCount();
    const int columnCount = d->chartView->columnCount();
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

void ChartFileView::reset()
{
    Q_D(ChartFileView);
    d->reset();
}

bool ChartFileView::xAxisSync() const
{
    Q_D(const ChartFileView);
    return d->xAxisSync;
}

bool ChartFileView::isReplaying() const
{
    Q_D(const ChartFileView);
    if (d->timerReplay) {
        return d->timerReplay->isActive();
    } else {
        return false;
    }
}

int ChartFileView::replayPeriod() const
{
    Q_D(const ChartFileView);
    if (d->timerReplay) {
        return d->timerReplay->interval();
    } else {
        return 0;
    }
}

void ChartFileView::clearCharts()
{
    Q_D(ChartFileView);
    d->chartView->clearChart();
}

void ChartFileView::addTable(const QString &filePath, const TablePtr &table)
{
    Q_D(ChartFileView);
    d->fileTables[filePath] = table;
}

void ChartFileView::removeTable(const QString &filePath)
{
    Q_D(ChartFileView);
    d->fileTables.remove(filePath);
}

TablePtr ChartFileView::tableByFilePath(const QString &filePath) const
{
    Q_D(const ChartFileView);
    QMap<QString, Icd::TablePtr>::ConstIterator citer = d->fileTables.find(filePath);
    if (citer != d->fileTables.cend()) {
        return citer.value();
    } else {
        return TablePtr();
    }
}

void ChartFileView::showYLabel(bool checked)
{
    Q_D(ChartFileView);
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

void ChartFileView::showYAlign(bool align)
{
    Q_D(ChartFileView);
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
                if (chart->chartType() == JChart::ChartTypeFileBitMap) {
                    chart->setLegendVisible(align);
                }
            }
        }
    }
}

void ChartFileView::setYLabelLength(int length)
{
    Q_D(ChartFileView);
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

void ChartFileView::setColumnCount(int count)
{
    Q_D(ChartFileView);
    if (d->chartView->columnCount() != count) {
        clearCharts();
        d->chartView->setColumnCount(count);
        d->chartView->setColumnWidthSame();
    }
}

void ChartFileView::setColumnWidthSame()
{
    Q_D(ChartFileView);
    d->chartView->setColumnWidthSame();
}

void ChartFileView::setSyncTrack(bool enabled)
{
    Q_D(ChartFileView);
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

void ChartFileView::setChartTheme(int chartTheme)
{
    Q_D(ChartFileView);
    d->chartTheme = chartTheme;
    d->chartView->setChartTheme(static_cast<JChart::ChartTheme>(chartTheme));
}

void ChartFileView::setXAxisSync(bool value)
{
    Q_D(ChartFileView);
    if (value != d->xAxisSync) {
        d->xAxisSync = value;
        d->updateXAxisSync();
        emit xAxisSyncChanged(value);
    }
}

void ChartFileView::setReplayPeriod(int value)
{
    Q_D(ChartFileView);
    if (d->timerReplay) {
        d->timerReplay->setInterval(value);
    }
}

void ChartFileView::updateData()
{
    Q_D(ChartFileView);
    return d->updateData();
}

void ChartFileView::startReplay()
{
    Q_D(ChartFileView);
    d->startReplay();
}

void ChartFileView::stopReplay()
{
    Q_D(ChartFileView);
    d->stopReplay();
}

void ChartFileView::toggleReplay(bool enabled)
{
    Q_D(ChartFileView);
    if (isReplaying() == enabled) {
        return;
    }

    if (enabled) {
        d->startReplay();
    } else {
        d->stopReplay();
    }
}

void ChartFileView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ChartFileView::dropEvent(QDropEvent *event)
{
    //
    if (!event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        return;
    }

    //
    QStandardItem *itemTable = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("itemTable").value<void *>());
    if (!itemTable) {
        return;
    }

    const QString filePath = itemTable->data(TreeFilePathRole).toString();
    if (!QFile::exists(filePath)) {
        return;
    }

    const Icd::TablePtr table = tableByFilePath(filePath);
    if (!table) {
        qWarning("Worker has no binding recv-channel!");
        return;
    }

    const bool hasTimeFormat = itemTable->data(Icd::TreeHasTimeFormatRole).toBool();
    const int headerSize = itemTable->data(Icd::TreeHeaderSizeRole).toInt();

    //
    QStandardItem *item = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("item").value<void *>());
    if (!item) {
        return;
    }

    //
    JAutoCursor autoCursor(Qt::BusyCursor);

    //
    Q_D(ChartFileView);
    JChart::Chart *chart = nullptr;
    if (d->chartView->chartCount() != 0) {
        Qt::KeyboardModifiers keys = event->keyboardModifiers();
        switch (keys) {
        case Qt::ControlModifier:
        case Qt::ShiftModifier:
            chart = d->chartView->model()->chartAt(event->pos());
            if (chart && chart->chartType() == JChart::ChartTypeFileBitMap) {
                chart = nullptr;  // not supported multi-chart for bit-chart
            }
            break;
        default:
            break;
        }
    }

    //
    const QString itemDomain = event->mimeData()->property("domain").toString();

    //
    Icd::ObjectPtr dataObject = Icd::ObjectPtr();
    switch (item->type()) {
    case Icd::TreeItemTypeTable:
    {
        dataObject = table;
        break;
    }
    case Icd::TreeItemTypeDataItem:
    {
        // get dataItem object
        Icd::ObjectPtr dataItem = table->itemByDomain(itemDomain.section('/', 3).toStdString());
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
        Icd::ObjectPtr itemTable = table->tableByDomain(itemDomain.section('/', 3).toStdString());
        if (!itemTable) {
            break;
        }
        //
        dataObject = itemTable;
        break;
    }
    default:
        break;
    }

    //
    if (!dataObject) {
        return;
    }

    JAutoCursor cursor(Qt::BusyCursor);

    //
    QApplication::processEvents(QEventLoop::EventLoopExec);

    //
    if (!d->addDataItem(FileBaseInfo(table, filePath, hasTimeFormat, headerSize),
                        dataObject, item, chart)) {
        return;
    }
}

} // end of namespace Icd
