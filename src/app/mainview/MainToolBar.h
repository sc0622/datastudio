#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include <QToolBar>

namespace Json { class Value; }

namespace Main {

class ToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit ToolBar(QWidget *parent = nullptr);

    bool init();

signals:

public slots:
    void update(int index);

private:
    void bindingNotify();

    // prefer
    void updatePrefer();

    // edit
    void updateEdit();
    void addEditViewAction(const Json::Value &option);
    void addEditItemAction(const Json::Value &option);
    void addEditWindowAction(const Json::Value &option);
    void addEditSettingsAction();

    // monitor
    void updateMonitor();
    void addMonitorViewAction(const Json::Value &option);
    QAction *addMonitorOrigValueRadixAction(QAction *action, const Json::Value &option);
    void addMonitorFlushSwitchAction(const Json::Value &option);
    void addMonitorTreeAction(const Json::Value &option);
    void addMonitorChartAction(const Json::Value &option);
    void addMonitorWindowAction(const Json::Value &option);
    void addMonitorSettingsAction();

    // simulate
    void updateSimulate();
    void addSimulateViewAction(const Json::Value &option);
    QAction *addSimulateOrigValueRadixAction(QAction *action, const Json::Value &option);
    void addSimulateFlushSwitchAction(const Json::Value &option);
    void addSimulateTreeAction(const Json::Value &option);
    void addSimulateSetAction(const Json::Value &option);
    void addSimulateWindowAction(const Json::Value &option);
    void addSimulateSettingsAction();

    // analyse
    void updateAnalyse();
    void addAnalyseViewAction(const Json::Value &option);
    void addAnalyseTreeAction(const Json::Value &option);
    void addAnalyseChartAction(const Json::Value &option);
    void addAnalyseWindowAction(const Json::Value &option);
    void addAnalyseSettingsAction();

private:
};

}

#endif // MAINTOOLBAR_H
