#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

class DoubleSpinbox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinbox(QWidget *parent = nullptr);

    virtual QString textFromValue(double val) const;

protected:
    virtual QValidator::State validate(QString &input, int &pos) const override;
};

#endif // DOUBLESPINBOX_H
