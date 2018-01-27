#ifndef JNUMERIC_CHART_H
#define JNUMERIC_CHART_H

#include "jchart_global.h"
#include "jchart.h"
#include "jbitseries.h"

namespace JChart {

class NumericChartPrivate;

/**
 * @brief The NumericChart class
 */
class JCHART_EXPORT NumericChart : public Chart
{
    Q_OBJECT
public:
    /**
     * @brief NumericChart
     * @param parent
     */
    explicit NumericChart(QWidget *parent = 0);

    virtual ~NumericChart();

    /**
     * @brief setXScaleDrawType
     * @param type
     */
    void setXScaleDrawType(ScaleDrawType type);

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
     * @param axis
     */
    void updateScale(JChart::Axis axis);

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
    void setAxisAlign(int axisId, bool align) override;

    /**
     * @brief setAxisLabelLength
     * @param axisId
     * @param length
     */
    void setAxisLabelLength(int axisId, int length) override;

signals:

public slots:

private:
    Q_DISABLE_COPY(NumericChart)
    J_DECLARE_PRIVATE(NumericChart)
};

} // end of namespace JChart

#endif // JNUMERIC_CHART_H
