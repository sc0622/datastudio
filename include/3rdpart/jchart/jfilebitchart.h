#ifndef JFILEBIT_CHART_H
#define JFILEBIT_CHART_H

#include "jchart_global.h"
#include "jchart.h"
#include "jbitseries.h"

namespace JChart {

class FileBitChartPrivate;

/**
 * @brief The FileBitChart class
 */
class JCHART_EXPORT FileBitChart : public Chart
{
    Q_OBJECT
public:

    /**
     * @brief FileBitChart
     * @param parent
     */
    explicit FileBitChart(QWidget *parent = 0);

    virtual ~FileBitChart();

    /**
     * @brief setXScaleDrawType
     * @param type
     */
    void setXScaleDrawType(ScaleDrawType type);

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
     * @brief setBuffer
     * @param buffer
     * @param bufferSize
     * @param bufferStep
     */
    void setBuffer(const uchar *buffer, size_t bufferSize, int bufferStep);

    /**
     * @brief setAttributes
     * @param xSize
     * @param ySize
     * @param xOffset
     * @param yOffset
     */
    void setAttributes(uchar xSize, uchar ySize, int xOffset, int yOffset);

    /**
     * @brief axisMinValue
     * @param axisId
     * @return
     */
    qreal axisMinValue(int axisId) const;

    /**
     * @brief axisMaxValue
     * @param axisId
     * @return
     */
    qreal axisMaxValue(int axisId) const;

signals:

public slots:

private:
    Q_DISABLE_COPY(FileBitChart)
    J_DECLARE_PRIVATE(FileBitChart)
};

} // end of namespace JChart

#endif // JFILEBIT_CHART_H
