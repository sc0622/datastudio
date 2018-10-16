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
    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modifiedChanged)
public:
    explicit DetailView(QWidget *parent = nullptr);
    ~DetailView();

    bool isModified() const;

    bool init();

    void requestAdd(QStandardItem *item, const QVariant &data);
    void moveCurrentRow(bool up);
    void updateView(QStandardItem *item, bool unloaded = false);
    void removeRow(QStandardItem *item);
    void clearItem(QStandardItem *item);

signals:
    void modifiedChanged(bool modified);

public slots:
    void setModified(bool modified);

    void onSelectedChanged(const QVariant &index, const Icd::ObjectPtr &newObject);
    void onRowMoved(int previousRow, int currentRow, bool restore);
    void onRequestInsert(int row, const QVariant &data);
    void onRequestPast(int row, const Icd::ObjectPtr &object, bool clone);
    void onApplied();
    void onCanceled();

private:
    void insertRow(int row, const QVariant &data);
    void insertRow(int row, const Icd::ObjectPtr &object, bool clone);
    bool saveObject();

    void setAddEnabled(bool enabled);
    void setUpEnabled(bool enabled);
    void setDownEnabled(bool enabled);
    void setActionEnabled(const QString &action, bool enabled);
    void updateMoveActionState();

    void cancelInsert(int row);

    Icd::ObjectPtr createNew(const Icd::ObjectPtr &copied, bool clone,
                             const Icd::ObjectPtr &parent = Icd::ObjectPtr());

private:
    JSplitter *splitterMain_;
    DetailTable *detailTable_;
    DetailEdit *detailEdit_;
    QStandardItem *treeItem_;
    bool modified_;
    Icd::ObjectPtr object_;
    Icd::ObjectPtr newObject_;
};

}

#endif // DETAILVIEW_H
