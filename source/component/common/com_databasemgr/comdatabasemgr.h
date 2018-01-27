#ifndef COMDATABASEMGR_H
#define COMDATABASEMGR_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

namespace Icd {

template<typename T> class JHandlePtr;
class Parser;
typedef JHandlePtr<Parser> ParserPtr;

}   // end of namespace Icd

class ComDatabaseMgrUi;

class ComDatabaseMgr
        : public IJComponent
        , public IJComponentUi
        , public IJCommandSink
        , public IJMessageSink
        , public JObserver
{
public:
    explicit ComDatabaseMgr(IJAttempter* attemper);
    virtual ~ComDatabaseMgr();

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
    JLRESULT onDataSourceChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT queryIcdParser(const std::string &id, JWPARAM wParam, JLPARAM lParam);

private:
    bool updateParser(const QString &sourceType, const QString &filePath = QString());

private:
    IJAttempter *d_attempter;
    ComDatabaseMgrUi *d_ui;
    Icd::ParserPtr d_icdParser;
};

#endif // COMDATABASEMGR_H
