#ifndef EDITTREEVIEW_H
#define EDITTREEVIEW_H

#include <QWidget>

namespace Icd { class JProtoTreeView; }

namespace Edit {

class TreeView : public QWidget
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = nullptr);
    ~TreeView();

    bool init();

    void setShowAttribute(int attr, bool on = true);

signals:

public slots:

private:
    bool updateParser();

private:
    Icd::JProtoTreeView *treeView_;
};

}

#endif // EDITTREEVIEW_H
