#pragma once

#include <QQmlExtensionPlugin>

class IcdPluginsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    IcdPluginsPlugin(QObject *parent = nullptr);
    ~IcdPluginsPlugin();
    void registerTypes(const char *uri) override;
    void initializeEngine(QQmlEngine *engine, const char *uri) override;

private:
};
