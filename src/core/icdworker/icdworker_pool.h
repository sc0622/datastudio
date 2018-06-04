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

/**
 * @brief The WorkerGroup class
 */
class ICDWORKER_EXPORT WorkerPool : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 获取工作组管理器实例
     * @return
     */
    static WorkerPoolPtr getInstance();

    /**
     * @brief releaseInstance
     */
    static void releaseInstance();

    /**
     * @brief allWorkers
     * @return
     */
    WorkerPtrArray allWorkers();

    /**
     * @brief allWorkers
     * @return
     */
    const WorkerPtrArray &allWorkers() const;

    /**
     * @brief appendWorker
     * @param worker
     */
    void appendWorker(const WorkerPtr &worker);

    /**
     * @brief insertWorker
     * @param index
     * @param worker
     */
    void insertWorker(int index, const WorkerPtr &worker);

    /**
     * @brief removeWorker
     * @param worker
     */
    void removeWorker(const WorkerPtr &worker);

    /**
     * @brief removeWorker
     * @param channel
     */
    void removeWorker(const ChannelPtr &channel);

    /**
     * @brief 删除工作组实例
     * @param [in] index : 工作组在队列中的索引号
     */
    void removeWorker(int index);

    /**
     * @brief clearWorker
     */
    void clearWorker();

    /**
     * @brief moveToBottom
     * @param worker
     */
    void moveToBottom(const WorkerPtr &worker);

    /**
     * @brief swapWorker
     * @param worker1
     * @param worker2
     */
    void swapWorker(const WorkerPtr &worker1, const WorkerPtr &worker2);

    /**
     * @brief 获取指定序号的工作组实例
     * @param [in] index : 工作组在队列中的索引号
     * @return
     */
    WorkerPtr workerAt(int index);

    /**
     * @brief workerByChannel
     * @param channel
     * @return
     */
    WorkerPtr workerByChannel(const ChannelPtr &channel) const;

    /**
     * @brief workerByChannelIdentity
     * @param identity
     * @return
     */
    WorkerPtr workerByChannelIdentity(const std::string &identity) const;


    /**
     * @brief workerByChannelName
     * @param name
     * @return
     */
    WorkerPtr workerByChannelName(const std::string &name) const;

    /**
     * @brief configFile
     * @return
     */
    std::string configFile() const;

    /**
     * @brief setConfigFile
     * @param filePath
     */
    void setConfigFile(const std::string filePath);

    /**
     * @brief 载入通道配置文件
     * @return
     */
    bool loadConfig();

    /**
     * @brief 载入通道配置文件
     * @param filePath
     * @return
     */
    bool loadConfig(const std::string filePath);

    /**
     * @brief 保存通道配置文件
     * @return
     */
    bool saveConfig();

    /**
     * @brief stop
     */
    void stop();

signals:
    /**
     * @brief workerAdded
     * @param worker
     */
    void workerAdded(const Icd::WorkerPtr &worker);

    /**
     * @brief workerRemoved
     * @param worker
     */
    void workerRemoved(const Icd::WorkerPtr &worker);

    /**
     * @brief workerCleared
     */
    void workerCleared();

    /**
     * @brief updated
     */
    void updated();

public slots:

public:
    //J_FRIEND_HANDLEPTR()
    explicit WorkerPool(QObject *parent = nullptr);
    ~WorkerPool();

private:
    std::shared_ptr<WorkerPoolData> d;
};

} // end of namespace Icd

#endif // ICDWORKER_POOL_H
