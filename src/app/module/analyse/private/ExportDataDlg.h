#ifndef EXPORTDATADLG_H
#define EXPORTDATADLG_H

#include <QDialog>
#include "icdcore/icdcore_global.h"

namespace Icd {

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Table;
typedef std::shared_ptr<Table> TablePtr;

class Item;
typedef std::shared_ptr<Item> ItemPtr;
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
    QString filePath_;
    Icd::TablePtr table_;
    Icd::ObjectPtr objectItem_;
    bool hasTimeFormat_;
    int headerSize_;
    int bitOffset_;
    QTabWidget *tabWidget_;
    ExportOrgDataWidget *exportOrgDataWidget_;
    ConvertToDataAnalyse *convertToDataAnalyse_;
};

#endif // EXPORTDATADLG_H
