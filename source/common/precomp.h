#ifndef PRECOMP_H
#define PRECOMP_H

#include <qglobal.h>
#include <QString>

#ifdef _AFXDLL
#include <afxwin.h>
#elif (defined(WIN32) && defined(DEBUG))
#include <Windows.h>
#endif

#ifdef QT_CORE_LIB
#include <QtCore>
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

#if (QT_VERSION < 0x050000)

#include <QtDeclarative/QtDeclarative>

#else

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#endif

#ifdef QT_WINEXTRAS_LIB
#include <QtWinExtras>
#endif

#ifdef QT_QUICK_LIB
#include <QtQuick>
#endif

#endif

#if defined(JFRAME_FACADE_LIB)
#include "jframe_facade.h"
#endif

#if defined(JFRAME_FACTORY_LIB)
#include "factory/jframe_factory.h"
#include "factory/jnotifier.h"
#endif

#if defined(JFRAME_KERNEL_LIB)
#include "kernel/jframe_kernel.h"
#include "kernel/jframe_core.h"
#include "kernel/jframe_layout.h"
#include "kernel/jframe_login.h"
#endif

#if defined(TINYXML_LIB)
#include "tinyxml/tinyxml.h"
#endif

#if defined(QTWINMIGRATE_LIB)
#include "qtwinmigrate/qmfcapp.h"
#endif

#if defined(QTRIBBON_LIB)
#include "QtRibbon/QtRibbon.h"
#endif

#if defined(LOG4CPP_LIB)
#include "log4cpp/convenience.h"
#endif

#if defined(JWT_LIB)
#include "jwt/jwt_inc.h"
#endif

#if defined(JCHART_LIB)
#include "jchart/jchart_inc.h"
#endif

#if defined(JUTRALEDIT_LIB)
#include "jutraledit/jutraledit_inc.h"
#endif

// customer

#if defined(ICDCORE_LIB) && !defined(ICDCORE_BUILD)
#include "icdcore/icdcore_inc.h"
#if defined(ICDCORE_JSON_LIB)
#include "icdcore/3rdpart/jsoncpp/json/json.h"
#endif
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

#if defined(KERNELCLASS_LIB) && !defined(KERNELCLASS_BUILD)
//
#endif

#if defined(DBACCESS_LIB) && !defined(DBACCESS_BUILD)
//
#endif

#if defined(JSONCPP_INC)
#include "json/json.h"
#endif

#ifndef J_DELETE_QOBJECT
#define J_DELETE_QOBJECT
static void j_delete_qobject(QObject *object)
{ delete object; }
#endif // J_DELETE_QOBJECT

#endif // PRECOMP_H
