#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <QSpinBox>

class hexSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    hexSpinBox(QWidget *parent);
    ~hexSpinBox();

protected:
    virtual QString textFromValue(int val) const override;

private:
    
};

#endif // HEXSPINBOX_H
