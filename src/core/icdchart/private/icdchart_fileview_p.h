#ifndef ICDCHART_FILEVIEW_P_H
#define ICDCHART_FILEVIEW_P_H

#include "../icdchart_fileview.h"
#include <QFile>
#include "../icdchart_view.h"
#include "jchart/jchart.h"
#include "jchart/jabstractseries.h"
#include "icdcore/icd_item_frame.h"
#include "icdcore/icd_item_numeric.h"
#include "icdwidget/icdwidget_global.h"
#include <QStandardItem>

namespace Icd {

typedef QSharedPointer<uchar> BufferPtr;

// struct FileBaseInfo

struct FileBaseInfo
{
    Icd::TablePtr table;
    QString filePath;
    bool hasTimeFormat;
    int headerSize;

    FileBaseInfo()
        : table(Icd::TablePtr())
        , hasTimeFormat(false)
        , headerSize(0)
    {

    }

    FileBaseInfo(const Icd::TablePtr &table, const QString &filePath,
                 bool hasTimeFormat, int headerSize)
        : table(table)
        , filePath(filePath)
        , hasTimeFormat(hasTimeFormat)
        , headerSize(headerSize)
    {

    }

    FileBaseInfo(const FileBaseInfo &other)
    {
        operator =(other);
    }

    FileBaseInfo &operator =(const FileBaseInfo &other)
    {
        if (this == &other) {
            return *this;
        }
        table = other.table;
        filePath = other.filePath;
        hasTimeFormat = other.hasTimeFormat;
        headerSize = other.headerSize;
        return *this;
    }
};

// struct SeriesInfo

struct SeriesInfo;
typedef JHandlePtr<SeriesInfo> SeriesInfoPtr;

struct SeriesInfo
{
    FileBaseInfo baseInfo;

    BufferPtr buffer;
    int bufferSize;
    int step;
    uchar xSize, ySize;
    int xOffset, yOffset;
    qreal valueScale, valueOffset;
    int numericTypeX, numericTypeY;
    int bitStart, bitCount;

    SeriesInfo()
        : buffer(BufferPtr())
        , bufferSize(0)
        , step(0)
        , xSize(0), ySize(0)
        , xOffset(0), yOffset(0)
        , valueScale(1), valueOffset(0)
        , numericTypeX(Icd::NumericInvalid)
        , numericTypeY(Icd::NumericInvalid)
        , bitStart(0), bitCount(0)
    {

    }

    SeriesInfo(const FileBaseInfo &baseInfo)
        : baseInfo(baseInfo)
        , buffer(BufferPtr())
        , bufferSize(0)
        , step(0)
        , xSize(0), ySize(0)
        , xOffset(0), yOffset(0)
        , valueScale(1), valueOffset(0)
        , numericTypeX(Icd::NumericInvalid)
        , numericTypeY(Icd::NumericInvalid)
        , bitStart(0), bitCount(0)
    {

    }

    SeriesInfo(const SeriesInfo &other)
    {
        operator =(other);
    }

    SeriesInfo &operator =(const SeriesInfo &other)
    {
        if (this == &other) {
            return *this;
        }
        baseInfo = other.baseInfo;
        buffer = other.buffer;
        bufferSize = other.bufferSize;
        step = other.step;
        xSize = other.xSize;
        ySize = other.ySize;
        xOffset = other.xOffset;
        yOffset = other.yOffset;
        valueScale = other.valueScale;
        valueOffset = other.valueOffset;
        numericTypeX = other.numericTypeX;
        numericTypeY = other.numericTypeY;
        bitStart = other.bitStart;
        bitCount = other.bitCount;
        return *this;
    }
};

class GroupSeries;
typedef JHandlePtr<GroupSeries> GroupSeriesPtr;

/**
 * @brief The GroupSeries struct
 */
class GroupSeries : public QObjectUserData
{
public:
    Icd::ItemPtr dataItem;
    QStandardItem *item;
    SeriesInfoPtr seriesInfo;

    GroupSeries() : item(nullptr)
    {

    }

    ~GroupSeries()
    {
        if (item) {
            item->setData(QVariant::Invalid, Icd::TreeBoundRole);
        }
    }

    // disabled copy and voluation
    GroupSeries(const GroupSeries &);
    GroupSeries &operator =(const GroupSeries &);
};

/**
 * @brief The GroupChart struct
 */
class GroupChart : public QObjectUserData
{
public:
    JChart::Chart *chart;
    QStandardItem *item;
    GroupSeriesPtr bitmapData;

    GroupChart(JChart::Chart *chart)
        : chart(chart)
        , item(nullptr)
    {

    }

    ~GroupChart()
    {
        if (item) {
            item->setData(QVariant::Invalid, Icd::TreeBoundRole);
        }
    }

    // disabled copy and voluation
    GroupChart(const GroupChart &);
    GroupChart &operator =(const GroupChart &);
};

// class ChartFileViewPrivate

class ChartFileViewPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ChartFileViewPrivate(ChartFileView *q);
    ~ChartFileViewPrivate();

    void init(bool styled);
    void reset();

    bool addDataItem(const FileBaseInfo &baseInfo, const Icd::ItemPtr &dataItem,
                     QStandardItem *item, JChart::Chart *chart);
    bool addDataItem(const FileBaseInfo &baseInfo, const Icd::TablePtr &table,
                     QStandardItem *item, JChart::Chart *chart);
    bool addDataItem(const FileBaseInfo &baseInfo, const Icd::FrameItemPtr &frame,
                     QStandardItem *item, JChart::Chart *chart);
    bool addDataItem(const FileBaseInfo &baseInfo, const Icd::ObjectPtr &dataObject,
                     QStandardItem *item, JChart::Chart *chart);

    GroupSeries *createGroupSeries(int chartType, const FileBaseInfo &baseInfo,
                                   const ItemPtr &dataItem, QStandardItem *item) const;
    bool createBuffer(const GroupSeries *seriesData) const;
    bool createBuffer(const GroupSeries *seriesData, QFile &file) const;
    JChart::Chart *createChart(const Icd::ItemPtr &dataItem);
    JChart::AbstractSeries *addSeries(JChart::Chart *chart, GroupSeries *seriesData,
                                      const QString &domain);
    bool findChart(JChart::Chart *chart);
    JChart::AbstractSeries *findSeries(const QString &domain) const;
    void setChartTitle(JChart::Chart *chart, int sectionOffset, QStandardItem *item);

    void updateXAxisSync();
    void updateData();
    void startReplay();
    void stopReplay();

public slots:
    void onTrackerChanged(JChart::Chart *chart, const QPointF &pos, bool visible);
    void onTrackerMarked(JChart::Chart *chart, const QPointF &pos);
    void onScaleDivChanged(int axisId, qreal minimum, qreal maximum);

private:
    void updateScale();
    bool syncNewChartScaleX(JChart::Chart *chart);
    bool updateSyncScale(JChart::Chart *chart, int axisId, qreal minimum, qreal maximum);
    void enableSyncScale(JChart::Chart *chart, bool enabled);

private:
    J_DECLARE_PUBLIC(ChartFileView)
    JChart::View *chartView;
    bool xAxisSync;
    bool showYLabel;
    bool showYAlign;
    int yLabelLength;
    QTimer *timerReplay;
    bool syncTrack;
    int chartTheme;
    QDateTime currentXTime;
    QPair<qreal, qreal> xRange;
    QMap<QString/*filePath*/,Icd::TablePtr> fileTables;
};

} // end of namespace Icd

#endif // ICDCHART_FILEVIEW_P_H
