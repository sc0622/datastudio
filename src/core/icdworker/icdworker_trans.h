#ifndef ICDWORKER_TRANS_H
#define ICDWORKER_TRANS_H

#include "icdcore/icd_table.h"
#include "icdcomm/icdcomm_channel.h"

namespace Icd {

class WorkerTrans;
typedef JHandlePtr<WorkerTrans> WorkerTransPtr;

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
     * @brief ��ȡ��ʱ����
     * @return ��ʱ���ڡ���λ��ms
     */
    virtual int interval() const = 0;

    /**
     * @brief ���ö�ʱ����
     * @param value : ��ʱ���ڡ���λ��ms
     */
    virtual void setInterval(int value) = 0;

    /**
     * @brief ��ȡ��ʱ�������¼�����
     * @return ��ʱ�������¼�����
     */
    virtual TimeEvent timeEvent() const = 0;

    /**
     * @brief ���ö�ʱ�������¼�����
     * @param value : ��ʱ�������¼�����
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
