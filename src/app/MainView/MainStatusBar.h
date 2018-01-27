#ifndef MAIN_STATUSBAR_H
#define MAIN_STATUSBAR_H

#include <QStatusBar>

namespace Main {

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = 0);

signals:

public slots:

private:
};

}

#endif // MAIN_STATUSBAR_H
