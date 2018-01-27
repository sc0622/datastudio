#ifndef MAINVIEWWIDGET_H
#define MAINVIEWWIDGET_H

#include <QFrame>

namespace Icd {
    class ChartView;
}

class QStandardItem;

/**
 * @brief The ChartViewWidget class
 */
class MainViewWidget : public QFrame
{
    Q_OBJECT
public:
    explicit MainViewWidget(QWidget *parent = 0);

    bool init();

    /**
     * @brief onChartOrder
     * @return
     */
    bool onChartOrder();

    /**
     * @brief releaseWork
     */
    void releaseWork();

    void clearCharts();
    void setColumnWidthSame();
    void showChartYLabel(bool checked);
    void showChartYAlign(bool align);
    void setSyncTrack(bool enabled);
    void setYLabelLength(int length);
    void setColumnCount(int count);

signals:

public slots:
    void onTreeItemClicked(QStandardItem *item);
    void onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void onChannelUnbound(QStandardItem *item, const QString &channelId);
    void onChannelChanged(QStandardItem *item, const QString &channelId);
    void onUnbindItem(QStandardItem *item, QStandardItem *itemTable);
    void setPlay(bool running);

protected:
    void paintEvent(QPaintEvent *event);

private:

private:
    Icd::ChartView *d_chartView;
};

#endif // MAINVIEWWIDGET_H
