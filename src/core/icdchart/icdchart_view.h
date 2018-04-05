#ifndef ICDCHART_VIEW_H
#define ICDCHART_VIEW_H

#include <QWidget>
#include "icdchart_global.h"

namespace JChart { class View; }

class QTreeWidgetItem;

namespace Icd {

class Worker;
typedef JHandlePtr<Worker> WorkerPtr;

class ChartViewPrivate;

class ICDCHART_EXPORT ChartView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool xAxisSync READ xAxisSync WRITE setXAxisSync NOTIFY xAxisSyncChanged)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
public:
    explicit ChartView(QWidget *parent = nullptr);
    explicit ChartView(bool styled, QWidget *parent = nullptr);
    virtual ~ChartView();

    JChart::View *chartView() const;
    void changeWorker(const QString &domain, const Icd::WorkerPtr &worker);
    void focusItem(const QString &domain);
    void removeItem(const QString &domain);
    void removeChart(const QString &domain);
    void clearCharts();
    bool xAxisSync() const;
    bool isRunning() const;
    void showYLabel(bool checked);
    void showYAlign(bool align);
    void setYLabelLength(int length);
    void setColumnCount(int count);
    void setColumnWidthSame();
    void setSyncTrack(bool enabled);
    void setChartTheme(int chartTheme);

signals:
    void xAxisSyncChanged(bool value);
    void runningChanged(bool value);

public slots:
    void setXAxisSync(bool value);
    void setRunning(bool value);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Q_DISABLE_COPY(ChartView)
    J_DECLARE_PRIVATE(ChartView)
};

}

#endif // ICDCHART_VIEW_H
