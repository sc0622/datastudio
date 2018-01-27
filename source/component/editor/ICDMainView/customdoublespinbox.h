#ifndef CUSTOMDOUBLESPINBOX_H
#define CUSTOMDOUBLESPINBOX_H

#include <QDoubleSpinBox>

class CustomDoubleSpinbox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    CustomDoubleSpinbox(QWidget *parent);
    ~CustomDoubleSpinbox();

    virtual QString textFromValue(double val) const;

protected:
    virtual QValidator::State validate(QString &input, int &pos) const override;

private:
    
};

#endif // CUSTOMDOUBLESPINBOX_H
