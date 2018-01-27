#ifndef _LOG4CPP_EXPORT_HH
#define _LOG4CPP_EXPORT_HH

#ifdef LOG4CPP_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#	ifdef LOG4CPP_BUILD
#       define LOG4CPP_EXPORT __declspec(dllexport)
#   else
#       define LOG4CPP_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "log4cppd.lib")
#       else
#           pragma comment(lib, "log4cpp.lib")
#       endif
#   endif // !LOG4CPP_BUILD
#endif // _MSC_VER || ...
#endif // LOG4CPP_LIB

#ifndef LOG4CPP_EXPORT
#define LOG4CPP_EXPORT
#endif

#endif // _LOG4CPP_EXPORT_HH

