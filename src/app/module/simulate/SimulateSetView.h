#ifndef SIMULATESETVIEW_H
#define SIMULATESETVIEW_H

#include <QWidget>

namespace Icd {

template<typename T> class std::shared_ptr;

class Table;
typedef std::shared_ptr<Table> TablePtr;

class Parser;
typedef std::shared_ptr<Parser> ParserPtr;

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

}   // end of namespace Icd

class QStandardItem;
class QScrollArea;

namespace Simulate {

class IcdTabWidget;
class DataTableWidget;
class DataItemBox;

class SetView : public QWidget
{
    Q_OBJECT
public:
    explicit SetView(QWidget *parent = nullptr);
    ~SetView();

    bool init();

signals:
    void dropTriggled(QStandardItem *itemTable, QStandardItem *item, const QString &domain);

public slots:
    void onDropTriggled(QStandardItem *itemTable, QStandardItem *item, const QString &domain);

protected:
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

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

private:
    IcdTabWidget *d_tabVehicles;
};

}

#endif // SIMULATESETVIEW_H
