#ifndef EXPORTORGDATAWIDGET_H
#define EXPORTORGDATAWIDGET_H

#include <QWidget>
#include "icdcore/icdcore_global.h"

namespace Icd {
class Object;
typedef JHandlePtr<Object> ObjectPtr;
class Table;
typedef JHandlePtr<Table> TablePtr;
class Item;
typedef JHandlePtr<Item> ItemPtr;
}

class ExportOrgDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExportOrgDataWidget(const QString &filePath,
                                 const Icd::TablePtr &table,
                                 const Icd::ObjectPtr &objectItem,
                                 bool hasTimeFormat, int headerSize,
                                 int bitOffset, QWidget *parent = nullptr);

signals:
    void rejected();

public slots:

private:
    void exportData(const QString &target, bool exportTime);

private:
    QString d_filePath;
    Icd::TablePtr d_table;
    Icd::ObjectPtr d_objectItem;
    bool d_hasTimeFormat;
    int d_headerSize;
    int d_bitOffset;
};

#endif // EXPORTORGDATAWIDGET_H
