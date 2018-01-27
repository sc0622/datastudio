#ifndef MONITORTREEVIEW_H
#define MONITORTREEVIEW_H

#include <QWidget>

namespace Icd { class CoreTreeWidget; }

namespace Monitor {

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
    Icd::CoreTreeWidget *d_treeView;
};

}

#endif // MONITORTREEVIEW_H
