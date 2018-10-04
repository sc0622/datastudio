#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include <QWidget>
#include "main_global.h"

class JSplitter;
class QStandardItem;

namespace Icd {
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
}

namespace Edit {

class DetailTable;
class DetailEdit;

class DetailView : public QWidget
{
    Q_OBJECT
public:
    explicit DetailView(QWidget *parent = nullptr);
    ~DetailView();

    bool init();

    void requestAdd(QStandardItem *item, const QVariant &data);
    void updateView(QStandardItem *item);
    void removeRow(QStandardItem *item);
    void cleanItem(QStandardItem *item);

signals:

public slots:
    void onCurrentItemChanged(const QVariant &index, const Icd::ObjectPtr &newObject);
    void onApplied();
    void onCanceled();

private:
    void insertRow(int row, QStandardItem *item, const QVariant &data);

    bool saveObject();

private:
    JSplitter *splitterMain_;
    DetailTable *detailTable_;
    DetailEdit *detailEdit_;
    QStandardItem *treeItem_;
    Icd::ObjectPtr object_;
    Icd::ObjectPtr newObject_;
};

}

#endif // DETAILVIEW_H
