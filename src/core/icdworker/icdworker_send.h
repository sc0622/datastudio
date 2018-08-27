#ifndef ICDWORKER_SEND_H
#define ICDWORKER_SEND_H

#include <QObject>
#include "icdworker_global.h"
#include "icdworker_trans.h"

namespace Icd {

class WorkerSendPrivate;
class WorkerSend;
typedef std::shared_ptr<WorkerSend> WorkerSendPtr;

class ICDWORKER_EXPORT WorkerSend : public QObject, public WorkerTrans
{
    Q_OBJECT
public:
    explicit WorkerSend(QObject *parent = nullptr);
    ~WorkerSend();

    Icd::ChannelPtr channel() const;
    void setChannel(const Icd::ChannelPtr &channel);

    Icd::TablePtr table() const;
    void setTable(const Icd::TablePtr &table);

    int interval() const;
    void setInterval(int value);

    TimeEvent timeEvent() const;
    void setTimeEvent(TimeEvent value);

    bool counterLoop() const;
    void setCounterLoop(bool loop);

    bool frameLoop() const;
    void setFrameLoop(bool loop);

    bool isRunning() const;
    bool start();
    void stop();
    bool sendOnce(bool counterLoop = false, bool frameLoop = false);
    bool sendOnce(const char *data, int size);

signals:
    void channelChanged(Icd::ChannelPtr &channel);
    void tableChanged(const Icd::TablePtr &table);
    void closed();
    void started();
    void stopped();
    void toggled(int isRunning);
    void intervalChanged(int value);
    void timeEventChanged(TimeEvent event);
    void counterLoopChanged(bool loop);
    void frameLoopChanged(bool loop);

public slots:

private:
    Q_DISABLE_COPY(WorkerSend)
    J_DECLARE_PRIVATE(WorkerSend)
};

} // end of namespace Icd

#endif // ICDWORKER_SEND_H
