#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QTabWidget>

class QPushButton;

namespace Main { class MenuBar; }
namespace Main { class ToolBar; }
namespace Preference { class View; }
namespace Edit { class View; }
namespace Monitor { class View; }
namespace Simulate { class View; }
namespace Analyse { class View; }

class CentralWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = 0);
    ~CentralWidget();

    bool init();

signals:

public slots:

private:
    Main::MenuBar *d_menuBar;
    Preference::View *d_preferenceView;
    Edit::View *d_editView;
    Monitor::View *d_monitorView;
    Simulate::View *d_simulateView;
    Analyse::View *d_analyseView;
};

#endif // CENTRALWIDGET_H
