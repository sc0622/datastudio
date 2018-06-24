#include "precomp.h"
#include "DoubleSpinBox.h"

DoubleSpinBox::DoubleSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{

}

QString DoubleSpinBox::textFromValue(double value) const
{
    QString text = QDoubleSpinBox::textFromValue(value);
    int count = text.count();
    while (--count) {
        if (text[count] != '0') {
            text = text.left(text[count] == '.' ? count : count - 1);
            break;
        }
    }

    return text;
}
