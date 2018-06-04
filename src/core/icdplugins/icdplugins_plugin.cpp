#include "precomp.h"
#include "icdplugins_plugin.h"
#include <qqml.h>
#include <QQmlEngine>
#include <QFontDatabase>
#include "icdmeta/icdmeta_global.h"
#include <QDebug>

static int registerQmlType(const char *path, const char *uri, const char *typeName)
{
    static const QString prefix = "qrc:/imports/";
    return qmlRegisterType(QUrl(prefix + path), uri, ICDMETA_VER_MAJOR, ICDMETA_VER_MINOR, typeName);
}

IcdPluginsPlugin::IcdPluginsPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
    //Q_INIT_RESOURCE(resource);
    Q_INIT_RESOURCE(icdplugins);
}

IcdPluginsPlugin::~IcdPluginsPlugin()
{
    //Q_CLEANUP_RESOURCE(resource);
    Q_CLEANUP_RESOURCE(icdplugins);
}

void IcdPluginsPlugin::registerTypes(const char *uri)
{
    // @uri Icd.Core
    IcdCore::registerQmlType();
    // utilityb
    registerQmlType("utility/CircualProgress.qml", uri, "CircualProgress");
    registerQmlType("utility/DataChannelButton.qml", uri, "DataChannelButton");
    registerQmlType("utility/HScrollBar.qml", uri, "HScrollBar");
    registerQmlType("utility/HScrollIndicator.qml", uri, "HScrollIndicator");
    registerQmlType("utility/IP4AddressBox.qml", uri, "IP4AddressBox");
    registerQmlType("utility/JChannelButton.qml", uri, "JChannelButton");
    registerQmlType("utility/JDelayProgress.qml", uri, "JDelayProgress");
    registerQmlType("utility/JItemDelegate.qml", uri, "JItemDelegate");
    registerQmlType("utility/JSearchObject.qml", uri, "JSearchObject");
    registerQmlType("utility/JTabButton.qml", uri, "JTabButton");
    registerQmlType("utility/JTableViewColumn.qml", uri, "JTableViewColumn");
    registerQmlType("utility/MainButton.qml", uri, "MainButton");
    registerQmlType("utility/ScrollBarPage.qml", uri, "ScrollBarPage");
    registerQmlType("utility/ScrollIndicatorPage.qml", uri, "ScrollIndicatorPage");
    registerQmlType("utility/SystemButton.qml", uri, "SystemButton");
    registerQmlType("utility/VScrollBar.qml", uri, "VScrollBar");
    registerQmlType("utility/VScrollIndicator.qml", uri, "VScrollIndicator");
    // utility/chart
    registerQmlType("utility/chart/QChart.qml", uri, "QChart");
    // utility/controls
    registerQmlType("utility/controls/JButton.qml", uri, "JButton");
    registerQmlType("utility/controls/JCmdButton.qml", uri, "JCmdButton");
    registerQmlType("utility/controls/JDialField.qml", uri, "JDialField");
    registerQmlType("utility/controls/JDialLabel.qml", uri, "JDialLabel");
    registerQmlType("utility/controls/JGauge.qml", uri, "JGauge");
    registerQmlType("utility/controls/JGroupBox.qml", uri, "JGroupBox");
    registerQmlType("utility/controls/JLineChart.qml", uri, "JLineChart");
    registerQmlType("utility/controls/JSlider.qml", uri, "JSlider");
    registerQmlType("utility/controls/JSpinBox.qml", uri, "JSpinBox");
    registerQmlType("utility/controls/JTextLabel.qml", uri, "JTextLabel");
    registerQmlType("utility/controls/JValueLabel.qml", uri, "JValueLabel");
    registerQmlType("utility/controls/RippleRectangle.qml", uri, "RippleRectangle");
    registerQmlType("utility/controls/SendValuePane.qml", uri, "SendValuePane");
    // utility/ftp
    registerQmlType("utility/ftp/DownloadingDialog.qml", uri, "DownloadingDialog");
    registerQmlType("utility/ftp/FtpDetailView.qml", uri, "FtpDetailView");
    registerQmlType("utility/ftp/FtpListView.qml", uri, "FtpListView");
    registerQmlType("utility/ftp/FtpMainView.qml", uri, "FtpMainView");
    registerQmlType("utility/ftp/FtpPage.qml", uri, "FtpPage");
    // utility/gauge
    registerQmlType("utility/gauge/JCircularGauge.qml", uri, "JCircularGauge");
    // utility/stackdrawer
    registerQmlType("utility/stackdrawer/StackDrawer.qml", uri, "StackDrawer");
}

void IcdPluginsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    //
    engine->addImportPath("qrc:/imports/");
    //
    const QString font(baseUrl().toLocalFile() + "/imports/fonts/NotoSansHans-Bold.otf");
    int result = QFontDatabase::addApplicationFont(font);
    if (result == -1) {
        qDebug() << "addApplicationFont:" << result << " (" + font + ")";
    }
}
