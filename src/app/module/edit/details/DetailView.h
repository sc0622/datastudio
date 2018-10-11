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
    void updateView(QStandardItem *item);
    void removeRow(QStandardItem *item);
    void cleanItem(QStandardItem *item);

signals:
    void modifiedChanged(bool modified);

public slots:
    void setModified(bool modified);

    void onCurrentItemChanged(const QVariant &index, const Icd::ObjectPtr &newObject);
    void onRowMoved(int previousRow, int currentRow, bool restore);
    void onApplied();
    void onCanceled();

private:
    void insertRow(int row, QStandardItem *item, const QVariant &data);
    bool saveObject();

    void setAddEnabled(bool enabled);
    void setUpEnabled(bool enabled);
    void setDownEnabled(bool enabled);
    void setActionEnabled(const QString &action, bool enabled);
    void updateMoveActionState();

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
