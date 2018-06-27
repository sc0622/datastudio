#include "precomp.h"
#include "JDoubleSlider.h"

// class JDoubleSliderPrivate

class JDoubleSliderPrivate
{
public:
    JDoubleSliderPrivate(JDoubleSlider *q)
        : J_QPTR(q)
        , decimals(2)
    {

    }

    void init();
    quint64 multipler() const;

private:
    J_DECLARE_PUBLIC(JDoubleSlider)
    int decimals;
};

void JDoubleSliderPrivate::init()
{
    Q_Q(JDoubleSlider);
    QObject::connect(q, SIGNAL(valueChanged(int)), q, SLOT(setValue(int)));
    q->setSingleStep(multipler());
    q->setFocusPolicy(Qt::NoFocus);
}

quint64 JDoubleSliderPrivate::multipler() const
{
    return quint64(qPow(10, decimals));
}

// class JDoubleSlider

JDoubleSlider::JDoubleSlider(QWidget *parent)
    : QSlider(parent)
    , J_DPTR(new JDoubleSliderPrivate(this))
{
    Q_D(JDoubleSlider);
    d->init();
}

JDoubleSlider::JDoubleSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
    , J_DPTR(new JDoubleSliderPrivate(this))
{
    Q_D(JDoubleSlider);
    d->init();
}

double JDoubleSlider::singleStep() const
{
    Q_D(const JDoubleSlider);
    return QSlider::singleStep() / d->multipler();
}

void JDoubleSlider::setSingleStep(double value)
{
    Q_D(JDoubleSlider);
    QSlider::setSingleStep(value * d->multipler());
}

double JDoubleSlider::minimum() const
{
    Q_D(const JDoubleSlider);
    return QSlider::minimum() / d->multipler();
}

void JDoubleSlider::setMinimum(double minimum)
{
    Q_D(JDoubleSlider);
    QSlider::setMinimum(minimum * d->multipler());
}

double JDoubleSlider::maximum() const
{
    Q_D(const JDoubleSlider);
    return QSlider::maximum() / d->multipler();
}

void JDoubleSlider::setMaximum(double maximum)
{
    Q_D(JDoubleSlider);
    QSlider::setMaximum(maximum * d->multipler());
}

void JDoubleSlider::setRange(double minimum, double maximum)
{
    Q_D(JDoubleSlider);
    QSlider::setRange(minimum * d->multipler(), maximum * d->multipler());
}

int JDoubleSlider::decimals() const
{
    Q_D(const JDoubleSlider);
    return d->decimals;
}

void JDoubleSlider::setDecimals(int prec)
{
    Q_D(JDoubleSlider);
    d->decimals = prec;
}

double JDoubleSlider::value() const
{
    Q_D(const JDoubleSlider);
    int value = QSlider::value();
    return (double)value / d->multipler();
}

void JDoubleSlider::setValue(double value, bool blockSignals)
{
    Q_D(JDoubleSlider);
    QSlider::blockSignals(blockSignals);
    QSlider::setValue(value * d->multipler());

    if (!blockSignals) {
        emit valueChanged(value);
    }

    QSlider::blockSignals(false);
}

void JDoubleSlider::setValue(int value)
{
    Q_D(JDoubleSlider);
    emit valueChanged((double)value / d->multipler());
}
