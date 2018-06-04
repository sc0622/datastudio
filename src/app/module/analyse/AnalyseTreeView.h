#ifndef ANALYSETREEVIEW_H
#define ANALYSETREEVIEW_H

#include <QWidget>

class QTemporaryFile;

namespace Icd {

class CoreTreeWidget;

template<typename T> class std::shared_ptr;

class Table;
typedef std::shared_ptr<Table> TablePtr;

class Item;
typedef std::shared_ptr<Item> ItemPtr;

}

class QStandardItem;
class ExportDataDlg;

namespace Analyse {

class TreeView : public QWidget
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = nullptr);
    ~TreeView();

    bool init();

    void setShowAttribute(int attr, bool on = true);
    void loadRecordData();
    void unloadRecordData();

signals:

public slots:

private:
    bool updateParser();
    bool parseDataFile(const QString &filePath);
    bool loadData(const QString &domain, const QString &filePath,
                  bool hasTimeFormat, int headerSize);
    void exportData(const QStandardItem *item, const QString &filePath,
                    bool hasTimeFormat, int headerSize);
    QString generateTitle(const Icd::ObjectPtr &item) const;

private:
    Icd::CoreTreeWidget *d_treeView;
    Icd::TablePtr d_table;
    ExportDataDlg *d_exportDataDlg;
    QMap<QString/*filePath*/, Icd::TablePtr> d_fileTables;
    QMap<QString/*filePath*/, QSharedPointer<QTemporaryFile>> d_tempFiles;
};

}

#endif // ANALYSETREEVIEW_H
