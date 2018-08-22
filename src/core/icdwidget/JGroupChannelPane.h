#ifndef JGROUPCHANNELPANE_H
#define JGROUPCHANNELPANE_H

#include <QDialog>
#include "icdwidget_global.h"
#include "icdworker/icdworker.h"

namespace Icd {

// class JGroupChannelPane

class JGroupChannelPanePrivate;

class ICDWIDGET_EXPORT JGroupChannelPane : public QDialog
{
    Q_OBJECT
public:
    explicit JGroupChannelPane(QWidget *parent = nullptr);
    virtual ~JGroupChannelPane();

    Icd::WorkerPtr selectedWorker() const;

signals:

public slots:

private:
    Q_DISABLE_COPY(JGroupChannelPane)
    J_DECLARE_PRIVATE(JGroupChannelPane)
};

} // end of namespace Icd

#endif // JGROUPCHANNELPANE_H
