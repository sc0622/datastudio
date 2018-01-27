#ifndef JFILENUMERIC_CHART_H
#define JFILENUMERIC_CHART_H

#include "jchart_global.h"
#include "jchart.h"
#include "jbitseries.h"

namespace JChart {

class FileNumericChartPrivate;

/**
 * @brief The FileNumericChart class
 */
class JCHART_EXPORT FileNumericChart : public Chart
{
    Q_OBJECT
public:
    /**
     * @brief FileNumericChart
     * @param parent
     */
    explicit FileNumericChart(QWidget *parent = 0);

    virtual ~FileNumericChart();

    /**
     * @brief setXScaleDrawType
     * @param type
     */
    void setXScaleDrawType(ScaleDrawType type);

    /**
     * @brief zoomReset
     */
    void zoomReset();

    /**
     * @brief addSeries
     * @param id
     * @param title
     * @return
     */
    AbstractSeries *addSeries(const QString &id, const QString &title);

    /**
     * @brief addSeries
     * @param id
     * @param pen
     * @param title
     * @return
     */
    AbstractSeries *addSeries(const QString &id, const QPen &pen,
                               const QString &title = QString::null);

    /**
     * @brief updateScale
     * @param axisId
     */
    void updateScale(JChart::Axis axisId);

    /**
     * @brief xRange
     * @return
     */
    QPair<qreal, qreal> xRange() const;

    /**
     * @brief setBuffer
     * @param buffer
     * @param bufferSize
     * @param bufferStep
     */
    void setBuffer(const uchar *buffer, size_t bufferSize, int bufferStep);

    /**
     * @brief updateScaleDraw
     * @param axisId
     * @param align
     * @param labelLength
     */
    void updateScaleDraw(JChart::Axis axisId, bool align, int labelLength);

    /**
     * @brief setAxisAlign
     * @param axisId
     * @param align
     */
    void setAxisAlign(int axisId, bool align);

    /**
     * @brief setAxisLabelLength
     * @param axisId
     * @param length
     */
    void setAxisLabelLength(int axisId, int length) override;

signals:
    /**
     * @brief zoneAreaChanged
     * @param range
     */
    void zoneAreaChanged(const QRectF &range);

public slots:

private:
    Q_DISABLE_COPY(FileNumericChart)
    J_DECLARE_PRIVATE(FileNumericChart)
};

} // end of namespace JChart

#endif // JNUMERIC_CHART_H
