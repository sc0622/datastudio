#ifndef ICDCHART_FILEVIEW_H
#define ICDCHART_FILEVIEW_H

#include "icdchart_global.h"
#include <QWidget>

namespace Icd {

template<typename T> class HandlePtr;
class Table;
typedef JHandlePtr<Table> TablePtr;

// class ChartFileView

class ChartFileViewPrivate;

class ICDCHART_EXPORT ChartFileView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool xAxisSync READ xAxisSync WRITE setXAxisSync NOTIFY xAxisSyncChanged)
    Q_PROPERTY(bool replaying READ isReplaying NOTIFY replayingChanged)
    Q_PROPERTY(int replayPeriod READ replayPeriod WRITE setReplayPeriod NOTIFY replayPeriodChanged)
public:
    explicit ChartFileView(QWidget *parent = nullptr);
    explicit ChartFileView(bool styled, QWidget *parent = nullptr);
    ~ChartFileView();

    void focusItem(const QString &domain);
    void removeItem(const QString &domain);
    void removeChart(const QString &domain);
    void reset();
    bool xAxisSync() const;
    bool isReplaying() const;
    int replayPeriod() const;
    void clearCharts();
    void addTable(const QString &filePath, const Icd::TablePtr &table);
    void removeTable(const QString &filePath);
    Icd::TablePtr tableByFilePath(const QString &filePath) const;
    void showYLabel(bool checked);
    void showYAlign(bool align);
    void setYLabelLength(int length);
    void setColumnCount(int count);
    void setColumnWidthSame();
    void setSyncTrack(bool enabled);
    void setChartTheme(int chartTheme);

signals:
    void xAxisSyncChanged(bool value);
    void replayingChanged(bool replaying);
    void replayPeriodChanged(int value);
    void replayFinished();

public slots:
    void setXAxisSync(bool value);
    void setReplayPeriod(int value);
    void updateData();
    void startReplay();
    void stopReplay();
    void toggleReplay(bool enabled);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Q_DISABLE_COPY(ChartFileView)
    J_DECLARE_PRIVATE(ChartFileView)
};

}

#endif // ICDCHART_FILEVIEW_H
