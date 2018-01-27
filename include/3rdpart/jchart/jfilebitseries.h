#ifndef JFILEBIT_SERIES_H
#define JFILEBIT_SERIES_H

#include "jchart_global.h"
#include "jabstractseries.h"

namespace JChart {

class FileBitSeriesPrivate;
class JBitBitChart;

/**
 * @brief The FileBitSeries class
 */
class JCHART_EXPORT FileBitSeries : public AbstractSeries
{
public:
    /**
     * @brief FileBitSeries
     * @param yPos
     * @param maskOffset
     * @param title
     */
    explicit FileBitSeries(int yPos, int maskOffset, const QString &title = QString::null);

    virtual ~FileBitSeries();

    /**
     * @brief yOffset
     * @return
     */
    int yPos() const;

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

private:
    Q_DISABLE_COPY(FileBitSeries)
    J_DECLARE_PRIVATE(FileBitSeries)
};

} // end of namespace JChart

#endif // JFILEBIT_SERIES_H
