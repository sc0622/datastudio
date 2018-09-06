#ifndef ICDWIDGET_GLOBAL_H
#define ICDWIDGET_GLOBAL_H

#ifdef ICDCORE_LIB
#include "icdcore/icdcore_global.h"
#endif

#ifdef ICDWORKER_LIB
#include "icdworker/icdworker_global.h"
#endif

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
        if (QLatin1String(QT_STRINGIFY(Class)) != #GlobalClass) { \
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

#ifndef J_TYPEDEF_QT_SHAREDPTR
#define J_TYPEDEF_QT_SHAREDPTR(_class_) \
    class _class_; \
    typedef QSharedPointer<_class_> _class_ ## Ptr; \
    typedef QList<_class_ ## Ptr> _class_ ## PtrArray;
#endif

#ifndef J_VARIANT_FROM_VOID
#define J_VARIANT_FROM_VOID
#include <QVariant>
template<typename T> inline
T *jVariantFromVoid(const QVariant &value)
{ return reinterpret_cast<T *>(value.value<void *>()); }
#endif

////////////////////////////////

namespace Icd {

#include <QStandardItem>

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

enum TreeItemDataRole {
    TreeItemUserRole = Qt::UserRole + 1,
    TreeItemIdRole,
    TreeItemDomainRole,
    TreeItemPathRole,
    TreeItemMarkRole,
    TreeChannelIdRole,
    TreeDataTypeRole,
    TreeBoundRole,
    TreeBitOffsetRole,
    TreeFilePathRole,
    TreeHasTimeFormatRole,
    TreeHeaderSizeRole,
    TreeLoadStatusRole
};

template<typename T>
inline std::shared_ptr<T> handlescope_cast(const QVariant &variant)
{
    JHandleScope<T> *handleScope = jVariantFromVoid<JHandleScope<T> >(variant);
    if (handleScope) {
        return handleScope->ptr;
    }
    return nullptr;
}

template<typename T>
inline std::weak_ptr<T> weakscope_cast(const QVariant &variant)
{
    JWeakScope<T> *weakScope = jVariantFromVoid<JWeakScope<T> >(variant);
    if (weakScope) {
        return weakScope->ptr;
    }
    return nullptr;
}

} // end of namespace Icd

// class IcdWidget

class IcdWidgetPrivate;

class ICDWIDGET_EXPORT IcdWidget : public QObject
{
    Q_OBJECT
public:

    bool init();

    void registerSingletonRelease(SingletonReleaseCallback callback);

    // {{ for icdcore
    static QStringList protoItemTypes();
    static QMap<int, QString> protoItemMapTypes();
    static QString typeString(const Icd::ItemPtr &item);
    static QString numericTypeString(int numericType);
    static QString counterTypeString(int counterType);
    static QString checkTypeString(int checkType);
    static QString frameCodeTypeString(int frameCodeType);
    // }} for icdcore

    static QString prettyValue(double value, bool isFloat = false);

signals:

public slots:

private:
    explicit IcdWidget(QObject *parent = nullptr);
    ~IcdWidget();

private:
    Q_DISABLE_COPY(IcdWidget)
    J_DECLARE_PRIVATE(IcdWidget)
    J_DECLARE_SINGLE_INSTANCE(IcdWidget)
};

#endif // ICDWIDGET_GLOBAL_H
