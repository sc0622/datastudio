#ifndef COMMGRWIDGETPRIVATE_H
#define COMMGRWIDGETPRIVATE_H

#include <QObject>
#include "icdworker/icdworker_pool.h"
#include "../commgr_widget.h"

namespace Icd {
class ChannelWidget;
}

class JSplitter;
class QScrollArea;
class CommDetailWidget;
class QPushButton;

class ComMgrWidgetPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ComMgrWidgetPrivate(ComMgrWidget *q);
    ~ComMgrWidgetPrivate();

    void init();
    void updateUi();

signals:

public slots:

private:
    J_DECLARE_PUBLIC(ComMgrWidget)
    JSplitter *splitterTop;
    QScrollArea *scrollArea;
    Icd::ChannelWidget *channelWidget;
    CommDetailWidget *commDetailWidget;
    QPushButton *buttonAdd;
    QPushButton *buttonClear;
};

#endif // COMMGRWIDGETPRIVATE_H
