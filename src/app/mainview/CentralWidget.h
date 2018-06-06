#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

class QStackedWidget;
namespace Prefer { class Window; }
namespace Edit { class Window; }
namespace Monitor { class Window; }
namespace Simulate { class Window; }
namespace Analyse { class Window; }

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

    bool init();

signals:

public slots:

private:
    QStackedWidget *stackedWidget_;
    Prefer::Window *preferWindow_;
    Edit::Window *editWindow_;
    Monitor::Window *monitorWindow_;
    Simulate::Window *simulateWindow_;
    Analyse::Window *analyseWindow_;
};

#endif // CENTRALWIDGET_H
