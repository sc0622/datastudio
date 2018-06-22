#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QWidget>

class JSplitter;

#ifdef EDIT_OLD
class ICDNavigation;
class EditMainView;
#endif

namespace Edit {

#ifdef EDIT_OLD
#else
class TreeView;
class SetView;
#endif

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
#ifdef EDIT_OLD
    ICDNavigation *nav_;
    EditMainView *view_;
#else
    TreeView *treeView_;
    SetView *setView_;
#endif
};

}

#endif // EDITWINDOW_H
