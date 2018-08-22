#ifndef SIMULATETREEVIEW_H
#define SIMULATETREEVIEW_H

#include <QWidget>

namespace Icd { class JProtoTreeView; }

namespace Simulate {

class TreeView : public QWidget
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = nullptr);
    ~TreeView();

    bool init();

    void setShowAttribute(int attr, bool on = true);
    void setOrigValueRadix(int radix);

signals:

public slots:

private:
    bool updateParser();

private:
    Icd::JProtoTreeView *treeView_;
};

}

#endif // SIMULATETREEVIEW_H
