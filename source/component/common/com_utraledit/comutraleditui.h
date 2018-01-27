#ifndef COMUTRALEDITUI_H
#define COMUTRALEDITUI_H

#include <QWidget>
#include "icdcore/icd_table.h"
#include "icdworker/icdworker_pool.h"

class QStandardItem;

namespace JUtralEdit {
class JView;
}

class IJAttempter;

class ComUtralEditUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComUtralEditUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComUtralEditUi();

    void setVisible(bool visible);

    void start();

    void stop();

Q_SIGNALS:

public Q_SLOTS:
    void onTreeItemClicked(QStandardItem *item);
    void onCurrentItemChanged(QStandardItem *current, QStandardItem *previous);
    void onChannelUnbound(QStandardItem *item, const QString &channelId);
    void onChannelChanged(QStandardItem *item, const QString &channelId);
    void onWorkerRemoved(const Icd::WorkerPtr &worker);
    void onWorkerCleared();
    void onWorkerStarted();
    void onWorkerStopped();

protected:
    void timerEvent(QTimerEvent *event);

private:
    void setDirty();

    void updateView();

private:
    IJAttempter &d_attempter;
    JUtralEdit::JView *d_view;
    QStandardItem *d_item;
    Icd::WorkerPtr d_worker;
    int d_timerId;
};

#endif // COMUTRALEDITUI_H
