#ifndef EDITVIEW_H
#define EDITVIEW_H

#include <QWidget>

class QToolBar;
class QAction;
class JSplitter;

namespace Json { class Value; }

namespace Edit {

class TreeView;
class SetView;

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
    QAction *addOrigValueRadixAction(QAction *action);
    void addSettingsAction();

private:
    QToolBar *d_toolBar;
    JSplitter *d_splitterMain;
    TreeView *d_treeView;
    SetView *d_setView;
};

}

#endif // EDITVIEW_H
