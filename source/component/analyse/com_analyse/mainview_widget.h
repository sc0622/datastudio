#ifndef MAINVIEWWIDGET_H
#define MAINVIEWWIDGET_H

#include <QFrame>

class ChartFileView;
class QStandardItem;

namespace Icd {
template<typename T> class JHandlePtr;
class Table;
typedef JHandlePtr<Table> TablePtr;
}

/**
 * @brief The ChartViewWidget class
 */
class MainViewWidget : public QFrame
{
    Q_OBJECT
public:
    explicit MainViewWidget(QWidget *parent = 0);

    bool init();
    void releaseWork();
    bool resetChart();
    void onTreeItemClicked(QStandardItem *item);
    void onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void onUnbindItem(QStandardItem *item, QStandardItem *itemTable);
    void onTableLoaded(const QString &filePath, const Icd::TablePtr &table);
    void setXAxisSync(bool enabled);
    void showChartYLabel(bool checked);
    void showChartYAlign(bool align);
    void setSyncTrack(bool enabled);
    void setYLabelLength(int length);
    void setColumnCount(int count);
    void updateData();
    void toggleReplay(bool enabled);
    void clearCharts();
    void setColumnWidthSame();
    void setReplayPeriod(int interval);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    Icd::ChartFileView *d_chartView;
};

#endif // MAINVIEWWIDGET_H
