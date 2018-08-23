#ifndef JGROUPCHANNELPANE_H
#define JGROUPCHANNELPANE_H

#include <QDialog>
#include "icdwidget_global.h"
#include "icdworker/icdworker.h"
#include "JChannelPane.h"

class QTabWidget;

namespace Icd {

// class JGroupChannelPane

class JGroupChannelPanePrivate;

class ICDWIDGET_EXPORT JGroupChannelPane : public QWidget
{
    Q_OBJECT
public:
    explicit JGroupChannelPane(QWidget *parent = nullptr);
    explicit JGroupChannelPane(JChannelPane::OperateAttribute attrs, QWidget *parent = nullptr);
    virtual ~JGroupChannelPane();

    Icd::WorkerPtr selectedWorker() const;
    int currentTabIndex() const;
    int currentRowIndex() const;
    int currentRowCount() const;
    Icd::ChannelType currentChannelType() const;

    void addWorker(const Icd::WorkerPtr &worker);

    QTabWidget *tabWidget() const;

signals:
    void accepted();
    void rejected();

    void currentRowChanged(int rowIndex, int tabIndex);

public slots:
    void updateAllTab();
    void updateCurrentTab();
    void setCurrentTabIndex(int index);
    void setCurrentRowIndex(int index);

private:
    Q_DISABLE_COPY(JGroupChannelPane)
    J_DECLARE_PRIVATE(JGroupChannelPane)
};

// class JGroupChannelDlg

class ICDWIDGET_EXPORT JGroupChannelDlg : public QDialog
{
    Q_OBJECT
public:
    explicit JGroupChannelDlg(QWidget *parent = nullptr);
    explicit JGroupChannelDlg(JChannelPane::OperateAttribute attrs, QWidget *parent = nullptr);

    Icd::WorkerPtr selectedWorker() const;

    JGroupChannelPane *pane() const;

signals:

public slots:

private:
    void init();

private:
    JGroupChannelPane *pane_;
};

} // end of namespace Icd

#endif // JGROUPCHANNELPANE_H
