#ifndef ICDCHART_VIEW_H
#define ICDCHART_VIEW_H

#include <QWidget>
#include "icdchart_global.h"

namespace JChart {
class View;
} // end of namespace JChart

class QTreeWidgetItem;

namespace Icd {

class Worker;
typedef JHandlePtr<Worker> WorkerPtr;

class ChartViewPrivate;

/**
 * @brief The ChartView class
 */
class ICDCHART_EXPORT ChartView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool xAxisSync READ xAxisSync WRITE setXAxisSync NOTIFY xAxisSyncChanged)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
public:
    /**
     * @brief ChartView
     * @param parent
     */
    explicit ChartView(QWidget *parent = 0);

    virtual ~ChartView();

    /**
     * @brief chartView
     * @return
     */
    JChart::View *chartView() const;

    /**
     * @brief changeWorker
     * @param domain
     * @param worker
     */
    void changeWorker(const QString &domain, const Icd::WorkerPtr &worker);

    /**
     * @brief focusItem
     * @param domain
     */
    void focusItem(const QString &domain);

    /**
     * @brief removeItem
     * @param domain
     */
    void removeItem(const QString &domain);

    /**
     * @brief removeChart
     * @param domain
     */
    void removeChart(const QString &domain);

    /**
     * @brief clearCharts
     */
    void clearCharts();

    /**
     * @brief xAxisSync
     * @return
     */
    bool xAxisSync() const;

    /**
     * @brief isRunning
     * @return
     */
    bool isRunning() const;

    /**
     * @brief showYLabel
     * @param checked
     */
    void showYLabel(bool checked);

    /**
     * @brief showYAlign
     * @param align
     */
    void showYAlign(bool align);

    /**
     * @brief setYLabelLength
     * @param length
     */
    void setYLabelLength(int length);

    /**
     * @brief setColumnCount
     * @param count
     */
    void setColumnCount(int count);

    /**
     * @brief setColumnWidthSame
     */
    void setColumnWidthSame();

    /**
     * @brief setSyncTrack
     * @param enabled
     */
    void setSyncTrack(bool enabled);

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

} // end of namespace Icd

#endif // ICDCHART_VIEW_H
