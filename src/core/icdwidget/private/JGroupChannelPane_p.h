#ifndef JGROUPCHANNELPANE_P_H
#define JGROUPCHANNELPANE_P_H

#include "../JGroupChannelPane.h"

class QTabWidget;
class QPushButton;

namespace Icd {

class JChannelPane;

class JGroupChannelPanePrivate : public QObject
{
    Q_OBJECT
public:
    explicit JGroupChannelPanePrivate(JGroupChannelPane *q);
    ~JGroupChannelPanePrivate();

    void init();

    Icd::WorkerPtr selectedWorker() const;

signals:

public slots:
    void onRowDoubleClicked(Icd::JChannelPane *widget, int row);

private:
    void updateCurrentList(int index = -1);

private:
    J_DECLARE_PUBLIC(JGroupChannelPane)
    QTabWidget *tabWidget;
    Icd::JChannelPane *widgetFile;
    Icd::JChannelPane *widgetSerial;
    Icd::JChannelPane *widgetUdp;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;
};

} // end of namespace Icd

#endif // JGROUPCHANNELPANE_P_H
