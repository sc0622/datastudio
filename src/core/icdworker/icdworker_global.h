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

#if defined(ICDCOMM_LIB) || defined(BUILDING_NODE_EXTENSION)
#include "icdcomm/icdcomm_global.h"
#endif

#ifdef QT_CORE_LIB

#include <qglobal.h>

// - private pointer
#ifndef J_DECLARE_PRIVATE
#define J_DECLARE_PRIVATE(Class) \
    Class##Private *J_DPTR_; \
    inline Class##Private* d_func() { return J_DPTR_; } \
    inline const Class##Private* d_func() const { return J_DPTR_; } \
    friend class Class##Private;
#endif

#ifndef J_DECLARE_PUBLIC
#define J_DECLARE_PUBLIC(Class) \
    Class *J_QPTR_; \
    inline Class* q_func() { return J_QPTR_; } \
    inline const Class* q_func() const { return J_QPTR_; } \
    friend class Class;
#endif

#ifndef J_DPTR
#define J_DPTR J_DPTR_
#endif

#ifndef J_QPTR
#define J_QPTR J_QPTR_
#endif

#endif

////////////////////////////////

namespace Icd {

} // end of namespace Icd

#endif // ICDWORKER_GLOBAL_H
