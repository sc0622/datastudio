#ifndef COMICDSYSTEM_H
#define COMICDSYSTEM_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ComIcdSystemUi;
class QFileSystemWatcher;

class ComIcdSystem
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public IJMessageSink
        , public JObserver
{
public:
    explicit ComIcdSystem(IJAttempter* attemper);
    virtual ~ComIcdSystem();

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
    JLRESULT onModuleChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT queryIcdParser(const std::string &id, JWPARAM wParam, JLPARAM lParam);

private:
    bool setRibbonCurrentPage(const QString &pageName);

private:
    IJAttempter *d_attempter;
    QFileSystemWatcher *fileWatcher;
    ComIcdSystemUi *d_ui;
};

#endif // COMICDSYSTEM_H
