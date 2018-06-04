#ifndef PRECOMP_H
#define PRECOMP_H

#ifdef QT_CORE_LIB
#include <qglobal.h>
#include <QString>
#include <QtCore>
#include <QDebug>
#endif

#ifdef _AFXDLL
#include <afxwin.h>
#elif (defined(WIN32) && defined(DEBUG))
#include <Windows.h>
#endif

#ifdef QT_GUI_LIB
#include <QtGui>
#endif

#ifdef QT_SQL_LIB
#include <QtSql>
#endif

#ifdef QT_XML_LIB
#include <QtXml>
#endif

#ifdef QT_CONCURRENT_LIB
#include <QtConcurrent>
#endif

#ifdef QT_DBUS_LIB
#include <QtDBus>
#endif

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#endif

#ifdef QT_WINEXTRAS_LIB
#include <QtWinExtras>
#endif

#ifdef QT_CONCURRENT_LIB
#include <QtConcurrent>
#endif

#ifdef QT_SERIALPORT_LIB
#include <QtSerialPort>
#endif

#if (QT_VERSION < 0x050000)

#ifdef QT_DECLARATIVE_LIB
#include <QtDeclarative/QtDeclarative>
#endif

#else

#ifdef QT_QUICK_LIB
#include <QtQuick>
#endif

#endif

#ifdef QT_SERIALPORT_LIB
#include <QtSerialPort>
#endif

// from jframework - 3rdpart

#if defined(TINYXML_LIB) && !defined(TINYXML_BUILD)
#include "tinyxml/tinyxml.h"
#endif

#if defined(QTWINMIGRATE_LIB) && !defined(QTWINMIGRATE_BUILD)
#include "qtwinmigrate/qmfcapp.h"
#endif

#if defined(QTRIBBON_LIB) && !defined(QTRIBBON_BUILD)
#include "qtribbon/QtRibbon.h"
#endif

#if defined(LOG4CPP_LIB) && !defined(LOG4CPP_BUILD)
#include "log4cpp/convenience.h"
#endif

#if defined(JWT_LIB) && !defined(JWT_BUILD)
#include "jwt/jwt_inc.h"
#endif

#if defined(JCHART_LIB) && !defined(JCHART_BUILD)
#include "jchart/jchart_inc.h"
#endif

#if defined(JUTRALEDIT_LIB) && !defined(JUTRALEDIT_BUILD)
#include "jutraledit/jutraledit_inc.h"
#endif

#if defined(JENCRYPT_LIB) && !defined(JENCRYPT_BUILD)
#include "jencrypt/jencrypt_inc.h"
#endif

#if defined(QT_FTP_LIB) && !defined(QT_FTP_BUILD)
#include "qtftp/qftp.h"
#include "qtftp/qurlinfo.h"
#endif

#if defined(QT_QTTELNET_LIB) && !defined(QT_QTTELNET_BUILD)
#include "QtTelnet/qttelnet.h"
#endif

#if defined(JFRAME_FACADE_LIB) && !defined(JFRAME_FACADE_BUILD)
#include "jframe_facade/jframe_facade.h"
#endif

#if defined(JFRAME_FACTORY_LIB) && !defined(JFRAME_FACTORY_BUILD)
#include "jframe_factory/jframe_factory.h"
#include "jframe_factory/jnotifier.h"
#endif

#if defined(JFRAME_KERNEL_LIB) && !defined(JFRAME_KERNEL_BUILD)
#include "jframe_kernel/jframe_kernel.h"
#include "jframe_kernel/jframe_core.h"
#include "jframe_kernel/jframe_layout.h"
#include "jframe_kernel/jframe_login.h"
#endif

// from datastudio - core

#if defined(ICDCORE_LIB) && !defined(ICDCORE_BUILD)
#include "icdcore/icdcore_inc.h"
#if defined(ICDCORE_JSON_LIB)
#include "icdcore/3rdpart/jsoncpp/json_tool.h"
#include "icdcore/3rdpart/jsoncpp/json/json.h"
#endif
#endif

#if defined(ICDCORE_BUILD)
#include "json/json.h"
#endif

#if defined(ICDPARSER_LIB) && !defined(ICDPARSER_BUILD)
#include "icdparser/icdparser_inc.h"
#endif

#if defined(ICDCOMM_LIB) && !defined(ICDCOMM_BUILD)
#include "icdcomm/icdcomm_inc.h"
#endif

#if defined(ICDWORKER_LIB) && !defined(ICDWORKER_BUILD)
#include "icdworker/icdworker_inc.h"
#endif

#if defined(ICDCHART_LIB) && !defined(ICDCHART_BUILD)
#include "icdchart/icdchart_inc.h"
#endif

#if defined(ICDWIDGET_LIB) && !defined(ICDWIDGET_BUILD)
#include "icdwidget/icdwidget_inc.h"
#endif

#if defined(ICDMETA_LIB) && !defined(ICDMETA_BUILD)
#include "icdmeta/icdmeta_inc.h"
#endif

// from nodestudio - core

#if defined(NODECORE_LIB) && !defined(NODECORE_BUILD)
#include "nodecore/nodecore_inc.h"
#endif

#ifdef PROJECT_APP
#include "main_global.h"
#endif

#ifdef QT_CORE_LIB
//
static void jdelete_qobject(QObject *object)
{
    delete object;
}

// nodejs
#ifdef BUILDING_NODE_EXTENSION
#include <napi.h>
#endif

#endif

#endif // PRECOMP_H
