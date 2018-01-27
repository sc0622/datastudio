#include "precomp.h"
#include "icdworker_recv_p.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item_frame.h"
#include "icdcore/icd_item_framecode.h"
#include "icdcore/icd_item_counter.h"
#include "icdcore/icd_item_check.h"
#include "icdworker_util.h"
#include <QtConcurrent>

namespace Icd {

/**
 * @brief _updateRelayer
 * @param channel
 * @param table
 */
extern void _updateRelayer(Icd::ChannelPtr &channel, const Icd::TablePtr &table);

WorkerRecvPrivate::WorkerRecvPrivate(Icd::WorkerRecv *q)
    : q_ptr(q)
    , mutex(QMutex::Recursive)
    , timer(new JRTTimer(JRTTimer::TimePeriodic, 5/*default: 5ms*/))
    , channel(0)
    , table(0)
    , relayer(0)
    , tableSize(0)
    , tableBuffer(0)
    , parseBuffer(0)
    , currentIndex(0)
    , queueCount(0)
{
    timer->setTask(this);
}

WorkerRecvPrivate::~WorkerRecvPrivate()
{
    updateBind(false);
}

void WorkerRecvPrivate::init()
{

}

void WorkerRecvPrivate::updateBind(bool valid)
{
    clearQueue();

    if (tableBuffer) {
        delete[] tableBuffer;
        tableBuffer = 0;
    }

    if (parseBuffer) {
        delete[] parseBuffer;
        parseBuffer = 0;
    }

    tableSize = 0;
    currentIndex = 0;

    if (table && table->bufferSize() > 0) {
        if (valid) {
            tableSize = (int)std::ceil(table->bufferSize());
            tableBuffer = new char[tableSize];
            parseBuffer = new char[tableSize];
            memset(tableBuffer, 0, tableSize);
            memset(parseBuffer, 0, tableSize);
            table->setBuffer(tableBuffer);
            _updateRelayer(channel, table);
        } else {
            table->setBuffer(0);
        }
    }
}

void WorkerRecvPrivate::recvData()
{
    char buffer[8192];
    int size = channel->read(buffer, 8192);
    if (size <= 0) {
        return;
    }
    //
    if (recordFile.isOpen()) {
        recordFile.write(buffer, size);
    }

    Icd::ChannelPtr relayer = channel->relayer();
    const std::vector<char> &headers = table->headers();
    const int headerSize = headers.size();

    for (int i = 0, section = 0; i < size; ++i) {
        if (currentIndex < headerSize) {
            if (buffer[i] == headers[currentIndex]) {
                parseBuffer[currentIndex++] = buffer[i];
            } else {
                currentIndex = 0;
                continue;
            }
        } else if (currentIndex < tableSize) {
            section = qMin(tableSize - currentIndex, size - i);
            memcpy(parseBuffer + currentIndex, buffer + i, section);
            currentIndex += section;
            i += section - 1;
        }

        if (currentIndex == tableSize) {
            if (doCheck(table, parseBuffer)) {
                //
                memcpy(tableBuffer, parseBuffer, tableSize);
                table->updateRecv();
                //
                if (relayer) {
                    relayer->write(parseBuffer, tableSize);
                }
                // queue
                if (queueCount > 0) {
                    char *newBuffer = new char[tableSize];
                    memcpy(newBuffer, tableBuffer, tableSize);
                    bufferQueue.enqueue(newBuffer);
                    if (bufferQueue.count() > queueCount) {
                        char *buffer = bufferQueue.dequeue();
                        delete[] buffer;
                    }
                }
            }
            currentIndex = 0;
        }
    }
}

bool WorkerRecvPrivate::isRunning() const
{
    return timer->isRunning();
}

bool WorkerRecvPrivate::start()
{
    return timer->start();
}

void WorkerRecvPrivate::stop()
{
    timer->stop();
}

int WorkerRecvPrivate::interval() const
{
    return (int)timer->interval();
}

void WorkerRecvPrivate::setInterval(int value)
{
    timer->setInterval(value);
}

WorkerTrans::TimeEvent WorkerRecvPrivate::timeEvent() const
{
    return (WorkerTrans::TimeEvent)timer->timeEvent();
}

void WorkerRecvPrivate::setTimeEvent(WorkerTrans::TimeEvent event)
{
    timer->setTimeEvent((JRTTimer::TimeEvent)event);
}

void WorkerRecvPrivate::clearQueue()
{
    for (auto buffer : bufferQueue) {
        delete[] buffer;
    }
    bufferQueue.clear();
}

bool WorkerRecvPrivate::startRecord()
{
    if (!channel || !table) {
        return false;
    }

    QMutexLocker locker(&mutex);

    if (recordFile.isOpen()) {
        recordFile.close();
    }

    QFileInfo fileInfo(QApplication::applicationDirPath()
                       .append("/data/record/%1/%2/%2-%3.dat")
                       .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                       .arg(QString::fromStdString(table->name()))
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss")));
    bool result = fileInfo.absoluteDir().mkpath(fileInfo.absolutePath());
    if (!result) {
        return false;
    }
    recordFile.setFileName(fileInfo.filePath());
    if (!recordFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    return false;
}

void WorkerRecvPrivate::stopRecord()
{
    mutex.lock();

    if (recordFile.isOpen()) {
        const int size = recordFile.size();
        recordFile.close();
        if (size == 0) {
            recordFile.remove();
        }
    }

    mutex.unlock();
}

void WorkerRecvPrivate::run()
{
    QMutexLocker locker(&mutex);

    if (!tableBuffer) {
        return;
    }

    if (!channel || !channel->isOpen() || !table || tableSize <= 0) {
        return;
    }

    recvData();
}

} // end of namespace Icd
