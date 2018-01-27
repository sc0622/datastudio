#ifndef COMMONITORUI_H
#define COMMONITORUI_H

#include <QWidget>

class IJAttempter;
class MainViewWidget;
class QStandardItem;

class ComMonitorUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComMonitorUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComMonitorUi();

    /**
     * @brief init
     * @return
     */
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

Q_SIGNALS:

public Q_SLOTS:
    void onTreeItemClicked(QStandardItem *item);
    void onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void onChannelUnbound(QStandardItem *item, const QString &channelId);
    void onChannelChanged(QStandardItem *item, const QString &channelId);
    void onUnbindItem(QStandardItem *item, QStandardItem *itemTable);
    void setPlay(bool running);

private:
    IJAttempter &d_attempter;
    MainViewWidget *d_viewWidget;
};

#endif // COMMONITORUI_H
