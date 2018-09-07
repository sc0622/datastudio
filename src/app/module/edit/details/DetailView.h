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

    void updateView(QStandardItem *item);

signals:

public slots:
    void onCurrentItemChanged(const QVariant &index);

private:
    JSplitter *splitterMain_;
    DetailTable *detailTable_;
    DetailEdit *detailEdit_;
    struct {
        QStandardItem *item;
        Icd::ObjectPtr object;
    } currentData_;
};

}

#endif // DETAILVIEW_H
