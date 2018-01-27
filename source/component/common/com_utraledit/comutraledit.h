#ifndef ComUtralEdit_H
#define ComUtralEdit_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ComUtralEditUi;

class ComUtralEdit
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public IJMessageSink
        , public JObserver
{
public:
    explicit ComUtralEdit(IJAttempter* attemper);
    virtual ~ComUtralEdit();

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
    JLRESULT queryShowUtralEdit(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onTreeItemClicked(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onCurrentItemChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onChannelUnbound(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT onChannelChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);

private:
    IJAttempter *d_attempter;
    ComUtralEditUi *d_ui;
};

#endif // ComUtralEdit_H
