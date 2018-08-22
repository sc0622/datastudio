#ifndef JWORKERLABEL_P_H
#define JWORKERLABEL_P_H

#include "../JWorkerLabel.h"
#include "../JChannelPane.h"

class QLabel;
class QLineEdit;
class QPushButton;

namespace Icd {

// class JWorkerLabelPrivate

class JWorkerLabelPrivate
{
public:
    explicit JWorkerLabelPrivate(const Icd::WorkerPtr &worker, JWorkerLabel *q);
    ~JWorkerLabelPrivate();

    void init(JChannelPane::OperateAttributes attrs);
    void updateUi();
    void setDirty();

    JChannelPane::OperateAttributes attrs() const;

private:
    void updateDetailText();
    static QString elidedText(const QFont &font, const QString &text, int width);

private:
    J_DECLARE_PUBLIC(JWorkerLabel)
    Icd::WorkerPtr worker;
    JChannelPane::OperateAttributes attributes;
    QLabel *labelChannelIcon;
    QLabel *labelChannelType;
    QLineEdit *labelName;
    QLineEdit *labelDetail;
    QPushButton *buttonOpen;
    QPushButton *buttonSwitchRecv;
    QPushButton *buttonRemove;
    QLabel *labelIndicator;
};

} // end of namespace Icd

#endif // JWORKERLABEL_P_H
