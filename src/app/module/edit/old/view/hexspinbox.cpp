#include "precomp.h"
#include "hexspinbox.h"

HexSpinBox::HexSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
    setDisplayIntegerBase(16);
    setPrefix("0x");
}

QString HexSpinBox::textFromValue(int val) const
{
    int maxValue = maximum();
    int count = QString::number(maxValue, 16).size();
    QString result = QSpinBox::textFromValue(val);
    if (result.size() < count) {
        result.prepend(QString(count - result.size(), QChar('0')));
    }

    return result.toUpper();
}
