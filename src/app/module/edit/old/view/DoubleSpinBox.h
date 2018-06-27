#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinBox(QWidget *parent = nullptr);

    QString textFromValue(double value) const override;
    QValidator::State validate(QString &input, int &pos) const override;
};

#endif // DOUBLESPINBOX_H
