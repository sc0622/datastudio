#ifndef COMANALYSE_H
#define COMANALYSE_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ComAnalyseUi;

class ComAnalyse
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public IJMessageSink
        , public JObserver
{
public:
    explicit ComAnalyse(IJAttempter* attemper);
    virtual ~ComAnalyse();

    // IJUnknown interface
public:
    bool loadInterface();
    bool updateInterface();
    void releaseInterface();
    void *queryInterface(const std::string &iid, unsigned int ver);

    // IJComponent interface
public:
    std::string componentName() const;
    std::string componentDesc() const;
    void attach();
    void detach();

    // IJComponentUi interface
public:
    void *createWindow(void *parent, const std::string &objectName);

    // IJCommandSink interface
public:
    bool commandSink(void *sender, const std::string &domain, const std::string &objectName,
                     const std::string &eventType, void *data);

    // IJMessageSink interface
public:
    bool messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam);

    // JObserver interface
public:
    std::string observerId() const;

protected:
    JLRESULT onResetChart(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onDataSourceChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onTreeItemClicked(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onCurrentItemChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onTreeItemUnloaded(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onUnbindItem(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onTableLoaded(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT setReplayState(const std::string &id, JWPARAM wParam, JLPARAM lParam);

    void setReplayState(QObject *target, bool checked);

private:
    IJAttempter *d_attempter;
    ComAnalyseUi *d_ui;
};

#endif // COMANALYSE_H
