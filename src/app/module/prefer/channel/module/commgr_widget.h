#ifndef COMMGR_WIDGET_H
#define COMMGR_WIDGET_H

#include <QWidget>
#include "icdworker/icdworker_pool.h"

// class ComMgrWidget;

class ComMgrWidgetPrivate;
class QListWidgetItem;

class ComMgrWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ComMgrWidget(QWidget *parent = nullptr);
    ~ComMgrWidget();

    bool init();

signals:

public slots:

private:
    Q_DISABLE_COPY(ComMgrWidget)
    J_DECLARE_PRIVATE(ComMgrWidget)
};

#endif // COMMGR_WIDGET_H
