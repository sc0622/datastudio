#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include <QWidget>
#include "main_global.h"

class JSplitter;

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

    bool updateView(const Icd::ObjectPtr &object);

signals:

public slots:

private:
    JSplitter *d_splitterMain;
    DetailTable *d_detailTable;
    DetailItemEdit *d_detailItemEdit;
};

}

#endif // DETAILVIEW_H
