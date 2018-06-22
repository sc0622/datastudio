#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <QSpinBox>

class HexSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    explicit HexSpinBox(QWidget *parent = nullptr);

protected:
    virtual QString textFromValue(int val) const override;
};

#endif // HEXSPINBOX_H
