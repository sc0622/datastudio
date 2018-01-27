#ifndef JUTRALEDIT_GLOBAL_H
#define JUTRALEDIT_GLOBAL_H

#include <qglobal.h>
#include <QString>

// JUTRALEDIT_VERSION is (major << 16) + (minor << 8) + patch.

#define JUTRALEDIT_VERSION       0x000001
#define JUTRALEDIT_VERSION_STR   "0.0.1"

#ifdef JUTRALEDIT_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef JUTRALEDIT_BUILD
#       define JUTRALEDIT_EXPORT  Q_DECL_EXPORT
#   else
#       define JUTRALEDIT_EXPORT  Q_DECL_IMPORT
#       if defined(_DEBUG) || defined(DEBUG)
#           pragma comment(lib, "jutraleditd.lib")
#       else
#           pragma comment(lib, "jutraledit.lib")
#       endif
#   endif // !JUTRALEDIT_BUILD
#endif // _MSC_VER || ...
#endif // JUTRALEDIT_LIB

#ifndef JUTRALEDIT_EXPORT
#define JUTRALEDIT_EXPORT
#endif

// - namespace JUTRALEDIT -
#define JUTRALEDIT_NAMESPACE JUTRALEDIT
#define JUTRALEDIT_BEGIN_NAMESPACE namespace JUTRALEDIT_NAMESPACE {
#define JUTRALEDIT_END_NAMESPACE }
#define JUTRALEDIT_USE_NAMESPACE using namespace JUTRALEDIT_NAMESPACE;

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

namespace JUtralEdit {

// - class JUtralEditCore -

class JUtralEditCorePrivate;

class JUTRALEDIT_EXPORT JUtralEditCore
{
public:
    static JUtralEditCore *instance();
    static void releaseInstance();

    void init();

private:
    explicit JUtralEditCore();
    ~JUtralEditCore();

private:
    JUtralEditCorePrivate *d;
};

} // end of namespace JUtralEdit

#endif // JUTRALEDIT_GLOBAL_H
