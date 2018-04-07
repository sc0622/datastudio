#ifndef SERIALDETAILWIDGET_H
#define SERIALDETAILWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class SerialSettingWidget;
class SerialSendWidget;
class SerialRecvWidget;

class SerialDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialDetailWidget(QWidget *parent = 0);
    ~SerialDetailWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:
    void onWorkerRemoved(const Icd::WorkerPtr &worker);
    void onWorkerCleared();

private:
    Icd::WorkerPtr d_worker;
    SerialSettingWidget *d_settingWidget;
    SerialSendWidget *d_sendWidget;
    SerialRecvWidget *d_recvWidget;
};

#endif // SERIALDETAILWIDGET_H
