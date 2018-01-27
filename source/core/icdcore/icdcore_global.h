#ifndef ICDCORE_GLOBAL_H
#define ICDCORE_GLOBAL_H

#include <string>
#include <list>

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

// - private pointer
#ifndef J_DECLARE_PRIVATE
#define J_DECLARE_PRIVATE(name) \
    name ## Private *d_ptr; \
    Q_DECLARE_PRIVATE(name)
#endif

#ifndef J_DECLARE_PUBLIC
#define J_DECLARE_PUBLIC(name) \
    name *q_ptr; \
    Q_DECLARE_PUBLIC(name)
#endif

#ifndef J_DECLARE_SINGLE_INSTANCE
#define J_DECLARE_SINGLE_INSTANCE(Class) \
    public: \
    static Class *instance(); \
    static void releaseInstance(); \
    private: \
    static Class *_instance;
#endif

#ifndef J_IMPLEMENT_SINGLE_INSTANCE
#define J_IMPLEMENT_SINGLE_INSTANCE(Class) \
    Class *Class::_instance = 0; \
    \
    Class *Class::instance() { \
    if (Class::_instance == 0) { \
    Class::_instance = new Class; \
    } \
    return Class::_instance; \
    } \
    \
    void Class::releaseInstance() { \
    if (Class::_instance != 0) { \
    delete Class::_instance; \
    Class::_instance = 0; \
    } \
    }
#endif

////////////////////////////////

// for shared_ptr, unique_ptr
#include <memory>

#ifndef J_HANDLEPTR
#define J_HANDLEPTR

#define JHandlePtr ::std::shared_ptr
#define JUniquePtr ::std::unique_ptr

#ifndef JHandlePtrCast
#define JHandlePtrCast ::std::dynamic_pointer_cast
#endif

#ifndef J_FRIEND_HANDLEPTR
#define J_FRIEND_HANDLEPTR() \
    template<typename T> friend class ::std::_Ref_count; \
    template<typename T> friend class ::std::shared_ptr;
#endif //J_FRIEND_HANDLEPTR

#endif // J_HANDLEPTR

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
    static bool parse(const std::string &filePath, Json::Value &rootJson, bool create = false);
    static Json::Value value(const Json::Value &rootJson, const std::string &domain, bool create = false);
    static Json::Value value(const std::string &filePath, const std::string &domain, bool create = false);
    static bool setValue(const std::string &filePath, const std::string &domain,
                         const Json::Value &value, bool create = false, bool fast = true);

    static bool merge(const Json::Value &source, Json::Value &target);

private:
    static bool setValue(Json::Value &parentJson, const std::string &domain,
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

void ICDCORE_EXPORT splitString(const std::string &str, const std::string &delim,
                                std::list<std::string> &ret);
std::string ICDCORE_EXPORT trimString(const std::string &str);
std::string &replaceString(std::string &str, const std::string &old_str,
                           const std::string &new_str);

} // end of namespace Icd

#endif // ICDCORE_GLOBAL_H
