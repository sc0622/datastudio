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
    : q_ptr(q)
    , mutex(QMutex::Recursive)
    , timer(new JRTTimer(JRTTimer::TimePeriodic, 20/*default: 10ms*/))
    , channel(0)
    , table(0)
    , tableSize(0)
    , tableBuffer(0)
    , counterLoop(true)
    , frameLoop(false)
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
        tableBuffer = 0;
    }

    tableSize = 0;

    if (table && table->bufferSize() > 0) {
        if (valid) {
            tableSize = (int)std::ceil(table->bufferSize());
            tableBuffer = new char[tableSize];
            memset(tableBuffer, 0, tableSize);
            table->setBuffer(tableBuffer);
            table->reset();
        } else {
            table->setBuffer(0);
        }
    }
}

bool WorkerSendPrivate::sendData(bool counterLoop, bool frameLoop)
{
    // 处理当前帧
    doFrame(frameLoop);

    // 帧计数累加
    if (counterLoop) {
        doCounter();
    }

    // 生成校验值
    doCheck();

    //
    int size = channel->write((const char *)table->buffer(), table->bufferSize());
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

    // 无帧计数项时直接返回成功
    if (!counterItem) {
        return;
    }

    //
    switch (counterItem->counterType()) {
    case Icd::CounterU8:
        counterItem->setData(
                    (icd_uint8)((icd_uint8)counterItem->data() + 1));
        break;
    case Icd::CounterU16:
        counterItem->setData(
                    (icd_uint16)((icd_uint16)counterItem->data() + 1));
        break;
    case Icd::CounterU32:
        counterItem->setData(
                    (icd_uint32)((icd_uint32)counterItem->data() + 1));
        break;
    case Icd::CounterU64:
        counterItem->setData(
                    (icd_uint64)((icd_uint64)counterItem->data() + 1));
        break;
    default:
        break;
    }
}

bool WorkerSendPrivate::doCheck()
{
    // 无校验项时直接返回成功
    if (!table->isCheckValid()) {
        return true;
    }

    //
    Icd::CheckItemPtr checkItem = table->checkItem();

    //
    switch (checkItem->checkType()) {
    case Icd::CheckSum8:
    {
        //
        icd_uint8 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum += *((icd_uint8*)tableBuffer + i);
        }

        //
        checkItem->setData(sum);

        break;
    }
    case Icd::CheckSum16:
    {
        //
        icd_uint16 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum += *((icd_uint16*)tableBuffer + i);
        }

        //
        checkItem->setData(sum);

        break;
    }
    case Icd::CheckCrc8:
    {/*
        icd_uint8 crcResult =
                gCalcCrc8((unsigned char *)tableBuffer + checkItem->startPos(),
                          checkItem->checkLength());
        //
        checkItem->setData(crcResult);
        break;*/
    }
    case Icd::CheckCrc16:
    {
        icd_uint16 crcResult =
                gCalcCrc16((unsigned char *)tableBuffer + checkItem->startPos(),
                           checkItem->checkLength());
        //
        checkItem->setData(crcResult);
        break;
    }
    case Icd::CheckXor8:
    {
        //
        icd_uint8 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum ^= *((icd_uint8*)tableBuffer + i);
        }

        //
        checkItem->setData(sum);
    }
    case Icd::CheckXor16:
    {
        //
        icd_uint16 sum = 0;
        for (int i = checkItem->startPos(); i <= checkItem->endPos(); ++i) {
            sum ^= *((icd_uint16*)tableBuffer + i);
        }

        //
        checkItem->setData(sum);
    }
    default:
        break;
    }

    return true;    // not spported check type
}

bool WorkerSendPrivate::isRunning() const
{
    return timer->isRunning();
}

bool WorkerSendPrivate::start()
{
    return timer->start();
}

void WorkerSendPrivate::stop()
{
    timer->stop();
}

int WorkerSendPrivate::interval() const
{
    return (int)timer->interval();
}

void WorkerSendPrivate::setInterval(int value)
{
    timer->setInterval(value);
}

WorkerTrans::TimeEvent WorkerSendPrivate::timeEvent() const
{
    return (WorkerTrans::TimeEvent)timer->timeEvent();
}

void WorkerSendPrivate::setTimeEvent(WorkerTrans::TimeEvent event)
{
    timer->setTimeEvent((JRTTimer::TimeEvent)event);
}

void WorkerSendPrivate::run()
{
    QMutexLocker locker(&mutex);

    if (!tableBuffer) {
        return;
    }

    if (!channel || !channel->isOpen() || !table || table->bufferSize() <= 0) {
        return;
    }

    sendData(counterLoop, frameLoop);
}

} // end of namespace Icd
