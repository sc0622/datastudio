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

namespace Icd {

} // end of namespace Icd

#endif // ICDPARSER_GLOBAL_H
