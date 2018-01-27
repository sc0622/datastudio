#ifndef JENCRYPT_GLOBAL_H
#define JENCRYPT_GLOBAL_H

///

#ifdef JENCRYPT_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef JENCRYPT_BUILD
#       define JENCRYPT_EXPORT __declspec(dllexport)
#   else
#       define JENCRYPT_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "jencryptd.lib")
#       else
#           pragma comment(lib, "jencrypt.lib")
#       endif
#   endif // !JENCRYPT_BUILD
#endif // _MSC_VER || ...
#endif // JENCRYPT_LIB

#ifndef JENCRYPT_EXPORT
#define JENCRYPT_EXPORT
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

#include <string>

namespace jencrypt {

typedef unsigned char	byte;
typedef unsigned int	uint32;

} // end of namespace jencrypt

#endif // JENCRYPT_GLOBAL_H
