#ifndef ICDCHART_VIEW_P_H
#define ICDCHART_VIEW_P_H

#include <QWidget>
#include "../icdchart_view.h"
#include "jchart/jchart.h"
#include "jchart/jabstractseries.h"
#include "icdcore/icd_item_frame.h"
#include "icdworker/icdworker.h"
#include "icdwidget/icdwidget_global.h"
#include <QStandardItem>
#include <QCoreApplication>
#include <QTimer>

namespace Icd {

class SeriesData : public QObjectUserData
{
public:
    Icd::WorkerPtr worker;
    Icd::ItemPtr dataItem;
    QStandardItem *item;

    SeriesData() : item(nullptr)
    {

    }

    ~SeriesData()
    {
        if (item) {
            item->setData(QVariant::Invalid, Icd::TreeBoundRole);
        }
    }

    // disabled copy and voluation
    SeriesData(const SeriesData &);
    SeriesData &operator =(const SeriesData &);
};

class ChartData : public QObjectUserData
{
public:
    JChart::Chart *chart;
    QStandardItem *item;
    SeriesData *bitmapData;
    QTimer *timer;

    ChartData(JChart::Chart *chart)
        : chart(chart)
        , item(nullptr)
        , bitmapData(nullptr)
        , timer(nullptr)
    {

    }

    ~ChartData()
    {
        if (item) {
            item->setData(QVariant::Invalid, Icd::TreeBoundRole);
        }

        if (bitmapData) {
            delete bitmapData;
            bitmapData = nullptr;
        }

        if (timer) {
            timer->stop();
            timer->disconnect();
            if (chart) {
                QCoreApplication::removePostedEvents(chart, QEvent::Timer);
            }
            timer->deleteLater();
        }
    }

    // disabled copy and voluation
    ChartData(const ChartData &);
    ChartData &operator =(const ChartData &);
};

// class JChartViewPrivate

class ChartViewPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ChartViewPrivate(ChartView *q);

    void init(bool styled);

    bool addDataItem(const Icd::WorkerPtr &worker, const Icd::ItemPtr &dataItem,
                     QStandardItem *item, JChart::Chart *chart);
    bool addDataItem(const Icd::WorkerPtr &worker, const Icd::TablePtr &table,
                     QStandardItem *item, JChart::Chart *chart);
    bool addDataItem(const Icd::WorkerPtr &worker, const Icd::FrameItemPtr &frame,
                     QStandardItem *item, JChart::Chart *chart);
    bool addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                     const Icd::ObjectPtr &dataObject, QStandardItem *item,
                     JChart::Chart *chart);
    JChart::Chart *createChart(const Icd::ItemPtr &dataItem);
    bool findChart(JChart::Chart *chart);
    JChart::AbstractSeries *findSeries(const QString &domain) const;
    bool isRunning() const;
    void setRunning(bool value);
    void setChartTitle(JChart::Chart *chart, int sectionOffset, QStandardItem *item);
    void updateXAxisSync();

signals:

public slots:
    void updateChart(JChart::Chart *chart);
    void onTrackerChanged(JChart::Chart *chart, const QPointF &pos, bool visible);
    void onTrackerMarked(JChart::Chart *chart, const QPointF &pos);
    void onTrackerMarkerCleared(JChart::Chart *chart);

private:
    void updateScale();

    void enableTrackerChangedNotify(bool enabled);
    void enableTrackerMakedNotify(bool enabled);
    void enableTrackerMarkerClearedNotify(bool enabled);

private:
    J_DECLARE_PUBLIC(ChartView)
    JChart::View *chartView;
    bool running;
    quint32 timerInterval;   // unit: ms
    int timerId;
    bool xAxisSync;
    bool showYLabel;
    bool showYAlign;
    int yLabelLength;
    bool syncTrack;
    int chartTheme;
    bool hexAsciiVisible;
    int hexColumnCount;
};

} // end of namespace Icd

#endif // ICDCHART_VIEW_P_H
