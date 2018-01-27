#ifndef ICDWORKER_H
#define ICDWORKER_H

#include <QObject>
#include "icdworker_global.h"
#include "icdcomm/icdcomm_channel.h"
#include "icdcore/icd_table.h"
#include "icdworker_send.h"
#include "icdworker_recv.h"

namespace Icd {

class Worker;
class WorkerData;
typedef JHandlePtr<Worker> WorkerPtr;
typedef std::vector<WorkerPtr> WorkerPtrArray;

class WorkerPrivate;

/**
 * @brief The Worker class
 */
class ICDWORKER_EXPORT Worker : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief The WorkerType enum
     */
    enum WorkerType {
        WorkerTypeSend,
        WorkerTypeRecv
    };

    /**
     * @brief Worker
     * @param parent
     */
    explicit Worker(QObject *parent = 0);

    /**
     * @brief Worker
     * @param channel
     * @param parent
     */
    explicit Worker(const Icd::ChannelPtr &channel, QObject *parent = 0);

    /**
     * @brief Worker
     * @param channel
     * @param tableSend
     * @param tableRecv
     * @param parent
     */
    explicit Worker(const Icd::ChannelPtr &channel, const Icd::TablePtr &tableSend,
                    const Icd::TablePtr &tableRecv, QObject *parent = 0);

    virtual ~Worker();

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
     * @brief workerSend
     * @return
     */
    Icd::WorkerSendPtr workerSend() const;

    /**
     * @brief workerRecv
     * @return
     */
    Icd::WorkerRecvPtr workerRecv() const;

    /**
     * @brief isRunning
     * @return
     */
    bool isOpen() const;

    /**
     * @brief open
     * @return
     */
    bool open();

    /**
     * @brief close
     */
    void close();

signals:
    void opened();
    void closed();
    void toggled(int isOpen);
    void channelChanged(const Icd::ChannelPtr &channel);
    void channelConfigChanged();

public slots:

private:
    Q_DISABLE_COPY(Worker)
    J_DECLARE_PRIVATE(Worker)
};

} // end of namespace Icd

#endif // ICDWORKER_H
