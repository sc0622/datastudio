#ifndef ICDWORKER_RECV_P_H
#define ICDWORKER_RECV_P_H

#include <QMutex>
#include "../icdworker_recv.h"
#include "../jrttimer.h"
#include "icdcomm/icdcomm_channel.h"
#include "icdcore/icd_table.h"
#include "../icdworker.h"
#include <QFile>
#include <QQueue>

namespace Icd {

class WorkerRecvPrivate : public Icd::Runnable
{
public:
    explicit WorkerRecvPrivate(WorkerRecv *q);
    ~WorkerRecvPrivate();

    void init();
    void updateBind(bool valid);
    void recvData();

    //
    bool isRunning() const;
    bool start();
    void stop();
    int interval() const;
    void setInterval(int value);
    WorkerTrans::TimeEvent timeEvent() const;
    void setTimeEvent(WorkerTrans::TimeEvent event);

    void clearQueue();

    bool startRecord();
    void stopRecord();

    // Icd::Runnable interface
protected:
    void run() override;

private:
    Q_DISABLE_COPY(WorkerRecvPrivate)
    J_DECLARE_PUBLIC(WorkerRecv)
    QMutex mutex;
    JRTTimerPtr timer;
    Icd::ChannelPtr channel;
    Icd::TablePtr table;
    Icd::WorkerPtr relayer;
    QQueue<char*> bufferQueue;
    int tableSize;
    char* tableBuffer;
    char* parseBuffer;
    int currentIndex;
    int queueCount;
    QFile recordFile;
};

} // end of namespace Icd

#endif // ICDWORKER_RECV_P_H
