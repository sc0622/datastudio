#ifndef JWORKERLABEL_H
#define JWORKERLABEL_H

#include <QWidget>
#include "icdwidget_global.h"
#include "JChannelPane.h"

namespace Icd {

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

class JWorkerLabelPrivate;

class ICDWIDGET_EXPORT JWorkerLabel : public QWidget
{
    Q_OBJECT
public:
    explicit JWorkerLabel(const Icd::WorkerPtr &worker, JChannelPane::OperateAttributes attrs,
                          QWidget *parent = nullptr);
    ~JWorkerLabel();

    void updateUi();

    Icd::WorkerPtr worker() const;
    JChannelPane::OperateAttributes attrs() const;
    QPixmap icon() const;
    void setIndicatorVisible(bool visible);

signals:

public slots:

private:
    Q_DISABLE_COPY(JWorkerLabel)
    J_DECLARE_PRIVATE(JWorkerLabel)
};

} // end of namespace Icd

#endif // JWORKERLABEL_H
