#ifndef NUMERICUI_H
#define NUMERICUI_H

#include <QtWidgets>
#include "commonui.h"
#include "KernelClass/icdnumericdata.h"

class NumericUI : public CommonUI
{
    Q_OBJECT
public:
    explicit NumericUI(QWidget *parent = nullptr);

    int uiType() const override;

signals:

private slots:

private:

private:
};

#endif // NUMERICUI_H
