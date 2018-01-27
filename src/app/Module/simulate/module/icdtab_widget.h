#ifndef ICDTABWIDGET_H
#define ICDTABWIDGET_H

#include <QTabWidget>

namespace Icd {

template<typename T> class JHandlePtr;

class Table;
typedef JHandlePtr<Table> TablePtr;

class Parser;
typedef JHandlePtr<Parser> ParserPtr;

class Worker;
typedef JHandlePtr<Worker> WorkerPtr;

class Object;
typedef JHandlePtr<Object> ObjectPtr;

}   // end of namespace Icd

class QStandardItem;

namespace Simulate {

class DataTableWidget;

class IcdTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit IcdTabWidget(QWidget *parent = 0);

    ~IcdTabWidget();

    /**
     * @brief findTab
     * @param identity
     * @return
     */
    QWidget *findTab(const QString &identity);

    /**
     * @brief removeTab
     * @param identity
     */
    void removeTabById(const QString &identity);

    /**
     * @brief setCurrentTab
     * @param identity
     * @return
     */
    QWidget *setCurrentTab(const QString &identity);

    /**
     * @brief addDataItem
     * @param domain
     * @param worker
     * @param object
     * @param itemTable
     * @return
     */
    bool addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                     const Icd::ObjectPtr &object, QStandardItem *itemTable);

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

signals:
    void itemRemoved(const QString &identity);
    void heightChanged(int height);

public slots:
    void onTabCloseRequested(int index);

protected:
    void tabRemoved(int index);

private:
};

}

#endif // ICDTABWIDGET_H
