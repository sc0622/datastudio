#ifndef EDITTREEVIEW_H
#define EDITTREEVIEW_H

#include <QWidget>
#include <memory>

namespace Icd {
class JProtoTreeView;
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
}

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
    void save(bool saveAs);
    QString nameDomainOfObject(const Icd::ObjectPtr &object) const;

private:
    Icd::JProtoTreeView *treeView_;
};

}

#endif // EDITTREEVIEW_H
