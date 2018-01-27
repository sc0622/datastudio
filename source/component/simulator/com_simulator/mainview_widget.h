#ifndef MAINVIEWWIDGET_H
#define MAINVIEWWIDGET_H

#include <QFrame>

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

// class MainViewWidget

class IcdTabWidget;
class QScrollArea;
class QStandardItem;
class DataTableWidget;
class DataItemBox;

class MainViewWidget : public QFrame
{
    Q_OBJECT
public:
    explicit MainViewWidget(QWidget *parent = 0);

signals:
    void dropTriggled(QStandardItem *itemTable, QStandardItem *item, const QString &domain);

public slots:
    void onDropTriggled(QStandardItem *itemTable, QStandardItem *item, const QString &domain);
    void onTreeItemClicked(QStandardItem *item);
    void onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void onChannelUnbound(QStandardItem *item, const QString &channelId);
    void onChannelChanged(QStandardItem *item, const QString &channelId);
    void onUnbindItem(QStandardItem *item);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);

private:

    // -- change --
    void changeWorker(const QString &vehicleId, const QString &systemId,
                      const QString &tableId, const Icd::WorkerPtr &worker);
    // -- find --
    IcdTabWidget *findTabVehicle(const QString &vehicleId);
    IcdTabWidget *findTabSystem(const QString &vehicleId, const QString &systemId);
    DataTableWidget *findTabTable(const QString &vehicleId, const QString &systemId,
                                  const QString &tableId);
    // -- add --
    IcdTabWidget *addTabVehicle(const QString &domain);
    IcdTabWidget *addTabSystem(const QString &domain);
    bool addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                     const Icd::ObjectPtr &object, QStandardItem *itemTable);
    // -- focus --
    int focusItem(const QString &domain);
    // -- remove --
    void removeItem(const QString &domain);

    Icd::Parser *queryIcdParser() const;

private:
    IcdTabWidget *d_tabVehicles;
    QScrollArea *scrollArea;
    QWidget *widgetCharts;
};

#endif // MAINVIEWWIDGET_H
