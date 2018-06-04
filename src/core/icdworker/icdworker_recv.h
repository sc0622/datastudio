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

/**
 * @brief The WorkerRecv class
 */
class ICDWORKER_EXPORT WorkerRecv : public QObject, public WorkerTrans
{
    Q_OBJECT
public:
    /**
     * @brief WorkerRecv
     * @param parent
     */
    explicit WorkerRecv(QObject *parent = nullptr);

    ~WorkerRecv();

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
     * @brief relayer
     * @return
     */
    Icd::WorkerPtr relayer() const;

    /**
     * @brief setRelayer
     * @param relayer
     */
    void setRelayer(const Icd::WorkerPtr &relayer);

    /**
     * @brief 
     * @return ¦Ëms
     */
    int interval() const;

    /**
     * @brief 
     * @param value : ¦Ëms
     */
    void setInterval(int value);

    /**
     * @brief 
     * @return 
     */
    TimeEvent timeEvent() const;

    /**
     * @brief 
     * @param value : 
     */
    void setTimeEvent(TimeEvent value);

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
     * @brief queueCount
     * @return
     */
    int queueCount() const;

    /**
     * @brief startRecord
     * @return
     */
    bool startRecord();

    /**
     * @brief stopRecord
     */
    void stopRecord();

    /**
     * @brief isRecording
     * @return
     */
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
