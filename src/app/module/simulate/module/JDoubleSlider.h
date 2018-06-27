#ifndef JDOUBLESLIDER_H
#define JDOUBLESLIDER_H

#include <QSlider>
#include "main_global.h"

class JDoubleSliderPrivate;

class JDoubleSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
public:
    explicit JDoubleSlider(QWidget *parent = nullptr);
    explicit JDoubleSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

    double singleStep() const;
    void setSingleStep(double value);

    double minimum() const;
    void setMinimum(double minimum);

    double maximum() const;
    void setMaximum(double maximum);

    void setRange(double minimum, double maximum);

    int decimals() const;
    void setDecimals(int prec);

    double value() const;

signals:
    void valueChanged(double value);

public slots:
    void setValue(double value, bool blockSignals = false);

private slots:
    void setValue(int value);

private:
    Q_DISABLE_COPY(JDoubleSlider)
    J_DECLARE_PRIVATE(JDoubleSlider)
};

#endif // JDOUBLESLIDER_H
