#include "precomp.h"
#include "icdworker_send_p.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item_frame.h"
#include "icdcore/icd_item_framecode.h"
#include "icdcore/icd_item_counter.h"
#include "icdcore/icd_item_check.h"
#include "icdcomm/icdcomm_filechannel.h"
#include "../icdworker_util.h"
#include "../icdworker_pool.h"

namespace Icd {

WorkerSendPrivate::WorkerSendPrivate(Icd::WorkerSend *q)
    : J_QPTR(q)
    , mutex(QMutex::Recursive)
    , timer(new JRTTimer(JRTTimer::TimePeriodic, 20/*default: 10ms*/))
    , channel(nullptr)
    , table(nullptr)
    , tableSize(0)
    , tableBuffer(nullptr)
    , counterLoop(true)
    , frameLoop(false)
    , running(false)
{
    timer->setTask(this);
}

WorkerSendPrivate::~WorkerSendPrivate()
{
    updateBind(false);
}

void WorkerSendPrivate::init()
{

}

void WorkerSendPrivate::updateBind(bool valid)
{
    if (tableBuffer) {
        delete[] tableBuffer;
        tableBuffer = nullptr;
    }

    tableSize = 0;

    if (table && table->bufferSize() > 0) {
        if (valid) {
            tableSize = table->bufferSize();
            tableBuffer = new char[size_t(tableSize)];
            memset(tableBuffer, 0, size_t(tableSize));
            table->setBuffer(tableBuffer);
            table->resetData();
        } else {
            table->setBuffer(nullptr);
        }
    }
}

bool WorkerSendPrivate::sendData(bool counterLoop, bool frameLoop)
{
    // ����ǰ֡
    doFrame(frameLoop);

    // ֡�����ۼ�
    if (counterLoop) {
        doCounter();
    }

    // ����У��ֵ
    doCheck();

    //
    const int size = channel->write(table->buffer(), table->bufferSize());
    if (size <= 0) {
        return false;
    }

    return true;
}

void WorkerSendPrivate::doFrame(bool loop)
{
    table->updateSend(loop);
}

void WorkerSendPrivate::doCounter()
{
    //
    Icd::CounterItemPtr counterItem = table->counterItem();

    // ��֡������ʱֱ�ӷ��سɹ�
    if (!counterItem) {
        return;
    }

    //
    switch (counterItem->counterType()) {
    case Icd::CounterU8:
        counterItem->setData(icd_uint8(icd_uint8(counterItem->data()) + 1));
        break;
    case Icd::CounterU16:
        counterItem->setData(icd_uint16(icd_uint16(counterItem->data()) + 1));
        break;
    case Icd::CounterU32:
        counterItem->setData(icd_uint32(icd_uint32(counterItem->data()) + 1));
        break;
    case Icd::CounterU64:
        counterItem->setData(icd_uint64(icd_uint64(counterItem->data()) + 1));
        break;
    default:
        break;
    }
}

bool WorkerSendPrivate::doCheck()
{
    // ��У����ʱֱ�ӷ��سɹ�
    if (!table->isCheckValid()) {
        return true;
    }

    Icd::CheckItemPtr checkItem = table->checkItem();

    switch (checkItem->checkType()) {
    case Icd::CheckSum8:
    {
        icd_uint8 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum += *(reinterpret_cast<icd_uint8*>(tableBuffer) + i);
        }

        checkItem->setData(sum);
        break;
    }
    case Icd::CheckSum16:
    {
        icd_uint16 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum += *(reinterpret_cast<icd_uint16*>(tableBuffer) + i);
        }

        checkItem->setData(sum);
        break;
    }
    case Icd::CheckCrc8:
    {/*
        icd_uint8 crcResult =
                gCalcCrc8(reinterpret_cast<unsigned char*>(tableBuffer) + checkItem->startPos(),
                          static_cast<unsigned int>(checkItem->checkLength()));
        checkItem->setData(crcResult);*/
        break;
    }
    case Icd::CheckCrc16:
    {
        icd_uint16 crcResult =
                gCalcCrc16(reinterpret_cast<unsigned char*>(tableBuffer) + checkItem->startPos(),
                           static_cast<unsigned int>(checkItem->checkLength()));
        checkItem->setData(crcResult);
        break;
    }
    case Icd::CheckXor8:
    {
        icd_uint8 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum ^= *(reinterpret_cast<icd_uint8*>(tableBuffer) + i);
        }

        checkItem->setData(sum);
        break;
    }
    case Icd::CheckXor16:
    {
        icd_uint16 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum ^= *(reinterpret_cast<icd_uint16*>(tableBuffer) + i);
        }

        checkItem->setData(sum);
        break;
    }
    default:
        break;
    }

    return true;    // not spported check type
}

bool WorkerSendPrivate::isRunning() const
{
    if (channel && channel->autoSend()) {
        return timer->isRunning();
    } else {
        return running;
    }
}

bool WorkerSendPrivate::start()
{
    if (channel) {
        if (channel->autoSend()) {
            return timer->start();
        } else {
            running = true;
            return true;
        }
    } else {
        running = timer->start();
        return running;
    }
}

void WorkerSendPrivate::stop()
{
    running = false;
    timer->stop();
}

int WorkerSendPrivate::interval() const
{
    if (channel) {
        return channel->sendInterval();
    } else {
        return int(timer->interval());
    }
}

void WorkerSendPrivate::setInterval(int value)
{
    if (channel) {
        channel->setSendInterval(value);
    }
    timer->setInterval(static_cast<unsigned int>(value));
}

WorkerTrans::TimeEvent WorkerSendPrivate::timeEvent() const
{
    if (channel) {
        return channel->autoSend() ? Icd::WorkerTrans::TimePeriodic
                                   : Icd::WorkerTrans::TimeOneShot;
    } else {
        return WorkerTrans::TimeEvent(timer->timeEvent());
    }
}

void WorkerSendPrivate::setTimeEvent(WorkerTrans::TimeEvent event)
{
    timer->setTimeEvent(JRTTimer::TimeEvent(event));

    if (channel) {
        channel->setAutoSend(event == Icd::WorkerTrans::TimePeriodic);
        if (channel->autoSend()) {
            if (running && !timer->isRunning()) {
                timer->start();
            }
        } else {
            if (timer->isRunning()) {
                timer->stop();
            }
        }
    }
}

void WorkerSendPrivate::run()
{
    QMutexLocker locker(&mutex);

    if (!tableBuffer) {
        return;
    }

    if (!channel || !channel->isOpen() || !table || tableSize <= 0) {
        return;
    }

    sendData(counterLoop, frameLoop);
}

} // end of namespace Icd
