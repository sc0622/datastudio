#include "precomp.h"
#include "icdworker_pool.h"
#include <QMutex>
#include "icdcomm/icdcomm_filechannel.h"
#include <algorithm>

namespace Icd {

// class WorkerPoolData

class WorkerPoolData
{
    friend class WorkerPool;
public:
    WorkerPoolData()
    {
    }

private:
    WorkerPtrArray workers;
    std::string filePath;       // 配置文件路径
    static QMutex instanceMutex;
    static WorkerPoolPtr instance;
};

QMutex WorkerPoolData::instanceMutex;
WorkerPoolPtr WorkerPoolData::instance = WorkerPoolPtr(0);

// clas WorkerPool

WorkerPoolPtr WorkerPool::getInstance()
{
    // 双重检测（防止多线程竞争）
    if (WorkerPoolData::instance == 0) {
        WorkerPoolData::instanceMutex.lock();
        if (WorkerPoolData::instance == 0) {
            WorkerPoolPtr tempInstance = WorkerPoolPtr(new WorkerPool());
            WorkerPoolData::instance = tempInstance;
        }
        WorkerPoolData::instanceMutex.unlock();
    }

    return WorkerPoolData::instance;
}

void WorkerPool::releaseInstance()
{
    // 双重检测（防止多线程竞争）
    if (WorkerPoolData::instance != 0) {
        WorkerPoolData::instanceMutex.lock();
        if (WorkerPoolData::instance != 0) {
            WorkerPoolData::instanceMutex.unlock();
            WorkerPoolData::instance = 0;
            WorkerPoolData::instanceMutex.lock();
        }
        WorkerPoolData::instanceMutex.unlock();
    }
}

WorkerPtrArray WorkerPool::allWorkers()
{
    return d->workers;
}

const WorkerPtrArray &WorkerPool::allWorkers() const
{
    return d->workers;
}

void WorkerPool::appendWorker(const WorkerPtr &worker)
{
    for (WorkerPtrArray::const_iterator citer = d->workers.cbegin();
         citer != d->workers.cend(); ++citer) {
        const WorkerPtr &_worker = *citer;
        if (_worker == worker) {
            return;
        }
    }

    worker->setParent(this);
    d->workers.push_back(worker);
    emit workerAdded(worker);
    //emit updated();
}

void WorkerPool::insertWorker(int index, const WorkerPtr &worker)
{
    for (WorkerPtrArray::const_iterator citer = d->workers.cbegin();
         citer != d->workers.cend(); ++citer) {
        const WorkerPtr &_worker = *citer;
        if (_worker == worker) {
            return;
        }
    }

    worker->setParent(this);
    d->workers.insert(d->workers.cbegin() + index, worker);
    emit workerAdded(worker);
    //emit updated();
}

void WorkerPool::removeWorker(const WorkerPtr &worker)
{
    for (WorkerPtrArray::const_iterator citer = d->workers.cbegin();
         citer != d->workers.cend(); ++citer) {
        const WorkerPtr &_worker = *citer;
        if (_worker == worker) {
            d->workers.erase(citer);
            emit workerRemoved(worker);
            //emit updated();
            break;
        }
    }
}

void WorkerPool::removeWorker(const ChannelPtr &channel)
{
    for (WorkerPtrArray::const_iterator citer = d->workers.cbegin();
         citer != d->workers.cend(); ++citer) {
        const WorkerPtr &worker = *citer;
        if (worker->channel() == channel) {
            d->workers.erase(citer);
            emit workerRemoved(worker);
            //emit updated();
            break;
        }
    }
}

void WorkerPool::removeWorker(int index)
{
    if (index < 0 || index >= (int)d->workers.size()) {
        return;
    }

    WorkerPtrArray::const_iterator citer = d->workers.cbegin() + index;
    d->workers.erase(citer);
    emit workerRemoved(*citer);
    //emit updated();
}

void WorkerPool::clearWorker()
{
    d->workers.clear();
    emit workerCleared();
    //emit updated();
}

void WorkerPool::moveToBottom(const WorkerPtr &worker)
{
    if (!worker || d->workers.size() < 2) {
        return;
    }

    WorkerPtrArray::iterator iter =
            std::find(d->workers.begin(), d->workers.end(), worker);
    if (iter == d->workers.end()) {
        return;
    }

    d->workers.erase(iter);
    d->workers.push_back(worker);
    //emit updated();
}

void WorkerPool::swapWorker(const WorkerPtr &worker1, const WorkerPtr &worker2)
{
    if (!worker1 || !worker2) {
        return;
    }

    WorkerPtrArray::iterator iter1 =
            std::find(d->workers.begin(), d->workers.end(), worker1);
    WorkerPtrArray::iterator iter2 =
            std::find(d->workers.begin(), d->workers.end(), worker2);
    if (iter1 == d->workers.end() || iter2 == d->workers.end()) {
        return;
    }

    *iter1 = worker2;
    *iter2 = worker1;
}

WorkerPtr WorkerPool::workerAt(int index)
{
    if (index < 0 || index >= (int)d->workers.size()) {
        return WorkerPtr(0);
    }

    return d->workers.at(index);
}

WorkerPtr WorkerPool::workerByChannel(const ChannelPtr &channel) const
{
    for (WorkerPtrArray::const_iterator citer = d->workers.cbegin();
         citer != d->workers.cend(); ++citer) {
        const WorkerPtr &worker = *citer;
        if (worker->channel() == channel) {
            return worker;
        }
    }

    return WorkerPtr(0);
}

WorkerPtr WorkerPool::workerByChannelIdentity(const std::string &identity) const
{
    for (WorkerPtrArray::const_iterator citer = d->workers.cbegin();
         citer != d->workers.cend(); ++citer) {
        const WorkerPtr &worker = *citer;
        if (worker->channel()->identity() == identity) {
            return worker;
        }
    }

    return WorkerPtr(0);
}

WorkerPtr WorkerPool::workerByChannelName(const std::string &name) const
{
    for (WorkerPtrArray::const_iterator citer = d->workers.cbegin();
         citer != d->workers.cend(); ++citer) {
        const WorkerPtr &worker = *citer;
        if (worker->channel()->name() == name) {
            return worker;
        }
    }

    return WorkerPtr(0);
}

std::string WorkerPool::configFile() const
{
    return d->filePath;
}

void WorkerPool::setConfigFile(const std::string filePath)
{
    d->filePath = filePath;
}

bool WorkerPool::loadConfig()
{
    //
    d->workers.clear();

    //
    std::ifstream fin(d->filePath);
    if (!fin) {
        return false;
    }
    //
    std::map<std::string, Icd::WorkerPtr> relayers;
    //
    char *line = new char[1024];
    memset(line, 0, 1024);
    while (!fin.eof()) {
        //
        fin.getline(line, 1024);
        //
        std::string config(line);
        if (config.empty()) {
            continue;
        }
        // ignore string that starts with '#' character
        config = config.substr(0, config.find_first_of('#'));
        if (config.empty()) {
            continue;
        }
        // trim
        config.erase(0, config.find_first_not_of(' '));   // left trim
        config.erase(config.find_last_not_of(' ') + 1);   // right trim
        //
        if (config.empty()) {
            continue;   // invalid line, ignore
        }
        //
        ChannelPtr newChannel = Icd::Channel::createInstance(config);
        if (newChannel == 0) {
            continue;
        }
        // parse
        const std::map<std::string, std::string> items = Icd::Channel::parseConfig(config);
        if (items.empty()) {
            continue;
        }
        // create worker
        Icd::WorkerPtr newWorker = Icd::WorkerPtr(new Icd::Worker(newChannel));
        // relayer
        std::map<std::string, std::string>::const_iterator citer = items.find("relayer");
        if (citer != items.cend()) {
            relayers[citer->second] = newWorker;
        }
        //
        appendWorker(newWorker);
    }
    //
    fin.close();
    delete[] line;
    line = 0;

    // relayers
    std::map<std::string, Icd::WorkerPtr>::iterator iterRelayers = relayers.begin();
    for (; iterRelayers != relayers.end(); ++iterRelayers) {
        Icd::WorkerPtr &relayer = iterRelayers->second;
        Icd::WorkerPtr worker = workerByChannelIdentity(iterRelayers->first);
        if (worker) {
            relayer->setRelayer(worker);
        }
    }

    //
    emit updated();

    return true;
}

bool WorkerPool::loadConfig(const std::string filePath)
{
    //
    d->filePath = filePath;

    //
    return loadConfig();
}

bool WorkerPool::saveConfig()
{
    //
    std::ofstream fout(d->filePath);
    if (!fout) {
        return false;
    }
    fout << std::endl;
    //
    WorkerPtrArray::const_iterator citer = d->workers.cbegin();
    for (; citer != d->workers.cend(); ++citer) {
        const WorkerPtr &worker = *citer;
        if (worker) {
            Icd::ChannelPtr channel = worker->channel();
            fout << channel->config() << std::endl;
        }
    }
    fout.close();

    return true;
}

void WorkerPool::stop()
{
    WorkerPtrArray::const_iterator citer = d->workers.cbegin();
    for (; citer != d->workers.cend(); ++citer) {
        const WorkerPtr &worker = *citer;
        if (worker) {
            worker->workerSend()->stop();
            worker->workerRecv()->stop();
        }
    }
}

WorkerPool::WorkerPool(QObject *parent)
    : QObject(parent)
    , d(new WorkerPoolData())
{

}

WorkerPool::~WorkerPool()
{

}

} // end of namespace Icd
