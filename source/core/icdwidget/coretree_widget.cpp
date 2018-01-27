#include "precomp.h"
#include "coretree_widget.h"
#include "private/coretree_widget_p.h"

namespace Icd {

// class CoreTreeWidget

CoreTreeWidget::CoreTreeWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new CoreTreeWidgetPrivate(this))
{
    Q_D(CoreTreeWidget);
    d->init();
}

CoreTreeWidget::~CoreTreeWidget()
{
    Q_D(CoreTreeWidget);
    delete d;
}

CoreTreeWidget::BindTableTypes CoreTreeWidget::bindTableType() const
{
    Q_D(const CoreTreeWidget);
    return d->d_bindTableTypes;
}

void CoreTreeWidget::setBindTableType(BindTableTypes value)
{
    Q_D(CoreTreeWidget);
    if (value != d->d_bindTableTypes) {
        d->d_bindTableTypes = value;
        // emit
    }
}

CoreTreeWidget::ShowAttributes CoreTreeWidget::showAttributes() const
{
    Q_D(const CoreTreeWidget);
    return d->showAttributes();
}

void CoreTreeWidget::setShowAttributes(CoreTreeWidget::ShowAttributes attrs)
{
    Q_D(CoreTreeWidget);
    d->setShowAttributes(attrs);
}

void CoreTreeWidget::setShowAttribute(CoreTreeWidget::ShowAttribute attr, bool on)
{
    Q_D(CoreTreeWidget);
    d->setShowAttribute(attr, on);
}

bool CoreTreeWidget::testShowAttribute(CoreTreeWidget::ShowAttribute attr) const
{
    Q_D(const CoreTreeWidget);
    return d->testShowAttribute(attr);
}

int CoreTreeWidget::dataFormat() const
{
    Q_D(const CoreTreeWidget);
    return d->dataFormat();
}

CoreTreeWidget::TreeModes CoreTreeWidget::treeModes() const
{
    Q_D(const CoreTreeWidget);
    return d->treeModes();
}

void CoreTreeWidget::setTreeMode(TreeModes modes)
{
    Q_D(CoreTreeWidget);
    d->setTreeMode(modes);
}

void CoreTreeWidget::setTreeMode(CoreTreeWidget::TreeMode mode, bool on)
{
    Q_D(CoreTreeWidget);
    d->setTreeMode(mode, on);
}

bool CoreTreeWidget::testTreeMode(CoreTreeWidget::TreeMode mode) const
{
    Q_D(const CoreTreeWidget);
    return d->testTreeMode(mode);
}

Parser *CoreTreeWidget::parser() const
{
    Q_D(const CoreTreeWidget);
    return d->d_parser;
}

void CoreTreeWidget::setParser(Parser *parser)
{
    Q_D(CoreTreeWidget);
    d->setParser(parser);
}

int CoreTreeWidget::loadingDeep() const
{
    Q_D(const CoreTreeWidget);
    return d->loadingDeep();
}

void CoreTreeWidget::setLoadingDeep(int deep)
{
    Q_D(CoreTreeWidget);
    d->setLoadingDeep(deep);
}

int CoreTreeWidget::intervalUpdate() const
{
    Q_D(const CoreTreeWidget);
    return d->intervalUpdate();
}

void CoreTreeWidget::setIntervalUpdate(int interval)
{
    Q_D(CoreTreeWidget);
    d->setIntervalUpdate(interval);
}

void CoreTreeWidget::clearContents()
{
    Q_D(CoreTreeWidget);
    //
    d->clearData();
}

bool CoreTreeWidget::loadData()
{
    Q_D(CoreTreeWidget);
    return d->loadData();
}

bool CoreTreeWidget::loadData(const TablePtr &table, const QString &domain)
{
    Q_D(CoreTreeWidget);
    return d->loadData(table, domain);
}

bool CoreTreeWidget::loadData(const TablePtr &table, const QString &filePath,
                              bool hasTimeFormat, int headerSize, const QString &domain)
{
    Q_D(CoreTreeWidget);
    return d->loadData(table, filePath, hasTimeFormat, headerSize, domain);
}

QStandardItem *Icd::CoreTreeWidget::currentItem() const
{
    Q_D(const CoreTreeWidget);
    return d->currentItem();
}

bool CoreTreeWidget::isRunning() const
{
    Q_D(const CoreTreeWidget);
    return d->isRunning();
}

void CoreTreeWidget::bindingChannels(const QString &filePath)
{
    Q_D(CoreTreeWidget);
    d->bindingChannels(filePath);
}

void CoreTreeWidget::unbindingChannels()
{
    Q_D(CoreTreeWidget);
    d->unbindingChannels();
}

void CoreTreeWidget::exportBindingStatus(const QString &filePath)
{
    Q_D(CoreTreeWidget);
    d->exportBindingStatus(filePath);
}

void CoreTreeWidget::runAllChannels()
{
    Q_D(CoreTreeWidget);
    d->runAllChannels();
}

void CoreTreeWidget::stopAllChannels()
{
    Q_D(CoreTreeWidget);
    d->stopAllChannels();
}

void CoreTreeWidget::setRunning(bool value)
{
    Q_D(CoreTreeWidget);
    if (value != d->isRunning()) {
        d->setRunning(value);
        emit runningChanged(value);
    }
}

void CoreTreeWidget::setDataFormat(int format)
{
    Q_D(CoreTreeWidget);
    d->setDataFormat(format);
}

} // end of namespace Icd
