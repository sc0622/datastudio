#ifndef EXPORTDATADLG_H
#define EXPORTDATADLG_H

#include <QDialog>
#include "icdcore/icdcore_global.h"

namespace Icd {

class Object;
typedef JHandlePtr<Object> ObjectPtr;

class Table;
typedef JHandlePtr<Table> TablePtr;

class Item;
typedef JHandlePtr<Item> ItemPtr;
}

class QTabWidget;
class ExportOrgDataWidget;
class ConvertToDataAnalyse;

class ExportDataDlg : public QDialog
{
    Q_OBJECT
public:
    ExportDataDlg(const QString &filePath,
                  const Icd::TablePtr &table,
                  const Icd::ObjectPtr &objectItem,
                  bool hasTimeFormat, int headerSize,
                  int bitOffset, QWidget *parent = nullptr);
    ~ExportDataDlg();

signals:

public slots:

private:
    QString d_filePath;
    Icd::TablePtr d_table;
    Icd::ObjectPtr d_objectItem;
    bool d_hasTimeFormat;
    int d_headerSize;
    int d_bitOffset;
    QTabWidget *d_tabWidget;
    ExportOrgDataWidget *d_exportOrgDataWidget;
    ConvertToDataAnalyse *d_convertToDataAnalyse;
};

#endif // EXPORTDATADLG_H
