#ifndef BINDCHANNEL_WIDGET_H
#define BINDCHANNEL_WIDGET_H

#include <QDialog>
#include "icdwidget_global.h"
#include "icdworker/icdworker.h"

namespace Icd {

// class BindChannelWidget

class BindChannelWidgetPrivate;

class ICDWIDGET_EXPORT BindChannelWidget : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief BindChannelWidget
     * @param parent
     */
    explicit BindChannelWidget(QWidget *parent = 0);

    virtual ~BindChannelWidget();

    /**
     * @brief selectedWorker
     * @return
     */
    Icd::WorkerPtr selectedWorker() const;

signals:

public slots:

private:
    Q_DISABLE_COPY(BindChannelWidget)
    J_DECLARE_PRIVATE(BindChannelWidget)
};

} // end of namespace Icd

#endif // BINDCHANNEL_WIDGET_H
