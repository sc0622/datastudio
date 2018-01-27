#ifndef COMSIMULATORNAV_H
#define COMSIMULATORNAV_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ComSimulatorNavUi;

class ComSimulatorNav
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public IJMessageSink
        , public JObserver
{
public:
    explicit ComSimulatorNav(IJAttempter* attemper);
    virtual ~ComSimulatorNav();

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
    JLRESULT onParserChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT queryLoadingDeep(const std::string &id, JWPARAM wParam, JLPARAM lParam);

private:
    IJAttempter *d_attempter;
    ComSimulatorNavUi *d_ui;
};

#endif // COMSIMULATORNAV_H
