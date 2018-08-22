#ifndef MONITORDATAVIEW_H
#define MONITORDATAVIEW_H

#include <QWidget>
#include "icdworker/icdworker_global.h"

namespace Icd {

class JProtoTreeView;

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

}

namespace JUtralEdit { class JView; }

class QStandardItem;

namespace Monitor {

class DataView : public QWidget
{
    Q_OBJECT
public:
    explicit DataView(QWidget *parent = nullptr);
    ~DataView();

    bool init();

signals:

public slots:
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent *event);

private:
    void reset();
    void updateData(QStandardItem *item);
    void updateView();

private:
    JUtralEdit::JView *d_dataView;
    QStandardItem *d_item;
    Icd::WorkerPtr d_worker;
    int d_timerId;
};

}

#endif // MONITORDATAVIEW_H
