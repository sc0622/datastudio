#ifndef COMMONITOR_H
#define COMMONITOR_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ComMonitorUi;

class ComMonitor
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public IJMessageSink
        , public JObserver
{
public:
    explicit ComMonitor(IJAttempter* attemper);
    virtual ~ComMonitor();

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
    JLRESULT onTreeItemClicked(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onCurrentItemChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onTreeItemUnloaded(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onChannelBound(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onChannelUnbound(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onChannelChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onUnbindItem(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onSetPlay(const std::string &id, JWPARAM wParam, JLPARAM lParam);

private:
    IJAttempter *d_attempter;
    ComMonitorUi *d_ui;
};

#endif // COMMONITOR_H
