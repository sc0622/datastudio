#ifndef JBIT_SERIES_H
#define JBIT_SERIES_H

#include "jchart_global.h"
#include "jabstractseries.h"

namespace JChart {

class BitSeriesPrivate;
class JBitChart;

/**
 * @brief The BitSeries class
 */
class JCHART_EXPORT BitSeries : public AbstractSeries
{
public:
    /**
     * @brief BitSeries
     * @param yOffset
     * @param maskOffset
     * @param data
     * @param title
     */
    explicit BitSeries(int yOffset, int maskOffset, QVector<QPointF> &data,
                        const QString &title = QString::null);

    virtual ~BitSeries();

    /**
     * @brief yOffset
     * @return
     */
    int yOffset() const;

    /**
     * @brief maskOffset
     * @return
     */
    int maskOffset() const;

    /**
     * @brief attach
     * @param chart
     */
    void attach(Chart *chart);

    /**
     * @brief detach
     */
    void detach();

    /**
     * @brief title
     * @return
     */
    QString title() const;

    /**
     * @brief setTitle
     * @param text
     */
    void setTitle(const QString &text);

private:
    Q_DISABLE_COPY(BitSeries)
    J_DECLARE_PRIVATE(BitSeries)
};

} // end of namespace JChart

#endif // JBIT_SERIES_H
