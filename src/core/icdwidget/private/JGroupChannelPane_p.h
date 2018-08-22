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
    explicit JGroupChannelPanePrivate(JChannelPane::OperateAttribute attrs, JGroupChannelPane *q);
    ~JGroupChannelPanePrivate();

    void init();

    Icd::WorkerPtr selectedWorker() const;
    Icd::ChannelType currentChannelType() const;
    Icd::JChannelPane *currentPane() const;
    int currentTabIndex() const;
    int currentRowIndex() const;
    int currentRowCount() const;

    void addWorker(const Icd::WorkerPtr &worker);

signals:
    void currentRowChanged(int rowIndex, int tabIndex);

public slots:
    void updateAllTab();
    void setCurrentRowIndex(int index);

private slots:
    void onRowDoubleClicked(Icd::JChannelPane *widget, int row);

private:
    void updateCurrentList(int index = -1);

private:
    J_DECLARE_PUBLIC(JGroupChannelPane)
    JChannelPane::OperateAttribute operateAttrs;
    QTabWidget *tabWidget;
    Icd::JChannelPane *channelPaneFile;
    Icd::JChannelPane *channelPaneSerial;
    Icd::JChannelPane *channelPaneUdp;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;
};

} // end of namespace Icd

#endif // JGROUPCHANNELPANE_P_H
