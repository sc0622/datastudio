#ifndef JCHART_H
#define JCHART_H

#include "jchart_global.h"
#include <QFrame>

class QwtPlot;

namespace JChart {

class ChartPrivate;
class AbstractSeries;
class AbstractAxis;
class JPlotZoomer;
class ZoneSlider;

/**
 * @brief 图表类
 */
class JCHART_EXPORT Chart : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param type : 图表类型
     * @param parent : 父窗口
     */
    explicit Chart(ChartType type, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~Chart();

    /**
     * @brief chartType
     * @return
     */
    ChartType chartType() const;

    /**
     * @brief setChartType
     * @param type
     */
    void setChartType(ChartType type);

    /**
     * @brief chartTheme
     * @return
     */
    ChartTheme chartTheme() const;

    /**
     * @brief JChart::setChartTheme
     * @param value
     */
    void Chart::setChartTheme(ChartTheme value);

    /**
     * @brief xScaleDrawType
     * @return
     */
    ScaleDrawType xScaleDrawType() const;

    /**
     * @brief setXScaleDrawType
     * @param type
     */
    virtual void setXScaleDrawType(ScaleDrawType type);

    /**
     * @brief identity
     * @return
     */
    QString identity() const;

    /**
     * @brief setIdentity
     * @param identity
     */
    void setIdentity(const QString &identity);

    /**
     * @brief 获取标题
     * @return 标题
     */
    QString title() const;

    /**
     * @brief 设置标题
     * @param text : 标题
     */
    void setTitle(const QString &value);

    /**
     * @brief zoomIn
     */
    void zoomIn();

    /**
     * @brief zoomOut
     */
    void zoomOut();

    /**
     * @brief zoomIn
     * @param rect
     */
    void zoomIn(const QRectF &rect);

    /**
     * @brief zoom
     * @param factor
     */
    void zoom(qreal factor);

    /**
     * @brief zoomReset
     */
    virtual void zoomReset();

    /**
     * @brief clearMarker
     */
    void clearMarker();

    /**
     * @brief isZoomed
     * @return
     */
    bool isZoomed();

    /**
     * @brief scroll
     * @param dx
     * @param dy
     */
    void scroll(qreal dx, qreal dy);

    /**
     * @brief axisX
     * @return
     */
    Axis axisX() const;

    /**
     * @brief setAxisX
     * @param axis
     */
    virtual void setAxisX(Axis axis);

    /**
     * @brief addAxis
     * @param axis
     * @param alignment
     */
    void addAxis(AbstractAxis *axis, Qt::Alignment alignment);

    /**
     * @brief removeAxis
     * @param axis
     */
    void removeAxis(AbstractAxis *axis);

    /**
     * @brief axes
     * @param orientations
     * @param series
     * @return
     */
    QList<AbstractAxis *> axes(Qt::Orientations orientations = Qt::Horizontal | Qt::Vertical,
                                AbstractSeries *series = 0) const;

    /**
     * @brief createDefaultAxes
     */
    void createDefaultAxes();

    /**
     * @brief setAxisVisible
     * @param axisId
     * @param visible
     */
    void setAxisVisible(int axisId, bool visible);

    /**
     * @brief setAxisEnabled
     * @param axisId
     * @param enabled
     */
    void setAxisEnabled(int axisId, bool enabled);

    /**
     * @brief setAxisAlign
     * @param axisId
     * @param align
     */
    virtual void setAxisAlign(int axisId, bool align);

    /**
     * @brief setAxisLabelLength
     * @param axisId
     * @param length
     */
    virtual void setAxisLabelLength(int axisId, int length);

    /**
     * @brief setGridEnabled
     * @param orientations
     * @param enabled
     */
    void setGridEnabled(Qt::Orientations orientations, bool enabled);

    /**
     * @brief setGridPen
     * @param pen
     */
    void setGridPen(const QPen &pen);

    /**
     * @brief zoomBase
     * @return
     */
    QRectF zoomBase() const;

    /**
     * @brief setZoomBase
     * @param rect
     */
    void setZoomBase(const QRectF &rect);

    /**
     * @brief setZoomBaseY
     * @param yMin
     * @param yMax
     */
    void setZoomBaseY(qreal yMin, qreal yMax);

    /**
     * @brief setZoomBase
     * @param replot
     */
    void setZoomBase(bool replot = true);

    /**
     * @brief zoomRect
     * @return
     */
    QRectF zoomRect() const;

    /**
     * @brief zoomRectIndex
     * @return
     */
    uint zoomRectIndex() const;

    /**
     * @brief resetZoomer
     */
    void resetZoomer();

    /**
     * @brief shiftType
     * @return
     */
    ShiftType shiftType() const;

    /**
     * @brief setShiftType
     * @param type
     */
    virtual void setShiftType(ShiftType type);

    /**
     * @brief defaultShiftCount
     * @return
     */
    int defaultShiftCount() const;

    /**
     * @brief setDefaultShiftCount
     * @param count
     */
    virtual void setDefaultShiftCount(int count);

    /**
     * @brief shiftCount
     * @return
     */
    int shiftCount() const;

    /**
     * @brief setShiftCount
     * @param count
     */
    virtual void setShiftCount(int count);

    /**
     * @brief defaultShiftTimeZone
     * @return
     */
    qreal defaultShiftTimeZone() const;

    /**
     * @brief setDefaultShiftTimeZone
     * @param msecs
     */
    virtual void setDefaultShiftTimeZone(qreal msecs);

    /**
     * @brief shiftTimeZone
     * @return
     */
    qreal shiftTimeZone() const;

    /**
     * @brief setShiftTimeZone
     * @param zone
     */
    virtual void setShiftTimeZone(qreal msecs);

    /**
     * @brief setTitleVisible
     * @param visible
     */
    void setTitleVisible(bool visible);

    /**
     * @brief plot
     * @return
     */
    QwtPlot *plot();

    /**
     * @brief zoomer
     * @return
     */
    JPlotZoomer *zoomer();

    /**
     * @brief resizeSeries
     * @param size
     */
    void resizeSeries(int size);

    /**
     * @brief addSeries
     * @param id
     * @param title
     * @return
     */
    virtual AbstractSeries *addSeries(const QString &id, const QString &title);

    /**
     * @brief addSeries
     * @param id
     * @param pen
     * @param title
     * @return
     */
    virtual AbstractSeries *addSeries(const QString &id, const QPen &pen,
                                       const QString &title = QString::null);

    /**
     * @brief addSeries
     * @param series
     * @return
     */
    virtual AbstractSeries *addSeries(AbstractSeries *series);

    /**
     * @brief removeSeries
     * @param index
     */
    virtual void removeSeries(int index);

    /**
     * @brief removeSeries
     * @param series
     */
    virtual void removeSeries(AbstractSeries *series);

    /**
     * @brief clearSeries
     */
    virtual void clearSeries();

    /**
     * @brief setSeries
     * @param index
     * @param series
     */
    virtual void setSeries(int index, AbstractSeries *series);

    /**
     * @brief series
     * @return
     */
    QVector<AbstractSeries *> series();

    /**
     * @brief series
     * @return
     */
    const QVector<AbstractSeries *> &series() const;

    /**
     * @brief seriesCount
     * @return
     */
    int seriesCount() const;

    /**
     * @brief seriesAt
     * @param index
     * @return
     */
    AbstractSeries *seriesAt(int index) const;

    /**
     * @brief seriesAt
     * @param identity
     * @return
     */
    AbstractSeries *seriesAt(const QString &identity) const;

    /**
     * @brief indexOfSeries
     * @param series
     * @return
     */
    int indexOfSeries(AbstractSeries *series) const;

    /**
     * @brief setSeriesPen
     * @param index
     * @param pen
     */
    void setSeriesPen(int index, const QPen &pen);

    /**
     * @brief setSeriesTitle
     * @param index
     * @param text
     */
    void setSeriesTitle(int index, const QString &text);

    /**
     * @brief updateSeries
     * @param index
     */
    void updateSeries(int index);

    /**
     * @brief updateScale
     * @param axisId
     */
    virtual void updateScale(JChart::Axis axisId);

    /**
     * @brief updateScaleDraw
     * @param axisId
     * @param align
     * @param labelLength
     */
    virtual void updateScaleDraw(JChart::Axis axisId, bool align, int labelLength);

    /**
     * @brief setAxisScale
     * @param axisId
     * @param minimum
     * @param maximum
     * @param stepSize
     */
    virtual void setAxisScale(int axisId, double minimum, double maximum, double stepSize = 0);

    /**
     * @brief siftArea
     * @return
     */
    QRectF siftArea() const;

    /**
     * @brief setSiftArea
     * @param area
     */
    void setSiftArea(const QRectF &area);

    /**
     * @brief axisStepSize
     * @param axisId
     * @return
     */
    qreal axisStepSize(int axisId) const;

    /**
     * @brief axisInterval
     * @param axisId
     * @return
     */
    QPair<qreal, qreal> axisInterval(Axis axisId) const;

    /**
     * @brief queryColor
     * @param index
     * @return
     */
    QColor queryColor(int index = -1) const;

    /**
     * @brief zoneVisible
     * @return
     */
    bool zoneVisible() const;

    /**
     * @brief setZoneVisible
     * @param visible
     */
    void setZoneVisible(bool visible);

    /**
     * @brief setLegendVisible
     * @param visible
     */
    void setLegendVisible(bool visible);

    /**
     * @brief zoneSlider
     * @return
     */
    ZoneSlider *zoneSlider();

    /**
     * @brief zoneSlider
     * @return
     */
    const ZoneSlider *zoneSlider() const;

    /**
     * @brief replot
     */
    void replot();

    /**
     * @brief isPlay
     * @return
     */
    bool isPlay() const;

    /**
     * @brief userData
     * @return
     */
    QObjectUserData *userData() const;

    /**
     * @brief setUserData
     * @param data
     */
    void setUserData(QObjectUserData *data);

    /**
     * @brief setTrackerPos
     * @param pos
     * @param visible
     */
    void setTrackerPos(const QPointF &pos, bool visible);

    /**
     * @brief markTrackerPos
     * @param pos
     */
    void markTrackerPos(const QPointF &pos);

signals:
    void axisIntervalChanged(int axisId, qreal minimum, qreal maximum);
    void scaleDivChanged(int axisId, qreal minimum, qreal maximum);
    void triggledPlay(bool checked);
    void seriesRemoved(int index);
    void triggledClose();
    void trackerChanged(const QPointF &pos, bool visible);
    void trackerMarked(const QPointF &pos);
    void defShiftCountChanged(int count);
    void defaultShiftTimeZoneChanged(qreal value);

public slots:
    void togglePlay(bool checked);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Q_DISABLE_COPY(Chart)
    J_DECLARE_PRIVATE(Chart)
};

} // end of namespace JChart

#endif // JCHART_H
