#ifndef JSTATUSBAR_H
#define JSTATUSBAR_H

#include "jwt_global.h"
#include <QStatusBar>

class JWT_EXPORT JStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit JStatusBar(QWidget *parent = 0);

private:
};

#endif // JSTATUSBAR_H
