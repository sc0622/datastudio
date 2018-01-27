#ifndef JZONESLIDER_H
#define JZONESLIDER_H

#include "jchart_global.h"
#include <QWidget>

class QwtPlot;

namespace JChart {

// - class JAreaSift -

class ZoneSliderPrivate;

class JCHART_EXPORT ZoneSlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
public:
    explicit ZoneSlider(QWidget *parent = 0);
    ~ZoneSlider();

    qreal minimum() const;
    qreal maximum() const;
    bool snapEnabled() const;

    void attach(QwtPlot *plot);
    void detach();
    void updateZone();

Q_SIGNALS:
    void minimumChanged(qreal value);
    void maximumChanged(qreal value);
    void rangeChanged(qreal minimum, qreal maximum);

public Q_SLOTS:
    void setMinimum(qreal value);
    void setMaximum(qreal value);
    void setRange(qreal minimum, qreal maximum);
    void setSnapEnabled(bool enabled);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Q_DISABLE_COPY(ZoneSlider)
    J_DECLARE_PRIVATE(ZoneSlider)
};

} // end of namespace JChart

#endif // JZONESLIDER_H
