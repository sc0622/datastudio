#ifndef JFILTERTABLEVIEW_H
#define JFILTERTABLEVIEW_H

#include "jwt_global.h"
#include "jheaderarea.h"
#include "jxmltable.h"

// - class JFilterTableView -

class JFilterTableViewPrivate;

class JWT_EXPORT JFilterTableView : public JHeaderArea
{
    Q_OBJECT
public:
    explicit JFilterTableView(QWidget *parent);
    virtual ~JFilterTableView();

    JXmlTable *view();

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JFilterTableView)
    J_DECLARE_PRIVATE(JFilterTableView)
};

#endif // JFILTERTABLEVIEW_H
