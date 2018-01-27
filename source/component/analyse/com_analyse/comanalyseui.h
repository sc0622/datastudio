#ifndef COMANALYSEUI_H
#define COMANALYSEUI_H

#include <QWidget>

class IJAttempter;
class MainViewWidget;
class QStandardItem;

namespace Icd {
template<typename T> class JHandlePtr;
class Table;
typedef JHandlePtr<Table> TablePtr;
}

class ComAnalyseUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComAnalyseUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComAnalyseUi();

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

Q_SIGNALS:

public Q_SLOTS:

private:
    IJAttempter &d_attempter;
    MainViewWidget *d_viewWidget;
};

#endif // COMANALYSEUI_H
