#ifndef COMCHANNELMGR_H
#define COMCHANNELMGR_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ComChannelMgrUi;

class ComChannelMgr
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public IJMessageSink
        , public JObserver
{
public:
    explicit ComChannelMgr(IJAttempter* attemper);
    virtual ~ComChannelMgr();

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

private:
    IJAttempter *d_attempter;
    ComChannelMgrUi *d_ui;
};

#endif // COMCHANNELMGR_H
