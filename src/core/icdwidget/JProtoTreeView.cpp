#include "precomp.h"
#include "JProtoTreeView.h"
#include "private/JProtoTreeView_p.h"

namespace Icd {

// class JProtoTreeView

JProtoTreeView::JProtoTreeView(QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new JProtoTreeViewPrivate(this))
{
    Q_D(JProtoTreeView);
    d->init();
}

JProtoTreeView::~JProtoTreeView()
{
    Q_D(JProtoTreeView);
    delete d;
}

JProtoTreeView::BindTableTypes JProtoTreeView::bindTableType() const
{
    Q_D(const JProtoTreeView);
    return d->bindTableTypes_;
}

void JProtoTreeView::setBindTableType(BindTableTypes value)
{
    Q_D(JProtoTreeView);
    if (value != d->bindTableTypes_) {
        d->bindTableTypes_ = value;
        // emit
    }
}

JProtoTreeView::ShowAttributes JProtoTreeView::showAttributes() const
{
    Q_D(const JProtoTreeView);
    return d->showAttributes();
}

void JProtoTreeView::setShowAttributes(JProtoTreeView::ShowAttributes attrs)
{
    Q_D(JProtoTreeView);
    d->setShowAttributes(attrs);
}

void JProtoTreeView::setShowAttribute(JProtoTreeView::ShowAttribute attr, bool on)
{
    Q_D(JProtoTreeView);
    d->setShowAttribute(attr, on);
}

bool JProtoTreeView::testShowAttribute(JProtoTreeView::ShowAttribute attr) const
{
    Q_D(const JProtoTreeView);
    return d->testShowAttribute(attr);
}

int JProtoTreeView::dataFormat() const
{
    Q_D(const JProtoTreeView);
    return d->dataFormat();
}

JProtoTreeView::TreeModes JProtoTreeView::treeModes() const
{
    Q_D(const JProtoTreeView);
    return JProtoTreeView::TreeModes(d->treeModes());
}

void JProtoTreeView::setTreeMode(TreeModes modes)
{
    Q_D(JProtoTreeView);
    d->setTreeMode(modes);
}

void JProtoTreeView::setTreeMode(JProtoTreeView::TreeMode mode, bool on)
{
    Q_D(JProtoTreeView);
    d->setTreeMode(mode, on);
}

bool JProtoTreeView::testTreeMode(JProtoTreeView::TreeMode mode) const
{
    Q_D(const JProtoTreeView);
    return d->testTreeMode(mode);
}

Icd::ParserPtr JProtoTreeView::parser() const
{
    Q_D(const JProtoTreeView);
    return d->parser_;
}

void JProtoTreeView::setParser(const Icd::ParserPtr &parser)
{
    Q_D(JProtoTreeView);
    d->setParser(parser);
}

int JProtoTreeView::loadingDeep() const
{
    Q_D(const JProtoTreeView);
    return d->loadingDeep();
}

void JProtoTreeView::setLoadingDeep(int deep)
{
    Q_D(JProtoTreeView);
    d->setLoadingDeep(deep);
}

int JProtoTreeView::intervalUpdate() const
{
    Q_D(const JProtoTreeView);
    return d->intervalUpdate();
}

void JProtoTreeView::setIntervalUpdate(int interval)
{
    Q_D(JProtoTreeView);
    d->setIntervalUpdate(interval);
}

void JProtoTreeView::clearContents()
{
    Q_D(JProtoTreeView);
    //
    d->clearData();
}

bool JProtoTreeView::loadData()
{
    Q_D(JProtoTreeView);
    return d->loadData();
}

bool JProtoTreeView::loadData(const TablePtr &table, const QString &domain)
{
    Q_D(JProtoTreeView);
    return d->loadData(table, domain);
}

bool JProtoTreeView::loadData(const TablePtr &table, const QString &filePath,
                              bool hasTimeFormat, int headerSize, const QString &domain)
{
    Q_D(JProtoTreeView);
    return d->loadData(table, filePath, hasTimeFormat, headerSize, domain);
}

QStandardItem *Icd::JProtoTreeView::currentItem() const
{
    Q_D(const JProtoTreeView);
    return d->currentItem();
}

bool JProtoTreeView::isRunning() const
{
    Q_D(const JProtoTreeView);
    return d->isRunning();
}

void JProtoTreeView::bindingChannels(const QString &filePath)
{
    Q_D(JProtoTreeView);
    d->bindingChannels(filePath);
}

void JProtoTreeView::unbindingChannels()
{
    Q_D(JProtoTreeView);
    d->unbindingChannels();
}

void JProtoTreeView::exportBindingStatus(const QString &filePath)
{
    Q_D(JProtoTreeView);
    d->exportBindingStatus(filePath);
}

void JProtoTreeView::runAllChannels()
{
    Q_D(JProtoTreeView);
    d->runAllChannels();
}

void JProtoTreeView::stopAllChannels()
{
    Q_D(JProtoTreeView);
    d->stopAllChannels();
}

QStandardItem *JProtoTreeView::findItemTable(QStandardItem *item) const
{
    Q_D(const JProtoTreeView);
    return d->findItemTable(item);
}

void JProtoTreeView::selectItem(const QString &domain, int domainType)
{
    Q_D(JProtoTreeView);
    return d->selectItem(domain, domainType);
}

QString JProtoTreeView::itemDomain(QStandardItem *item, int domainType) const
{
    Q_D(const JProtoTreeView);
    return d->itemDomain(item, domainType);
}

QString JProtoTreeView::idDomain(QStandardItem *item)
{
    return JProtoTreeViewPrivate::idDomain(item);
}

QString JProtoTreeView::markDomain(QStandardItem *item)
{
    return JProtoTreeViewPrivate::markDomain(item);
}

bool JProtoTreeView::loadTable(JTreeView *treeView, QStandardItem *itemParent, const TablePtr &table)
{
    return JProtoTreeViewPrivate::loadSystem(treeView, itemParent, table, Icd::ObjectItem);
}

Icd::RootPtr JProtoTreeView::protoRoot() const
{
    Q_D(const JProtoTreeView);
    return d->protoRoot_;
}

void JProtoTreeView::setProtoRoot(const RootPtr &root)
{
    Q_D(JProtoTreeView);
    if (root == d->protoRoot_) {
        return;
    }

    if (root) {
        d->loadData();
    } else {
        d->clearData();
    }
}

bool JProtoTreeView::isItemLoaded(QStandardItem *item) const
{
    if (!item) {
        return false;
    }

    const QVariant varLoad = item->data(Icd::TreeLoadStatusRole);
    if (varLoad.isValid()) {
        return varLoad.toBool();
    } else {
        return false;
    }
}

bool JProtoTreeView::hasUnloadedItem() const
{
    Q_D(const JProtoTreeView);
    return d->hasUnloadedItem();
}

void JProtoTreeView::setRunning(bool value)
{
    Q_D(JProtoTreeView);
    if (value != d->isRunning()) {
        d->setRunning(value);
        emit runningChanged(value);
    }
}

void JProtoTreeView::setDataFormat(int format)
{
    Q_D(JProtoTreeView);
    d->setDataFormat(format);
}

} // end of namespace Icd
