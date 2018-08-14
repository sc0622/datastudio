#ifndef UDPRECVWIDGET_H
#define UDPRECVWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class QSpinBox;
class QPushButton;
class QLineEdit;
class UdpRecvWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UdpRecvWidget(QWidget *parent = nullptr);
    ~UdpRecvWidget();

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

#endif // UDPRECVWIDGET_H
