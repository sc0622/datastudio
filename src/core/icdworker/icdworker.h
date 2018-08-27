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
typedef std::shared_ptr<Worker> WorkerPtr;
typedef std::vector<WorkerPtr> WorkerPtrArray;

class WorkerPrivate;

class ICDWORKER_EXPORT Worker : public QObject
{
    Q_OBJECT
public:
    enum WorkerType {
        WorkerTypeSend,
        WorkerTypeRecv
    };

    explicit Worker(QObject *parent = nullptr);
    explicit Worker(const Icd::ChannelPtr &channel, QObject *parent = nullptr);
    explicit Worker(const Icd::ChannelPtr &channel, const Icd::TablePtr &tableSend,
                    const Icd::TablePtr &tableRecv, QObject *parent = nullptr);
    virtual ~Worker();

    Icd::ChannelPtr channel() const;
    void setChannel(const Icd::ChannelPtr &channel);

    Icd::WorkerPtr relayer() const;
    void setRelayer(const Icd::WorkerPtr &relayer);

    Icd::WorkerSendPtr workerSend() const;
    Icd::WorkerRecvPtr workerRecv() const;

    bool isOpen() const;
    bool open();
    void close();

signals:
    void opened();
    void closed();
    void toggled(bool isOpen);
    void channelChanged(const Icd::ChannelPtr &channel);
    void channelConfigChanged();

public slots:

private:
    Q_DISABLE_COPY(Worker)
    J_DECLARE_PRIVATE(Worker)
};

} // end of namespace Icd

#endif // ICDWORKER_H
