#ifndef UDPDETAILWIDGET_H
#define UDPDETAILWIDGET_H

#include <QWidget>
#include "icdworker/icdworker.h"

class UdpSettingWidget;
class UdpSendWidget;
class UdpRecvWidget;

class UdpDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UdpDetailWidget(QWidget *parent = 0);
    ~UdpDetailWidget();

    void updateUi(const Icd::WorkerPtr &worker);

signals:

public slots:
    void onWorkerRemoved(const Icd::WorkerPtr &worker);
    void onWorkerCleared();

private:
    Icd::WorkerPtr d_worker;
    UdpSettingWidget *d_settingWidget;
    UdpSendWidget *d_sendWidget;
    UdpRecvWidget *d_recvWidget;
};

#endif // UDPDETAILWIDGET_H
