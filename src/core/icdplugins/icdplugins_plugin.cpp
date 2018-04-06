#include "precomp.h"
#include "icdplugins_plugin.h"
#include <qqml.h>
#include "icdmeta/icdmeta_global.h"
#include <QDebug>

#ifndef PLUGIN_URI
#define PLUGIN_URI "Icd.Core"
#endif

void IcdPluginsPlugin::registerTypes(const char *uri)
{
    // @uri Icd.Core
    Q_UNUSED(uri);
    Q_ASSERT(uri == PLUGIN_URI);
    IcdCore::registerQmlType();
}
