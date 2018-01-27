#ifndef CHANNEL_WIDGET_P_H
#define CHANNEL_WIDGET_P_H

#include "../worker_label.h"
#include "channel_widget.h"

class QLabel;
class QPushButton;

namespace Icd {

// class ChannelWidgetPrivate

class WorkerLabelPrivate
{
public:
    explicit WorkerLabelPrivate(const Icd::WorkerPtr &worker,
                                WorkerLabel *q);
    ~WorkerLabelPrivate();

    void init(ChannelWidget::OperateAttributes attrs);
    void updateUi();
    void setDirty();

    ChannelWidget::OperateAttributes attrs() const;

private:
    J_DECLARE_PUBLIC(WorkerLabel)
    Icd::WorkerPtr worker;
    ChannelWidget::OperateAttributes attributes;
    QLabel *labelChannelIcon;   // 通道图标
    QLabel *labelChannelType;   // 通道类型
    QLabel *labeName;           // 通道配置
    QLabel *labelDetail;        // 通道描述
    QPushButton *buttonOpen;    // 通道开关
    QPushButton *buttonSwitchRecv;  // 接收开关
    QPushButton *buttonRemove;  // 删除通道
    QLabel *labelIndicator;     //
};

} // end of namespace Icd

#endif // CHANNEL_WIDGET_P_H
