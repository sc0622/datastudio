#ifndef ICDWORKER_POOL_H
#define ICDWORKER_POOL_H

#include <QObject>
#include "icdworker_global.h"
#include "icdworker.h"

namespace Icd {

class WorkerPool;
class WorkerPoolData;
typedef JHandlePtr<WorkerPool> WorkerPoolPtr;

class ::Icd::Channel;
typedef JHandlePtr<::Icd::Channel> ChannelPtr;

/**
 * @brief The WorkerGroup class
 */
class ICDWORKER_EXPORT WorkerPool : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief ��ȡ�����������ʵ��
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
     * @brief ɾ��������ʵ��
     * @param [in] index : �������ڶ����е�������
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
     * @brief ��ȡָ����ŵĹ�����ʵ��
     * @param [in] index : �������ڶ����е�������
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
     * @brief ����ͨ�������ļ�
     * @return
     */
    bool loadConfig();

    /**
     * @brief ����ͨ�������ļ�
     * @param filePath
     * @return
     */
    bool loadConfig(const std::string filePath);

    /**
     * @brief ����ͨ�������ļ�
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

private:
    J_FRIEND_HANDLEPTR()

    WorkerPool(QObject *parent = 0);
    ~WorkerPool();

private:
    JHandlePtr<WorkerPoolData> d;
};

} // end of namespace Icd

#endif // ICDWORKER_POOL_H
