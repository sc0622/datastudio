#ifndef SERIALRECVWIDGET_H
#define SERIALRECVWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class QSpinBox;
class QPushButton;
class QLineEdit;

class SerialRecvWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialRecvWidget(QWidget *parent = nullptr);
    ~SerialRecvWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:

private:
    Icd::WorkerPtr d_worker;
    QSpinBox *d_spinBoxInterval;
    QPushButton *d_buttonApply;
    QPushButton *d_buttonRestore;
    QLineEdit *d_editSequence;
};

#endif // SERIALRECVWIDGET_H
