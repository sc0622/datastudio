#ifndef MAINTABWIDGET_H
#define MAINTABWIDGET_H

#include <QTabWidget>
#include "main_global.h"

namespace Main {

class MenuBar;

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = nullptr);
    ~TabWidget();

    bool init();

signals:

public slots:

private:
    MenuBar *menuBar_;
};

}

#endif // MAINTABWIDGET_H
