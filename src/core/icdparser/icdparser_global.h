#ifndef ICDPARSER_GLOBAL_H
#define ICDPARSER_GLOBAL_H

#ifdef ICDPARSER_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDPARSER_BUILD
#       define ICDPARSER_EXPORT __declspec(dllexport)
#   else
#       define ICDPARSER_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "icdparserd.lib")
#       else
#           pragma comment(lib, "icdparser.lib")
#       endif
#   endif // !ICDPARSER_BUILD
#endif // _MSC_VER || ...
#endif // ICDPARSER_LIB

#ifndef ICDPARSER_EXPORT
#define ICDPARSER_EXPORT
#endif

#ifdef ICDCORE_LIB
#include "icdcore/icdcore_global.h"
#endif

////////////////////////////////

namespace Icd {

class Parser;
typedef JHandlePtr<Parser> ParserPtr;

struct JParserPtrHandle
{
    ParserPtr parser;
};

#ifdef QT_CORE_LIB
#include <QMetaType>
Q_DECLARE_METATYPE(JParserPtrHandle)
#endif

} // end of namespace Icd

#endif // ICDPARSER_GLOBAL_H
