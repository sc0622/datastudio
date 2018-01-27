#ifndef JNUMERIC_SERIES_H
#define JNUMERIC_SERIES_H

#include "jchart_global.h"
#include "jabstractseries.h"

namespace JChart {

class NumericSeriesPrivate;
class JNumericChart;

/**
 * @brief The NumericSeries class
 */
class JCHART_EXPORT NumericSeries : public AbstractSeries
{
public:
    /**
     * @brief NumericSeries
     * @param title
     */
    explicit NumericSeries(const QString &title = QString::null);

    virtual ~NumericSeries();

    /**
     * @brief setBrush
     * @param brush
     */
    virtual void setBrush(const QBrush &brush);

    /**
     * @brief setPen
     * @param pen
     */
    virtual void setPen(const QPen &pen);

private:
    Q_DISABLE_COPY(NumericSeries)
    J_DECLARE_PRIVATE(NumericSeries)
};

} // end of namespace JChart

#endif // JNUMERIC_SERIES_H
