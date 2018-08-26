#include "precomp.h"
#include "commgr_widget.h"
#include "private/commgr_widget_p.h"

// class ComMgrWidget

ComMgrWidget::ComMgrWidget(QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new ComMgrWidgetPrivate(this))
{
    Q_D(ComMgrWidget);
    d->init();
}

ComMgrWidget::~ComMgrWidget()
{
    Q_D(ComMgrWidget);
    delete d;
}

bool ComMgrWidget::init()
{
    Q_D(ComMgrWidget);

    d->updateUi();

    return true;
}
