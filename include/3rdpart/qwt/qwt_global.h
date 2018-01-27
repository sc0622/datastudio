/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_GLOBAL_H
#define QWT_GLOBAL_H

#include <qglobal.h>

// QWT_VERSION is (major << 16) + (minor << 8) + patch.

#define QWT_VERSION       0x060102
#define QWT_VERSION_STR   "6.1.2"

#if defined(_MSC_VER) /* MSVC Compiler */
/* template-class specialization 'identifier' is already instantiated */
#pragma warning(disable: 4660)
/* inherits via dominance */
#pragma warning(disable: 4250)
#endif // _MSC_VER
#ifdef QWT_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef QWT_BUILD
#       define QWT_EXPORT  Q_DECL_EXPORT
#   else
#       define QWT_EXPORT  Q_DECL_IMPORT
#       if defined(_DEBUG) || defined(DEBUG)
#           pragma comment(lib, "qwtd.lib")
#       else
#           pragma comment(lib, "qwt.lib")
#       endif
#   endif // !QWT_BUILD
#endif // _MSC_VER || ...
#endif // QWT_LIB

#ifndef QWT_EXPORT
#define QWT_EXPORT
#endif

#endif 
