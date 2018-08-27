#ifndef ICDWORKER_SEND_P_H
#define ICDWORKER_SEND_P_H

#include <QMutex>
#include "../icdworker_send.h"
#include "../jrttimer.h"
#include "icdcomm/icdcomm_channel.h"
#include "icdcore/icd_table.h"
#include "../icdworker.h"

namespace Icd {

class WorkerSendPrivate : public Icd::Runnable
{
public:
    explicit WorkerSendPrivate(WorkerSend *q);
    ~WorkerSendPrivate();

    void init();
    void updateBind(bool valid);
    bool sendData(bool counterLoop = false, bool frameLoop = false);
    void doFrame(bool loop);
    void doCounter();
    bool doCheck();

    //
    bool isRunning() const;
    bool start();
    void stop();
    int interval() const;
    void setInterval(int value);
    WorkerTrans::TimeEvent timeEvent() const;
    void setTimeEvent(WorkerTrans::TimeEvent event);

    // Icd::Runnable interface
protected:
    void run();

private:
    J_DECLARE_PUBLIC(WorkerSend)
    QMutex mutex;
    JRTTimerPtr timer;
    Icd::ChannelPtr channel;
    Icd::TablePtr table;
    int tableSize;
    char* tableBuffer;
    bool counterLoop;
    bool frameLoop;
    bool running;
};

} // end of namespace Icd

#endif // ICDWORKER_SEND_P_H
