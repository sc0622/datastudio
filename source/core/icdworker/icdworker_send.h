#ifndef ICDWORKER_SEND_H
#define ICDWORKER_SEND_H

#include <QObject>
#include "icdworker_global.h"
#include "icdworker_trans.h"

namespace Icd {

class WorkerSendPrivate;
class WorkerSend;
typedef JHandlePtr<WorkerSend> WorkerSendPtr;

/**
 * @brief The WorkerSend class
 */
class ICDWORKER_EXPORT WorkerSend : public QObject, public WorkerTrans
{
    Q_OBJECT
public:
    /**
     * @brief WorkerSend
     * @param parent
     */
    explicit WorkerSend(QObject *parent = 0);

    ~WorkerSend();

    /**
     * @brief channel
     * @return
     */
    Icd::ChannelPtr channel() const;

    /**
     * @brief setChannel
     * @param channel
     */
    void setChannel(const Icd::ChannelPtr &channel);

    /**
     * @brief table
     * @return
     */
    Icd::TablePtr table() const;

    /**
     * @brief setTable
     * @param table
     */
    void setTable(const Icd::TablePtr &table);

    /**
     * @brief 获取定时周期
     * @return 定时周期。单位：ms
     */
    int interval() const;

    /**
     * @brief 设置定时周期
     * @param value : 定时周期。单位：ms
     */
    void setInterval(int value);

    /**
     * @brief 获取定时器触发事件类型
     * @return 定时器触发事件类型
     */
    TimeEvent timeEvent() const;

    /**
     * @brief 设置定时器触发事件类型
     * @param value : 定时器触发事件类型
     */
    void setTimeEvent(TimeEvent value);

    /**
     * @brief counterLoop
     * @return
     */
    bool counterLoop() const;

    /**
     * @brief setCounterLoop
     * @param loop
     */
    void setCounterLoop(bool loop);

    /**
     * @brief frameLoop
     * @return
     */
    bool frameLoop() const;

    /**
     * @brief setFrameLoop
     * @param loop
     */
    void setFrameLoop(bool loop);

    /**
     * @brief isRunning
     * @return
     */
    bool isRunning() const;

    /**
     * @brief start
     * @return
     */
    bool start();

    /**
     * @brief stop
     */
    void stop();

    /**
     * @brief sendOnce
     * @param counterLoop
     * @param frameLoop
     * @return
     */
    bool sendOnce(bool counterLoop = false, bool frameLoop = false);

    /**
     * @brief sendOnce
     * @param data
     * @param size
     * @return
     */
    bool sendOnce(const char *data, int size);

signals:
    void channelChanged(Icd::ChannelPtr &channel);
    void tableChanged(const Icd::TablePtr &table);
    void closed();
    void started();
    void stopped();
    void toggled(int isRunning);
    void intervalChanged(unsigned int value);
    void timeEventChanged(TimeEvent event);
    void counterLoopChanged(bool loop);
    void frameLoopChanged(bool loop);

public slots:

private:
    Q_DISABLE_COPY(WorkerSend)
    J_DECLARE_PRIVATE(WorkerSend)
};

} // end of namespace Icd

#endif // ICDWORKER_SEND_H
