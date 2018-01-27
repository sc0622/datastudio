#ifndef ICDCOMM_GLOBAL_H
#define ICDCOMM_GLOBAL_H

#ifdef ICDCOMM_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDCOMM_BUILD
#       define ICDCOMM_EXPORT __declspec(dllexport)
#   else
#       define ICDCOMM_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "icdcommd.lib")
#       else
#           pragma comment(lib, "icdcomm.lib")
#       endif
#   endif // !ICDCOMM_BUILD
#endif // _MSC_VER || ...
#endif // ICDCOMM_LIB

#ifndef ICDCOMM_EXPORT
#define ICDCOMM_EXPORT
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

namespace Icd {

} // end of namespace Icd

#endif // ICDCOMM_GLOBAL_H
