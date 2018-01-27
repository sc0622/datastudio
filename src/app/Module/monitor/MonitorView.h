#ifndef MONITORVIEW_H
#define MONITORVIEW_H

#include <QWidget>

class QToolBar;
class QAction;
class JSplitter;

namespace Json { class Value; }

namespace Monitor {

class TreeView;
class DataView;
class ChartView;

class View : public QWidget
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = nullptr);
    ~View();

    bool init();

signals:

public slots:

private:
    void updateToolBar();
    void addViewAction(const Json::Value &option);
    QAction *addOrigValueRadixAction(QAction *action, const Json::Value &option);
    void addFlushSwitchAction(const Json::Value &option);
    void addSettingsAction();

private:
    QToolBar *d_toolBar;
    JSplitter *d_splitterMain;
    JSplitter *d_splitterLeft;
    TreeView *d_treeView;
    DataView *d_dataView;
    ChartView *d_chartView;
};

}

#endif // MONITORVIEW_H
