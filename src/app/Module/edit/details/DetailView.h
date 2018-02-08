#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include <QWidget>
#include "main_global.h"

class JSplitter;
class QStandardItem;

namespace Icd {
class Object;
typedef JHandlePtr<Object> ObjectPtr;
}

namespace Edit {

class DetailTable;
class DetailItemEdit;

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

private:
    JSplitter *d_splitterMain;
    DetailTable *d_detailTable;
    DetailItemEdit *d_detailItemEdit;
    struct {
        QStandardItem *item;
        Icd::ObjectPtr object;
    } d_currentData;
};

}

#endif // DETAILVIEW_H
