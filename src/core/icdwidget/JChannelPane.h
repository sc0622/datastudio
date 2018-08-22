#ifndef JCHANNELPANE_H
#define JCHANNELPANE_H

#include <QWidget>
#include "icdwidget_global.h"

namespace Icd {

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

// class JChannelPane

class JChannelPanePrivate;

class ICDWIDGET_EXPORT JChannelPane : public QWidget
{
    Q_OBJECT
public:
    enum OperateAttribute {
        NoOperate = 0x00,
        OperateOpen = 0x01,
        OperateSwitchRecv = 0x02,
        OperateRemove = 0x04,
        OperateIndicator = 0x08,
        AllOperate = 0x0f
    };
    Q_DECLARE_FLAGS(OperateAttributes, OperateAttribute)

    explicit JChannelPane(QWidget *parent = nullptr);
    ~JChannelPane();

    Icd::WorkerPtr selectedWorker() const;

    int rowCount() const;
    void setCurrentRow(int row);

    void addWorker(const Icd::WorkerPtr &worker, OperateAttributes attrs);
    void insertWorker(int row, const Icd::WorkerPtr &worker, OperateAttributes attrs);
    void removeWorker(const Icd::WorkerPtr &worker);
    void removeWorker(const QString &channelId);
    void clearWorker();

    void setTableEnabled(bool enabled);
    void setEditEnabled(bool enabled);

signals:
    void currentRowChanged(int currentRow, int previousRow);
    void rowMoved(int oldRow, int newRow);
    void rowDoubleClicked(int row);

public slots:

protected:
    bool eventFilter(QObject *watcher, QEvent *event);

private:
    Q_DISABLE_COPY(JChannelPane)
    J_DECLARE_PRIVATE(JChannelPane)
};

} // end of namespace Icd

#endif // JCHANNELPANE_H
