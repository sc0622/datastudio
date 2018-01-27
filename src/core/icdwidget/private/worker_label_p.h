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
    QLabel *labelChannelIcon;   // ͨ��ͼ��
    QLabel *labelChannelType;   // ͨ������
    QLabel *labeName;           // ͨ������
    QLabel *labelDetail;        // ͨ������
    QPushButton *buttonOpen;    // ͨ������
    QPushButton *buttonSwitchRecv;  // ���տ���
    QPushButton *buttonRemove;  // ɾ��ͨ��
    QLabel *labelIndicator;     //
};

} // end of namespace Icd

#endif // CHANNEL_WIDGET_P_H
