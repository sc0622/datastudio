#ifndef ICDWORKER_POOL_H
#define ICDWORKER_POOL_H

#include <QObject>
#include "icdworker_global.h"
#include "icdworker.h"

namespace Icd {

class WorkerPool;
class WorkerPoolData;
typedef std::shared_ptr<WorkerPool> WorkerPoolPtr;

class ::Icd::Channel;
typedef std::shared_ptr<::Icd::Channel> ChannelPtr;

class ICDWORKER_EXPORT WorkerPool : public QObject
{
    Q_OBJECT
public:
    static WorkerPoolPtr getInstance();
    static void releaseInstance();

    WorkerPtrArray allWorkers();
    const WorkerPtrArray &allWorkers() const;
    void appendWorker(const WorkerPtr &worker);
    void insertWorker(int index, const WorkerPtr &worker);
    void removeWorker(const WorkerPtr &worker);
    void removeWorker(const ChannelPtr &channel);
    void removeWorker(int index);
    void clearWorker();
    void moveToBottom(const WorkerPtr &worker);
    void swapWorker(const WorkerPtr &worker1, const WorkerPtr &worker2);
    WorkerPtr workerAt(int index);
    WorkerPtr workerByChannel(const ChannelPtr &channel) const;
    WorkerPtr workerByChannelIdentity(const std::string &identity) const;
    WorkerPtr workerByChannelName(const std::string &name) const;

    std::string configFile() const;
    void setConfigFile(const std::string filePath);
    bool loadConfig();
    bool loadConfig(const std::string filePath);
    bool saveConfig();

signals:
    void workerAdded(const Icd::WorkerPtr &worker);
    void workerRemoved(const Icd::WorkerPtr &worker);
    void workerCleared();
    void updated();

public slots:
    void stop();

public:
    //J_FRIEND_HANDLEPTR()
    explicit WorkerPool(QObject *parent = nullptr);
    ~WorkerPool();

private:
    std::shared_ptr<WorkerPoolData> d;
};

} // end of namespace Icd

#endif // ICDWORKER_POOL_H
