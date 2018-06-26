#ifndef ICDCORE_GLOBAL_H
#define ICDCORE_GLOBAL_H

#include <string>
#include <vector>

#ifdef ICDCORE_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDCORE_BUILD
#       define ICDCORE_EXPORT __declspec(dllexport)
#   else
#ifndef JSON_DLL
#       define JSON_DLL
#endif
#       define ICDCORE_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "icdcored.lib")
#       else
#           pragma comment(lib, "icdcore.lib")
#       endif
#   endif // !ICDCORE_BUILD
#endif // _MSC_VER || ...
#endif // ICDCORE_LIB

#ifndef ICDCORE_EXPORT
#define ICDCORE_EXPORT
#endif

////////////////////////////////

#ifdef __unix__
#if __cplusplus >= 201103L
#  include <exception>              // std::exception
#  include <typeinfo>               // std::type_info in get_deleter
#  include <iosfwd>                 // std::basic_ostream
#  include <ext/atomicity.h>
#  include <ext/concurrence.h>
#  include <bits/functexcept.h>
#  include <bits/stl_function.h>  // std::less
#  include <bits/uses_allocator.h>
#  include <type_traits>
#  include <functional>
#  include <debug/debug.h>
#  include <bits/unique_ptr.h>
#  include <bits/shared_ptr.h>
#  if _GLIBCXX_USE_DEPRECATED
#    include <backward/auto_ptr.h>
#  endif
#else
#  include <backward/auto_ptr.h>
#endif
typedef float float_t;
typedef double double_t;
#endif

#if defined(__apple__)
typedef float float_t;
typedef double double_t;
#endif

// for shared_ptr, unique_ptr
#include <memory>
//template<class T> using JHandlePtr = std::shared_ptr<T>;
//template<class T> using JUniquePtr = std::unique_ptr<T>;
#ifndef JHandlePtrCast
#define JHandlePtrCast std::dynamic_pointer_cast
#endif

#ifndef J_FRIEND_HANDLEPTR
#define J_FRIEND_HANDLEPTR() \
    template<typename T> friend class std::_Ref_count; \
    template<typename T> friend class std::shared_ptr;
#endif

#ifndef J_TYPEDEF_SHAREDPTR
#define J_TYPEDEF_SHAREDPTR(_class_) \
    class _class_; \
    typedef JHandlePtr<_class_> _class_ ## Ptr; \
    typedef std::vector<_class_ ## Ptr> _class_ ## PtrArray;
#endif

////////////////////////////////

//
#ifdef ICDCORE_JSON_LIB
#include "./3rdpart/jsoncpp/json_tool.h"
#else
namespace Json { class Value; }
#endif

//
namespace Icd {

#ifndef DEF_ICD_TYPE
#define DEF_ICD_TYPE

typedef char icd_int8;
typedef unsigned char icd_uint8;
typedef short icd_int16;
typedef unsigned short icd_uint16;
typedef int icd_int32;
typedef unsigned int icd_uint32;
#ifdef _MSC_VER
typedef /*__int64*/long long icd_int64;
typedef unsigned /*__int64*/long long icd_uint64;
#else
typedef long long icd_int64;
typedef unsigned long long icd_uint64;
#endif
typedef float icd_float;
typedef double icd_double;
typedef float icd_float32;
typedef double icd_float64;

#endif // DEF_ICD_TYPE

//
#ifndef J_HANDLE_SCOPE
#define J_HANDLE_SCOPE
template<typename T> struct JHandleScope { std::shared_ptr<T> ptr; };
#endif

//
#ifndef ICD_LIMIT_METHODS
#define ICD_LIMIT_METHODS
template <typename T>
inline const T &jMin(const T &a, const T &b) { return (a < b) ? a : b; }
template <typename T>
inline const T &jMax(const T &a, const T &b) { return (a < b) ? b : a; }
template <typename T>
inline const T &jBound(const T &min, const T &val, const T &max)
{ return jMax(min, jMin(max, val)); }
#endif

//
bool ICDCORE_EXPORT startsWith(const std::string &str, const std::string &header);
bool ICDCORE_EXPORT endsWith(const std::string &str, const std::string &tail);
int ICDCORE_EXPORT atoi(const std::string &str);
icd_uint64 ICDCORE_EXPORT strtou64(const std::string &str, int radix);
icd_uint64 ICDCORE_EXPORT atou64(const std::string &str);
std::string ICDCORE_EXPORT itoa(int value, bool hex = false);
std::string ICDCORE_EXPORT u64toa(icd_uint64 value, bool hex = false);
double ICDCORE_EXPORT atod(const std::string &str);
std::string ICDCORE_EXPORT dtoa(double value);

std::string ICDCORE_EXPORT stringSection(const char *ch, char sep, int start = 0, int end = -1);
std::string ICDCORE_EXPORT stringSection(const std::string &str, char sep, int start = 0, int end = -1);
std::string ICDCORE_EXPORT stringSection(const char *ch, const std::string &sep,
                                         int start = 0, int end = -1);
std::string ICDCORE_EXPORT stringSection(const std::string &str, const std::string &sep,
                                         int start = 0, int end = -1);
void ICDCORE_EXPORT splitString(const std::string &str, const std::string &delim,
                                std::vector<std::string> &ret, bool keepEmptyParts = false);
std::string ICDCORE_EXPORT trimString(const std::string &str);
std::string ICDCORE_EXPORT &replaceString(std::string &str, const std::string &old_str,
                                          const std::string &new_str);
int ICDCORE_EXPORT createPath(const std::string &path);
std::string ICDCORE_EXPORT pathOfFile(const std::string &filePath);

int ICDCORE_EXPORT asciiCountOfSize(int format, int size);

} // end of namespace Icd

#endif // ICDCORE_GLOBAL_H
