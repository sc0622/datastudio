#ifndef MAINNAVWIDGET_H
#define MAINNAVWIDGET_H

#include <QWidget>
#include <QTreeWidget>

class QTemporaryFile;

namespace Icd {

class CoreTreeWidget;

template<typename T> class JHandlePtr;

class Table;
typedef JHandlePtr<Table> TablePtr;

class Item;
typedef JHandlePtr<Item> ItemPtr;

} // end of namespace Icd

// class MainNavWidget

class QStandardItem;
class ExportDataDlg;

class MainNavWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief MainNavWidget
     * @param parent
     */
    explicit MainNavWidget(QWidget *parent = 0);

    /**
     * @brief init
     * @return
     */
    bool init();

    /**
     * @brief releaseWork
     */
    void releaseWork();

    /**
     * @brief updateUi
     */
    void updateUi();

    void showItemOffset(bool checked);
    void showItemType(bool checked);
    void loadRecordData();
    void unloadRecordData();

signals:

public slots:

private:
    bool parseDataFile(const QString &filePath);
    bool loadData(const QString &domain, const QString &filePath,
                  bool hasTimeFormat, int headerSize);
    void exportData(const QStandardItem *item, const QString &filePath,
                    bool hasTimeFormat, int headerSize);
    QString generateTitle(const Icd::ObjectPtr &item) const;

private:
    Icd::CoreTreeWidget *d_icdTree;
    Icd::TablePtr d_table;
    ExportDataDlg *d_exportDataDlg;
    QMap<QString/*filePath*/, Icd::TablePtr> d_fileTables;
    QMap<QString/*filePath*/, QSharedPointer<QTemporaryFile>> d_tempFiles;
};

#endif // MAINNAVWIDGET_H
