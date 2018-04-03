#ifndef BINDCHANNELWIDGETPRIVATE_H
#define BINDCHANNELWIDGETPRIVATE_H

#include "../bindchannel_widget.h"

class QTabWidget;
class QPushButton;

namespace Icd {

class ChannelWidget;

class BindChannelWidgetPrivate : public QObject
{
    Q_OBJECT
public:
    explicit BindChannelWidgetPrivate(BindChannelWidget *q);
    ~BindChannelWidgetPrivate();

    void init();

    /**
     * @brief selectedWorker
     * @return
     */
    Icd::WorkerPtr selectedWorker() const;

signals:

public slots:
    void onRowDoubleClicked(ChannelWidget *widget, int row);

private:
    void updateCurrentList(int index = -1);

private:
    J_DECLARE_PUBLIC(BindChannelWidget)
    QTabWidget *tabWidget;
    ChannelWidget *widgetFile;
    ChannelWidget *widgetSerial;
    ChannelWidget *widgetUdp;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;
};

} // end of namespace Icd

#endif // BINDCHANNELWIDGETPRIVATE_H
