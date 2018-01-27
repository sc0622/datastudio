#include "icdplugins_plugin.h"
#include <qqml.h>
#include "icdmeta_global.h"
#include <QDebug>

void IcdPluginsSPlugin::registerTypes(const char *uri)
{
    // @uri Icd.Core
    Q_UNUSED(uri);
    IcdCore::registerQmlType();
}

