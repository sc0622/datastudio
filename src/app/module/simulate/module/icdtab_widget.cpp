#include "precomp.h"
#include "icdtab_widget.h"
#include "datatable_widget.h"
#include "icdcore/icdcore_inc.h"
#include "icdparser/icdparser_inc.h"
#include "icdworker/icdworker_pool.h"
#include "icdwidget/icdwidget_global.h"

namespace Simulate {

IcdTabWidget::IcdTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setElideMode(Qt::ElideMiddle);
    setTabsClosable(true);
    setMovable(true);

    tabBar()->setShape(QTabBar::TriangularNorth);
    tabBar()->setExpanding(true);
    //
    connect(this, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onTabCloseRequested(int)));
}

IcdTabWidget::~IcdTabWidget()
{

}

QWidget *IcdTabWidget::findTab(const QString &identity)
{
    //
    int count = this->count();
    for (int i = 0; i < count; ++i) {
        if (tabBar()->tabData(i).toString() == identity) {
            return widget(i);
        }
    }

    return 0;
}

void IcdTabWidget::removeTabById(const QString &identity)
{
    int count = this->count();
    for (int i = 0; i < count; ++i) {
        if (tabBar()->tabData(i).toString() == identity) {
            //
            QWidget *widget = this->widget(i);
            if (widget) {
                widget->disconnect(this);
                widget->deleteLater();
            }
            //
            removeTab(i);
            break;
        }
    }
}

void IcdTabWidget::onTabCloseRequested(int index)
{
    //
    QWidget *widget = this->widget(index);
    if (widget) {
        widget->disconnect(this);
        widget->deleteLater();
    }

    //
    removeTab(index);
}

void IcdTabWidget::tabRemoved(int index)
{
    Q_UNUSED(index);

    QTabWidget::tabRemoved(index);
}

QWidget *IcdTabWidget::setCurrentTab(const QString &identity)
{
    QWidget *widget = findTab(identity);
    if (widget) {
        setCurrentWidget(widget);
    }

    return widget;
}

bool IcdTabWidget::addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                               const Icd::ObjectPtr &object, QStandardItem *itemTable)
{
    if (!worker || !object || !itemTable) {
        return false;
    }
    //
    const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(object);
    if (!table) {
        return false;
    }
    //
    const QString tableId = QString::fromStdString(table->id());
    const QString name = QString::fromStdString(object->name());
    //
    DataTableWidget *tableWidget = qobject_cast<DataTableWidget *>(findTab(tableId));
    if (!tableWidget) {
        tableWidget = new DataTableWidget(this);
        tableWidget->setItemTable(itemTable);
        //
        const Icd::Item *const parentItem = dynamic_cast<Icd::Item *>(object->parent());
        if (parentItem && parentItem->type() == Icd::ItemFrame) {
            //
            const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(object);
            if (!table) {
                return false;
            }
            //
            addTab(tableWidget, QString("%1 (%2)")
                   .arg(QString::fromStdString(table->name()))
                   .arg(QString::fromStdString(table->mark())));
        } else {
            addTab(tableWidget, name);
        }
        tabBar()->setTabData(count() - 1, tableId);

        //
        connect(tableWidget, SIGNAL(itemRemoved(QString)), this, SIGNAL(itemRemoved(QString)));
        connect(tableWidget, SIGNAL(heightChanged(int)), this, SIGNAL(heightChanged(int)));
    }
    //
    setCurrentWidget(tableWidget);
    //
    tableWidget->addDataItem(domain.section('/', 1), worker, object);

    return true;
}

int IcdTabWidget::focusItem(const QString &domain)
{
    //
    if (domain.isEmpty()) {
        return 0;
    }
    //
    const QString id = domain.section('/', 0, 0);
    if (id.isEmpty()) {
        return 0;
    }
    //
    QWidget *widget = findTab(id);
    if (!widget) {
        return 0;
    }
    //
    setCurrentWidget(widget);
    //
    const QString section = domain.section('/', 1);
    if (section.isEmpty()) {
        return 0;
    }
    //
    int offset = 0;
    if (!QMetaObject::invokeMethod(widget, "focusItem", Q_RETURN_ARG(int, offset),
                                   Q_ARG(QString, section))) {
        return 0;
    }

    return offset + tabBar()->height();
}

void IcdTabWidget::removeItem(const QString &domain)
{
    //
    if (domain.isEmpty()) {
        return;
    }
    //
    const QString id = domain.section('/', 0, 0);
    if (id.isEmpty()) {
        return;
    }
    //
    const QString section = domain.section('/', 1);
    //
    QWidget *widget = 0;
    int count = this->count();
    for (int i = 0; i < count; ++i) {
        if (tabBar()->tabData(i).toString() == id) {
            widget = this->widget(i);
            if (section.isEmpty()) {
                widget->disconnect();
                delete widget;
                removeTab(i);
                return;
            }
            break;
        }
    }
    //
    if (!widget) {
        return;
    }
    //
    if (!QMetaObject::invokeMethod(widget, "removeItem", Q_ARG(QString, section))) {
        return;
    }
}

}
