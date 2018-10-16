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

#if defined(ICDCORE_LIB) || defined(BUILDING_NODE_EXTENSION) || defined(BUILDING_APP)
#include "icdcore/icdcore_global.h"
#endif

////////////////////////////////

#ifndef J_DECLARE_SINGLE_INSTANCE
#define J_DECLARE_SINGLE_INSTANCE(Class) \
    public: \
        static Class *instance(); \
        static void releaseInstance(); \
    private: \
        static Class *_instance;
#endif

#ifndef J_IMPLEMENT_SINGLE_INSTANCE
#define J_IMPLEMENT_SINGLE_INSTANCE(Class, GlobalClass) \
    \
    static void __ ## Class ## _releaseInstance() { \
        Class::releaseInstance(); \
    } \
    Class *Class::_instance = nullptr; \
    \
    Class *Class::instance() { \
        if (Class::_instance == nullptr) { \
            Class::_instance = new Class; \
        } \
        if (std::string(J_STRINGIFY(Class)) != #GlobalClass) { \
            GlobalClass::instance()->registerSingletonRelease(__ ## Class ## _releaseInstance); \
        } \
        return Class::_instance; \
    } \
    \
    void Class::releaseInstance() { \
        if (Class::_instance != nullptr) { \
            delete Class::_instance; \
            Class::_instance = nullptr; \
        } \
    }
#endif

#ifndef J_SINGLE_RELEASE_CALLBACK
#define J_SINGLE_RELEASE_CALLBACK
typedef void(*SingletonReleaseCallback)();
#endif

namespace Icd {

class Parser;
typedef std::shared_ptr<Parser> ParserPtr;

struct JParserPtrHandle
{
    ParserPtr parser;
};

} // end of namespace Icd

// class IcdParser

class IcdParserPrivate;

class ICDPARSER_EXPORT IcdParser
{
public:
    bool init();
    void registerSingletonRelease(SingletonReleaseCallback callback);

private:
    explicit IcdParser();
    ~IcdParser();

private:
    IcdParserPrivate *d;
    J_DECLARE_SINGLE_INSTANCE(IcdParser)
};

#endif // ICDPARSER_GLOBAL_H
