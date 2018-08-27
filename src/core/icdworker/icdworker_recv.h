#ifndef ICDWORKER_RECV_H
#define ICDWORKER_RECV_H

#include <QObject>
#include "icdworker_global.h"
#include "icdworker_trans.h"

namespace Icd {

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

class WorkerRecvPrivate;
class WorkerRecv;
typedef std::shared_ptr<WorkerRecv> WorkerRecvPtr;

class ICDWORKER_EXPORT WorkerRecv : public QObject, public WorkerTrans
{
    Q_OBJECT
public:
    explicit WorkerRecv(QObject *parent = nullptr);
    ~WorkerRecv();

    Icd::ChannelPtr channel() const;
    void setChannel(const Icd::ChannelPtr &channel);

    Icd::TablePtr table() const;
    void setTable(const Icd::TablePtr &table);

    Icd::WorkerPtr relayer() const;
    void setRelayer(const Icd::WorkerPtr &relayer);

    int interval() const;
    void setInterval(int value);

    TimeEvent timeEvent() const;
    void setTimeEvent(TimeEvent value);

    bool isRunning() const;
    bool start();
    void stop();
    int queueCount() const;
    bool startRecord();
    void stopRecord();
    bool isRecording() const;

signals:
    void channelChanged(Icd::ChannelPtr &channel);
    void tableChanged(const Icd::TablePtr &table);
    void relayerChanged();
    void closed();
    void started();
    void stopped();
    void toggled(int isOpen);
    void intervalChanged(int value);
    void timeEventChanged(TimeEvent event);
    void queueCountChanged(int count);

public slots:
    void setQueueCount(int count);

private:
    Q_DISABLE_COPY(WorkerRecv)
    J_DECLARE_PRIVATE(WorkerRecv)
};

} // end of namespace Icd

#endif // ICDWORKER_RECV_H
