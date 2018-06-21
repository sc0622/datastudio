#ifndef WORKER_LABEL_H
#define WORKER_LABEL_H

#include <QWidget>
#include "icdwidget_global.h"
#include "channel_widget.h"

namespace Icd {

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

class WorkerLabelPrivate;

/**
 * @brief The WorkerLabel class
 */
class ICDWIDGET_EXPORT WorkerLabel : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief WorkerLabel
     * @param worker
     * @param attrs
     * @param parent
     */
    explicit WorkerLabel(const Icd::WorkerPtr &worker, ChannelWidget::OperateAttributes attrs,
                         QWidget *parent = nullptr);

    ~WorkerLabel();

    /**
     * @brief updateUi
     */
    void updateUi();

    /**
     * @brief worker
     * @return
     */
    Icd::WorkerPtr worker() const;

    /**
     * @brief attrs
     * @return
     */
    ChannelWidget::OperateAttributes attrs() const;

    /**
     * @brief icon
     * @return
     */
    QPixmap icon() const;

    /**
     * @brief setIndicatorVisible
     * @param visible
     */
    void setIndicatorVisible(bool visible);

signals:

public slots:

private:
    Q_DISABLE_COPY(WorkerLabel)
    J_DECLARE_PRIVATE(WorkerLabel)
};

} // end of namespace Icd

#endif // WORKER_LABEL_H
