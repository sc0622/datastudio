#ifndef ICDWORKER_TRANS_H
#define ICDWORKER_TRANS_H

#include "icdcore/icd_table.h"
#include "icdcomm/icdcomm_channel.h"

namespace Icd {

class WorkerTrans;
typedef std::shared_ptr<WorkerTrans> WorkerTransPtr;

class WorkerTrans
{
public:
    enum TimeEvent {
        TimeOneShot,
        TimePeriodic
    };

    virtual ~WorkerTrans() {}

    virtual Icd::ChannelPtr channel() const = 0;
    virtual void setChannel(const Icd::ChannelPtr &channel) = 0;

    virtual Icd::TablePtr table() const = 0;
    virtual void setTable(const Icd::TablePtr &table) = 0;

    virtual int interval() const = 0;
    virtual void setInterval(int value) = 0;

    virtual TimeEvent timeEvent() const = 0;
    virtual void setTimeEvent(TimeEvent value) = 0;

    virtual bool isRunning() const = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;
};

} // end of namespace Icd

#endif // ICDWORKER_TRANS_H
