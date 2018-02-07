#ifndef ICDCORE_GLOBAL_H
#define ICDCORE_GLOBAL_H

#include <string>
#include <vector>

#ifdef ICDCORE_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDCORE_BUILD
#       define ICDCORE_EXPORT __declspec(dllexport)
#   else
#       define JSON_DLL
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

// for shared_ptr, unique_ptr
#include <memory>

#ifdef JHandlePtr
#undef JHandlePtr
#endif
#define JHandlePtr ::std::shared_ptr

#ifdef JUniquePtr
#undef JUniquePtr
#endif
#define JUniquePtr ::std::unique_ptr

#ifdef JHandlePtrCast
#undef JHandlePtrCast
#endif
#define JHandlePtrCast ::std::dynamic_pointer_cast

#ifdef J_FRIEND_HANDLEPTR
#undef J_FRIEND_HANDLEPTR
#endif
#define J_FRIEND_HANDLEPTR() \
    template<typename T> friend class ::std::_Ref_count; \
    template<typename T> friend class ::std::shared_ptr;

#ifndef J_TYPEDEF_SHAREDPTR
#undef J_TYPEDEF_SHAREDPTR
#endif
#define J_TYPEDEF_SHAREDPTR(_class_) \
    class _class_; \
    typedef JHandlePtr<_class_> _class_ ## Ptr; \
    typedef std::vector<_class_ ## Ptr> _class_ ## PtrArray;

////////////////////////////////

namespace Json { class Value; }

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
typedef __int64 icd_int64;
typedef unsigned __int64 icd_uint64;
#else
typedef long long icd_int64;
typedef unsigned long long icd_uint64;
#endif
typedef float icd_float;
typedef double icd_double;
typedef float icd_float32;
typedef double icd_float64;

#endif // DEF_ICD_TYPE

// class Serializable

class ICDCORE_EXPORT Serializable
{
public:
    virtual ~Serializable() {}
    virtual std::string saveJson() const;
    virtual Json::Value save() const = 0;
    virtual bool restore(const std::string &json);
    virtual bool restore(const Json::Value &/*json*/, int /*deep*/ = -1) = 0;
};

// class JJson;

class ICDCORE_EXPORT JJson
{
public:
    static bool resolve(const std::string &filePath, Json::Value &root);
    static Json::Value resolve(const std::string &filePath, const std::string &path);
    static Json::Value resolve(const Json::Value &root, const std::string &path);
    static bool make(const std::string &filePath, const Json::Value &root,
                     bool create, bool fast = false);
    static bool make(const std::string &filePath, const std::string &path,
                     const Json::Value &value, bool create, bool fast = false);
    static Json::Value make(const Json::Value &root, const std::string &path,
                            const Json::Value &value);
};

//
bool ICDCORE_EXPORT startsWith(const std::string &str, const std::string &header);
bool ICDCORE_EXPORT endsWith(const std::string &str, const std::string &tail);
int ICDCORE_EXPORT atoi(const std::string &str);
icd_uint64 ICDCORE_EXPORT atou64(const std::string &str);
std::string ICDCORE_EXPORT itoa(int value, bool hex = false);
std::string ICDCORE_EXPORT u64toa(icd_uint64 value, bool hex = false);
double ICDCORE_EXPORT atod(const std::string &str);
std::string ICDCORE_EXPORT dtoa(double value);

std::string ICDCORE_EXPORT stringSection(const std::string &str, char sep, int start = 0, int end = -1);
std::string ICDCORE_EXPORT stringSection(const std::string &str, const std::string &sep,
                                         int start = 0, int end = -1);
void ICDCORE_EXPORT splitString(const std::string &str, const std::string &delim,
                                std::vector<std::string> &ret, bool keepEmptyParts = false);
std::string ICDCORE_EXPORT trimString(const std::string &str);
std::string ICDCORE_EXPORT &replaceString(std::string &str, const std::string &old_str,
                                          const std::string &new_str);
int ICDCORE_EXPORT createPath(const std::string &path);
std::string ICDCORE_EXPORT pathOfFile(const std::string &filePath);

} // end of namespace Icd

#endif // ICDCORE_GLOBAL_H
