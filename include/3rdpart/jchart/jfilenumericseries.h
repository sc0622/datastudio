#ifndef JFILENUMERIC_SERIES_H
#define JFILENUMERIC_SERIES_H

#include "jchart_global.h"
#include "jabstractseries.h"
#include <QFile>

namespace JChart {

class FileNumericSeriesPrivate;
class JNumericChart;

/**
 * @brief The FileNumericSeries class
 */
class JCHART_EXPORT FileNumericSeries : public AbstractSeries
{
public:
    /**
     * @brief FileNumericSeries
     * @param title
     */
    explicit FileNumericSeries(const QString &title = QString::null);

    virtual ~FileNumericSeries();

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
     * @param valueScale
     * @param valueOffset
     * @param numericTypeY
     * @param bitStart
     * @param bitCount
     */
    void setAttributes(uchar xSize, uchar ySize, int xOffset, int yOffset,
                       qreal valueScale, qreal valueOffset, int numericTypeY,
                       int bitStart = 0, int bitCount = -1);

    /**
     * @brief xMinimum
     * @return
     */
    qreal xMinimum() const;

    /**
     * @brief xMaximum
     * @return
     */
    qreal xMaximum() const;

    /**
     * @brief yMinimum
     * @return
     */
    qreal yMinimum() const;

    /**
     * @brief yMaximum
     * @return
     */
    qreal yMaximum() const;

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
    Q_DISABLE_COPY(FileNumericSeries)
    J_DECLARE_PRIVATE(FileNumericSeries)
};

} // end of namespace JChart

#endif // JFILENUMERIC_SERIES_H
