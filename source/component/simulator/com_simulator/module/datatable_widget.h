#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H

#include <QWidget>

namespace Icd {

template<typename T> class JHandlePtr;

class Item;
typedef JHandlePtr<Item> ItemPtr;

class Worker;
typedef JHandlePtr<Worker> WorkerPtr;

class Object;
typedef JHandlePtr<Object> ObjectPtr;

class Table;
typedef JHandlePtr<Table> TablePtr;

}   // end of namespace Icd

// class DataTableWidget

class DataItemWidget;
class QStandardItem;
class JTableView;

class DataTableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataTableWidget(QWidget *parent = 0);

    ~DataTableWidget();

    /**
     * @brief setWorker
     * @param worker
     */
    void setWorker(const Icd::WorkerPtr &worker);

    /**
     * @brief findItem
     * @param dataItemId
     * @return
     */
    DataItemWidget *findDataItem(const QString &dataItemId);

    /**
     * @brief addDataItem
     * @param domain
     * @param worker
     * @param object
     * @return
     */
    bool addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                     const Icd::ObjectPtr &object);

    /**
     * @brief focusItem
     * @param domain
     * @return
     */
    Q_INVOKABLE int focusItem(const QString &domain);

    /**
     * @brief removeItem
     * @param domain
     */
    Q_INVOKABLE void removeItem(const QString &domain);

    /**
     * @brief contentsHeight
     * @return
     */
    int contentsHeight() const;

    /**
     * @brief itemTable
     * @return
     */
    QStandardItem *itemTable() const;

    /**
     * @brief setItemTable
     * @param item
     */
    void setItemTable(QStandardItem *item);

    /**
     * @brief rowCount
     * @return
     */
    int rowCount() const;

signals:
    void itemRemoved(const QString &identity);
    void heightChanged(int height);

public slots:

private:
    /**
     * @brief addDataItem
     * @param domain
     * @param dataItem
     * @param item
     * @return
     */
    bool addDataItem(const QString &domain, const Icd::ItemPtr &dataItem,
                     QStandardItem *item);

private:
    Icd::WorkerPtr d_worker;
    JTableView *d_tableView;
    QStandardItem *d_itemTable;
};

#endif // DATATABLEWIDGET_H
