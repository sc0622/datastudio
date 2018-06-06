#ifndef SIMULATEWINDOW_H
#define SIMULATEWINDOW_H

#include <QWidget>

class JSplitter;

namespace Json { class Value; }

namespace Simulate {

class TreeView;
class SetView;

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
    TreeView *treeView_;
    SetView *setView_;
};

}

#endif // SIMULATEWINDOW_H
