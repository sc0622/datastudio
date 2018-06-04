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
    /**
     * @brief The TimeEvent enum
     */
    enum TimeEvent {
        TimeOneShot,
        TimePeriodic
    };

    virtual ~WorkerTrans() {}

    /**
     * @brief channel
     * @return
     */
    virtual Icd::ChannelPtr channel() const = 0;

    /**
     * @brief setChannel
     * @param channel
     */
    virtual void setChannel(const Icd::ChannelPtr &channel) = 0;

    /**
     * @brief table
     * @return
     */
    virtual Icd::TablePtr table() const = 0;

    /**
     * @brief setTable
     * @param table
     */
    virtual void setTable(const Icd::TablePtr &table) = 0;

    /**
     * @brief 获取定时周期
     * @return 定时周期。单位：ms
     */
    virtual int interval() const = 0;

    /**
     * @brief 设置定时周期
     * @param value : 定时周期。单位：ms
     */
    virtual void setInterval(int value) = 0;

    /**
     * @brief 获取定时器触发事件类型
     * @return 定时器触发事件类型
     */
    virtual TimeEvent timeEvent() const = 0;

    /**
     * @brief 设置定时器触发事件类型
     * @param value : 定时器触发事件类型
     */
    virtual void setTimeEvent(TimeEvent value) = 0;

    /**
     * @brief isRunning
     * @return
     */
    virtual bool isRunning() const = 0;

    /**
     * @brief start
     * @return
     */
    virtual bool start() = 0;

    /**
     * @brief stop
     */
    virtual void stop() = 0;
};

} // end of namespace Icd

#endif // ICDWORKER_TRANS_H
