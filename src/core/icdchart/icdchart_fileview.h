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
    /**
     * @brief ChartFileView
     * @param parent
     */
    explicit ChartFileView(QWidget *parent = 0);

    ~ChartFileView();

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
     * @brief reset
     */
    void reset();

    /**
     * @brief xAxisSync
     * @return
     */
    bool xAxisSync() const;

    /**
     * @brief isReplaying
     * @return
     */
    bool isReplaying() const;

    /**
     * @brief replayPeriod
     * @return
     */
    int replayPeriod() const;

    /**
     * @brief clearCharts
     */
    void clearCharts();

    /**
     * @brief addTable
     * @param filePath
     * @param table
     */
    void addTable(const QString &filePath, const Icd::TablePtr &table);

    /**
     * @brief removeTable
     * @param filePath
     */
    void removeTable(const QString &filePath);

    /**
     * @brief tableByDomain
     * @param filePath
     * @return
     */
    Icd::TablePtr tableByFilePath(const QString &filePath) const;

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

} // end of namespace Icd

#endif // ICDCHART_FILEVIEW_H
