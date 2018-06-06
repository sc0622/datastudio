#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QWidget>

class JSplitter;

namespace Json { class Value; }

namespace Monitor {

class TreeView;
class DataView;
class ChartView;

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

    bool init();

signals:

public slots:

private:
    JSplitter *splitterMain_;
    JSplitter *splitterLeft_;
    TreeView *treeView_;
    DataView *dataView_;
    ChartView *chartView_;
};

}

#endif // MONITORWINDOW_H
