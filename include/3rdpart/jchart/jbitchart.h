#ifndef JBIT_CHART_H
#define JBIT_CHART_H

#include "jchart_global.h"
#include "jchart.h"
#include "jbitseries.h"

namespace JChart {

class BitChartPrivate;

/**
 * @brief The BitChart class
 */
class JCHART_EXPORT BitChart : public Chart
{
    Q_OBJECT
public:

    /**
     * @brief BitChart
     * @param parent
     */
    explicit BitChart(QWidget *parent = 0);

    virtual ~BitChart();

    /**
     * @brief setXScaleDrawType
     * @param type
     */
    void setXScaleDrawType(ScaleDrawType type);

    /**
     * @brief setAxisX
     * @param axis
     */
    void setAxisX(Axis axis);

    /**
     * @brief setAxisAlign
     * @param axisId
     * @param align
     */
    void setAxisAlign(int axisId, bool align) override;

    /**
     * @brief setAxisLabelLength
     * @param axisId
     * @param length
     */
    void setAxisLabelLength(int axisId, int length) override;

    /**
     * @brief zoomReset
     */
    void zoomReset();

    /**
     * @brief bitsMask
     * @return
     */
    quint32 bitsMask() const;

    /**
     * @brief setBitsMask
     * @param mask
     */
    void setBitsMask(quint32 mask);

    /**
     * @brief setBitsRange
     * @param begin
     * @param end
     */
    void setBitsRange(int begin, int end);

    /**
     * @brief setBitMask
     * @param offset
     * @param on
     */
    void setBitMask(int offset, bool on);

    /**
     * @brief testBitMask
     * @param offset
     * @return
     */
    bool testBitMask(int offset) const;

    /**
     * @brief setBitsTitle
     * @param texts
     */
    void setBitsTitle(const QStringList &texts);

    /**
     * @brief samples
     * @return
     */
    const QVector<QPointF> &samples() const;

    /**
     * @brief setSamples
     * @param data
     */
    void setSamples(const QVector<QPointF> &data);

    /**
     * @brief appendSample
     * @param point
     */
    void appendSample(const QPointF &point);

    /**
     * @brief leftShiftSample
     * @param point
     */
    void shiftSample(const QPointF &point);

    /**
     * @brief setShiftType
     * @param type
     */
    void setShiftType(ShiftType type);

    /**
     * @brief setShiftCount
     * @param value
     */
    void setShiftCount(int value);

    /**
     * @brief setShiftTimeZone
     * @param zone
     */
    void setShiftTimeZone(qreal msecs);

    /**
     * @brief addSeries
     * @param series
     * @return
     */
    AbstractSeries *addSeries(AbstractSeries *series);

    /**
     * @brief removeSeries
     * @param index
     */
    void removeSeries(int index);

    /**
     * @brief removeSeries
     * @param series
     */
    void removeSeries(AbstractSeries *series);

    /**
     * @brief clearSeries
     */
    void clearSeries();

    /**
     * @brief updateScale
     * @param axisId
     */
    void updateScale(JChart::Axis axisId);

    /**
     * @brief updateScaleDraw
     * @param axisId
     * @param align
     */
    void updateScaleDraw(JChart::Axis axisId, bool align);

signals:

public slots:

private:
    Q_DISABLE_COPY(BitChart)
    J_DECLARE_PRIVATE(BitChart)
};

} // end of namespace JChart

#endif // JBIT_CHART_H
