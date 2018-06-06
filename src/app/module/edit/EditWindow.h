#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QWidget>

class JSplitter;

namespace Edit {

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

#endif // EDITWINDOW_H
