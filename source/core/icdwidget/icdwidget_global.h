#ifndef ICDWIDGET_GLOBAL_H
#define ICDWIDGET_GLOBAL_H

#include <qglobal.h>

#ifdef ICDWIDGET_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDWIDGET_BUILD
#       define ICDWIDGET_EXPORT __declspec(dllexport)
#   else
#       define ICDWIDGET_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "icdwidgetd.lib")
#       else
#           pragma comment(lib, "icdwidget.lib")
#       endif
#   endif // !ICDWIDGET_BUILD
#endif // _MSC_VER || ...
#endif // ICDWIDGET_LIB

#ifndef ICDWIDGET_EXPORT
#define ICDWIDGET_EXPORT
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

//
#include <QStandardItem>

/**
 * @brief The TreeItemType enum
 */
enum TreeItemType {
    TreeItemTypeUserRole = QStandardItem::UserType + 1,
    TreeItemTypeRoot,
    TreeItemTypeVehicle,
    TreeItemTypeSystem,
    TreeItemTypeTable,
    TreeItemTypeDataItem,
    TreeItemTypeItemTable,
    TreeItemTypeItemBitMap
};

/**
 * @brief The TreeItemDataRole enum
 */
enum TreeItemDataRole {
    TreeItemUserRole = Qt::UserRole + 1, /**< ICD用户项标识 */
    TreeItemDomainRole,                  /**< ICD数据项域名标识 */
    TreeItemPathRole,                    /**<  */
    TreeChannelIdRole,                   /**< ICD通道标识（绑定到数据表） */
    TreeDataTypeRole,                    /**< ICD数据项类型 */
    TreeBoundRole,                       /**< ICD绑定其他对象 */
    TreeBitOffsetRole,                   /**< */
    TreeFilePathRole,                    /**< */
    TreeHasTimeFormatRole,               /**< */
    TreeHeaderSizeRole,                  /**< */
};

} // end of namespace Icd

#endif // ICDWIDGET_GLOBAL_H
