#ifndef ICDCHART_VIEW_P_H
#define ICDCHART_VIEW_P_H

#include <QWidget>
#include "icdchart_view.h"
#include "jchart/jchart.h"
#include "jchart/jabstractseries.h"
#include "icdcore/icd_item_frame.h"
#include "icdworker/icdworker.h"
#include "icdwidget/icdwidget_global.h"
#include <QStandardItem>
#include <QCoreApplication>
#include <QTimer>

namespace Icd {

/**
 * @brief The SeriesData class
 */
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

/**
 * @brief The ChartData class
 */
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

    /**
     * @brief init
     */
    void init();

    /**
     * @brief addDataItem
     * @param worker
     * @param dataItem
     * @param item
     * @param chart
     * @return
     */
    bool addDataItem(const Icd::WorkerPtr &worker, const Icd::ItemPtr &dataItem,
                     QStandardItem *item, JChart::Chart *chart);

    /**
     * @brief addDataItem
     * @param worker
     * @param table
     * @param item
     * @param chart
     * @return
     */
    bool addDataItem(const Icd::WorkerPtr &worker, const Icd::TablePtr &table,
                     QStandardItem *item, JChart::Chart *chart);

    /**
     * @brief addDataItem
     * @param worker
     * @param frame
     * @param item
     * @param chart
     * @return
     */
    bool addDataItem(const Icd::WorkerPtr &worker, const Icd::FrameItemPtr &frame,
                     QStandardItem *item, JChart::Chart *chart);

    /**
     * @brief addDataItem
     * @param domain
     * @param worker
     * @param dataObject
     * @param item
     * @param chart
     * @return
     */
    bool addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                     const Icd::ObjectPtr &dataObject, QStandardItem *item,
                     JChart::Chart *chart);

    /**
     * @brief createChart
     * @param dataItem
     * @return
     */
    JChart::Chart *createChart(const Icd::ItemPtr &dataItem);

    /**
     * @brief findChart
     * @param chart
     * @return
     */
    bool findChart(JChart::Chart *chart);

    /**
     * @brief findSeries
     * @param domain
     * @return
     */
    JChart::AbstractSeries *findSeries(const QString &domain) const;

    /**
     * @brief isRunning
     * @return
     */
    bool isRunning() const;

    /**
     * @brief start
     */
    void setRunning(bool value);

    /**
     * @brief setChartTitle
     * @param chart
     * @param sectionOffset
     * @param item
     */
    void setChartTitle(JChart::Chart *chart, int sectionOffset, QStandardItem *item);

    /**
     * @brief updateXAxisSync
     */
    void updateXAxisSync();

signals:

public slots:
    void updateChart(JChart::Chart *chart);
    void onTrackerChanged(JChart::Chart *chart, const QPointF &pos, bool visible);
    void onTrackerMarked(JChart::Chart *chart, const QPointF &pos);

private:
    void updateScale();

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
};

} // end of namespace Icd

#endif // ICDCHART_VIEW_P_H
