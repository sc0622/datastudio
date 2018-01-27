#ifndef EDITTREEVIEW_H
#define EDITTREEVIEW_H

#include <QWidget>

namespace Icd { class CoreTreeWidget; }

namespace Edit {

class TreeView : public QWidget
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = nullptr);
    ~TreeView();

    bool init();

signals:

public slots:

private:
    bool updateParser();

private:
    Icd::CoreTreeWidget *d_treeView;
};

}

#endif // EDITTREEVIEW_H
