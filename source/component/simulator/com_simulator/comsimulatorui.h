#ifndef COMSIMULATORUI_H
#define COMSIMULATORUI_H

#include <QMainWindow>

class IJAttempter;
class MainViewWidget;
class QStandardItem;

class ComSimulatorUi : public QMainWindow
{
    Q_OBJECT
public:
    explicit ComSimulatorUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComSimulatorUi();

    void releaseWork();

Q_SIGNALS:

public Q_SLOTS:
    void onTreeItemClicked(QStandardItem *item);
    void onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void onChannelUnbound(QStandardItem *item, const QString &channelId);
    void onChannelChanged(QStandardItem *item, const QString &channelId);
    void onUnbindItem(QStandardItem *item);

private:
    IJAttempter &d_attempter;
    QWidget *d_centralWidget;
    MainViewWidget *d_viewWidget;
};

#endif // COMSIMULATORUI_H
