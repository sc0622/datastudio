#ifndef ICDWORKER_GLOBAL_H
#define ICDWORKER_GLOBAL_H

#ifdef ICDWORKER_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDWORKER_BUILD
#       define ICDWORKER_EXPORT __declspec(dllexport)
#   else
#       define ICDWORKER_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "icdworkerd.lib")
#       else
#           pragma comment(lib, "icdworker.lib")
#       endif
#   endif // !ICDWORKER_BUILD
#endif // _MSC_VER || ...
#endif // ICDWORKER_LIB

#ifndef ICDWORKER_EXPORT
#define ICDWORKER_EXPORT
#endif

#ifdef ICDCOMM_LIB
#include "icdcomm/icdcomm_global.h"
#endif

#ifdef QT_CORE_LIB

#include <qglobal.h>

// - private pointer
#ifdef J_DECLARE_PRIVATE
#undef J_DECLARE_PRIVATE
#endif
#define J_DECLARE_PRIVATE(name) \
    Q_DECLARE_PRIVATE(name) \
    name ## Private *d_ptr;

#ifdef J_DECLARE_PUBLIC
#undef J_DECLARE_PUBLIC
#endif
#define J_DECLARE_PUBLIC(name) \
    Q_DECLARE_PUBLIC(name) \
    name *q_ptr;

#endif

////////////////////////////////

namespace Icd {

} // end of namespace Icd

#endif // ICDWORKER_GLOBAL_H
