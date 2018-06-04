#ifndef CHANNEL_WIDGET_H
#define CHANNEL_WIDGET_H

#include <QWidget>
#include "icdwidget_global.h"

namespace Icd {

class WorkerLabel;

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

// class ChannelWidget

class ChannelWidgetPrivate;

/**
 * @brief The ChannelWidget class
 */
class ICDWIDGET_EXPORT ChannelWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief The OperateAttribute enum
     */
    enum OperateAttribute {
        NoOperate = 0x00,           /**< */
        OperateOpen = 0x01,         /**< */
        OperateSwitchRecv = 0x02,   /**< */
        OperateRemove = 0x04,       /**< */
        OperateIndicator = 0x08,    /**< */
        AllOperate = 0x0f           /**< */
    };
    Q_DECLARE_FLAGS(OperateAttributes, OperateAttribute)

    /**
     * @brief ChannelWidget
     * @param parent
     */
    explicit ChannelWidget(QWidget *parent = 0);

    ~ChannelWidget();

    /**
     * @brief selectedWorker
     * @return
     */
    Icd::WorkerPtr selectedWorker() const;

    /**
     * @brief rowCount
     * @return
     */
    int rowCount() const;

    /**
     * @brief setCurrentRow
     * @param row
     */
    void setCurrentRow(int row);

    /**
     * @brief addWorker
     * @param worker
     * @param attrs
     */
    void addWorker(const Icd::WorkerPtr &worker, OperateAttributes attrs);

    /**
     * @brief insertWorker
     * @param row
     * @param worker
     * @param attrs
     */
    void insertWorker(int row, const Icd::WorkerPtr &worker, OperateAttributes attrs);

    /**
     * @brief removeWorker
     * @param worker
     */
    void removeWorker(const Icd::WorkerPtr &worker);

    /**
     * @brief removeWorker
     * @param channelId
     */
    void removeWorker(const QString &channelId);

    /**
     * @brief clearWorker
     */
    void clearWorker();

    /**
     * @brief setTableEnabled
     * @param enabled
     */
    void setTableEnabled(bool enabled);

    /**
     * @brief setEditEnabled
     * @param enabled
     */
    void setEditEnabled(bool enabled);

signals:
    /**
     * @brief currentRowChanged
     * @param currentRow
     * @param previousRow
     */
    void currentRowChanged(int currentRow, int previousRow);

    /**
     * @brief rowMoved
     * @param oldRow
     * @param newRow
     */
    void rowMoved(int oldRow, int newRow);

    /**
     * @brief rowDoubleClicked
     * @param row
     */
    void rowDoubleClicked(int row);

public slots:

protected:
    bool eventFilter(QObject *watcher, QEvent *event);

private:
    Q_DISABLE_COPY(ChannelWidget)
    J_DECLARE_PRIVATE(ChannelWidget)
};

} // end of namespace Icd

#endif // CHANNEL_WIDGET_H
