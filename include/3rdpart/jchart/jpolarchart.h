#ifndef JPOLARCHART_H
#define JPOLARCHART_H

#include "jchart_global.h"
#include "jchart.h"

namespace JChart {

class PolarChartPrivate;

/**
 * @brief The PolarChart class
 */
class JCHART_EXPORT PolarChart : public Chart
{
    Q_OBJECT
public:
    /**
     * @brief PolarChart
     * @param parent
     * @param f
     */
    explicit PolarChart(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    ~PolarChart();

signals:

public slots:

private:
    Q_DISABLE_COPY(PolarChart)
    J_DECLARE_PRIVATE(PolarChart)
};

} // end of namespace JChart

#endif // JPOLARCHART_H
