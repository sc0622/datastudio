#ifndef ICDCHART_GLOBAL_H
#define ICDCHART_GLOBAL_H

#include <qglobal.h>

#ifdef ICDCHART_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDCHART_BUILD
#       define ICDCHART_EXPORT __declspec(dllexport)
#   else
#       define ICDCHART_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "icdchartd.lib")
#       else
#           pragma comment(lib, "icdchart.lib")
#       endif
#   endif // !ICDCHART_BUILD
#endif // _MSC_VER || ...
#endif // ICDCHART_LIB

#ifndef ICDCHART_EXPORT
#define ICDCHART_EXPORT
#endif

#ifdef ICDWIDGET_LIB
#include "icdwidget/icdwidget_global.h"
#endif

////////////////////////////////

namespace Icd {

} // end of namespace Icd

#endif // ICDCHART_GLOBAL_H
