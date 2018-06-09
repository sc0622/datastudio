#ifndef ICDMETA_GLOBAL_H
#define ICDMETA_GLOBAL_H

#if defined(ICDCORE_LIB) || defined(BUILDING_APP)
#include "icdcore/icdcore_global.h"
#ifdef ICDCORE_JSON_LIB
#include "icdcore/3rdpart/jsoncpp/json_tool.h"
#endif
#endif

#ifdef ICDMETA_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef ICDMETA_BUILD
#       define ICDMETA_EXPORT __declspec(dllexport)
#   else
#       define ICDMETA_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "icdmetad.lib")
#       else
#           pragma comment(lib, "icdmeta.lib")
#       endif
#   endif // !ICDMETA_BUILD
#endif // _MSC_VER || ...
#endif // ICDMETA_LIB

#ifndef ICDMETA_EXPORT
#define ICDMETA_EXPORT
#endif

#include <qglobal.h>
#include <QEvent>
#include <QJSValue>

// - common -

// - private pointer
#ifndef J_DECLARE_PRIVATE
#define J_DECLARE_PRIVATE(Class) \
    Class##Private *d_ptr_; \
    inline Class##Private* d_func() { return d_ptr_; } \
    inline const Class##Private* d_func() const { return d_ptr_; } \
    friend class Class##Private;
#endif

#ifndef J_DECLARE_PUBLIC
#define J_DECLARE_PUBLIC(Class) \
    Class *q_ptr_; \
    inline Class* q_func() { return q_ptr_; } \
    inline const Class* q_func() const { return q_ptr_; } \
    friend class Class;
#endif

#ifndef J_DPTR
#define J_DPTR d_ptr_
#endif

#ifndef J_QPTR
#define J_QPTR q_ptr_
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

#ifndef J_QML_IMPLEMENT_SINGLE_INSTANCE
#define J_QML_IMPLEMENT_SINGLE_INSTANCE(Class, Ownership, GlobalClass) \
    J_IMPLEMENT_SINGLE_INSTANCE(Class, GlobalClass) \
    \
    QObject *__ ## Class ## _Singleton_Callback(QQmlEngine *qmlEngine, QJSEngine *jsEngine) \
{ \
    Q_UNUSED(qmlEngine); \
    Q_UNUSED(jsEngine); \
    QObject *object = Class::instance(); \
    QQmlEngine::setObjectOwnership(object, Ownership); \
    return object; \
    }
#endif

#ifndef J_SINGLE_RELEASE_CALLBACK
#define J_SINGLE_RELEASE_CALLBACK
typedef void(*SingletonReleaseCallback)();
#endif

// --

#include <vector>

#ifndef J_TYPEDEF_QT_SHAREDPTR
#define J_TYPEDEF_QT_SHAREDPTR(_class_) \
    class _class_; \
    typedef QSharedPointer<_class_> _class_ ## Ptr; \
    typedef QList<_class_ ## Ptr> _class_ ## PtrArray;
#endif

////////////////////////////////

// qml register

#ifdef QT_QML_LIB
#include <qqml.h>
#include <QJSValue>
#else
class QJSValue;
class QJSValueList;
#endif

// global

#define ICDMETA_VER_MAJOR 1
#define ICDMETA_VER_MINOR 0
#define ICDMETA_DOMAIN "Icd.Core"

/// for icdmeta

#ifndef IcdMetaRegisterUncreatableType
#define IcdMetaRegisterUncreatableType(T, reason) \
    qmlRegisterUncreatableType<T>(ICDMETA_DOMAIN, ICDMETA_VER_MAJOR, \
        ICDMETA_VER_MINOR, QT_STRINGIFY(T), reason)
#endif

#ifndef IcdMetaRegisterUncreatableType2
#define IcdMetaRegisterUncreatableType2(T) \
    IcdMetaRegisterUncreatableType(T, "created by " ICDMETA_DOMAIN)
#endif

#ifndef IcdMetaRegisterSingletonType
#define IcdMetaRegisterSingletonType(T, typeName, callback) \
    qmlRegisterSingletonType<T>(ICDMETA_DOMAIN, ICDMETA_VER_MAJOR, ICDMETA_VER_MINOR, \
        typeName, callback)
#endif

#ifndef IcdMetaRegisterSingletonType2
#define IcdMetaRegisterSingletonType2(T, typeName) \
    IcdMetaRegisterSingletonType(T, typeName, __ ## T ## _Singleton_Callback)
#endif

#ifndef IcdMetaRegisterSingletonType3
#define IcdMetaRegisterSingletonType3(T) \
    IcdMetaRegisterSingletonType2(T, QT_STRINGIFY(T))
#endif

namespace icdmeta {

/**
 * @brief The CustomEvent enum
 */
enum CustomEvent {
    Event_Callback = QEvent::User + 100,
    Event_StateMachine,
    Event_SetProperty,
    Event_ProtocolFeedback
};

// class JCallbackEvent

struct JCallbackEventData;

class JCallbackEvent : public QEvent
{
    Q_GADGET
public:
    explicit JCallbackEvent(QJSValue callback, const QJSValueList &arguments);
    ~JCallbackEvent();

    bool isValid() const;
    void execute();

private:
    JCallbackEventData *data;
};

} // end of namespace icdmeta

// class IcdCore

class IcdCorePrivate;

class ICDMETA_EXPORT IcdCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString configDir READ configDir WRITE setConfigDir NOTIFY configDirChanged)
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
public:
    /**
     * @brief registerQmlType
     */
    static void registerQmlType();

    /**
     * @brief init
     * @return
     */
    bool init();

    /**
     * @brief The ObjectType enum
     */
    enum ObjectType {
        ObjectInvalid = -1,
        ObjectRoot,
        ObjectVehicle,
        ObjectSystem,
        ObjectTable,
        ObjectItem
    };
    Q_ENUM(ObjectType)

    /**
     * @brief The ItemType enum
     */
    enum ItemType {
        ItemInvalid = -1,   /**<  */
        ItemHead,           /**<  */
        ItemCounter,        /**<  */
        ItemCheck,          /**<  */
        ItemFrameCode,      /**<  */
        ItemNumeric,        /**<  */
        ItemBitMap,         /**< BITMAP */
        ItemBitValue,       /**< BITVALUE */
        ItemComplex,        /**<  */
        ItemFrame,          /**<  */
        ItemString,         /**<  */
        ItemDateTime,       /**<  */
        ItemArray,          /**<  */
        ItemTotal           /**<  */
    };
    Q_ENUM(ItemType)

    /**
     * @brief The CheckType enum
     */
    enum CheckType {
        CheckNone,      /**<  */
        CheckSum8,      /**< 8 */
        CheckSum16,     /**< 16 */
        CheckCrc8,      /**< Crc8 */
        CheckCrc16,     /**< Crc16 */
        CheckXor8,      /**< Xor8 */
        CheckXor16,     /**< Xor16 */
        CheckTotal      /**<  */
    };
    Q_ENUM(CheckType)

    /**
     * @brief The CounterType enum
     */
    enum CounterType {
        CounterInvalid = -1,
        CounterU8,
        CounterU16,
        CounterU32,
        CounterU64,
        CounterTotal
    };
    Q_ENUM(CounterType)

    /**
     * @brief The FrameCodeType enum
     */
    enum FrameCodeType {
        FrameCodeInvalid = -1,
        FrameCodeU8,
        FrameCodeU16,
        FrameCodeU32,
        FrameCodeU64,
        FrameCodeTotal
    };
    Q_ENUM(FrameCodeType)

    /**
     * @brief The NumericType enum
     */
    enum NumericType {
        NumericInvalid = -1,
        NumericInt8,
        NumericUint8,
        NumericInt16,
        NumericUint16,
        NumericInt32,
        NumericUint32,
        NumericInt64,
        NumericUint64,
        NumericFloat64,
        NumericFloat32,
        NumericTotal
    };
    Q_ENUM(NumericType)

    /**
     * @brief The DomainType enum
     */
    enum DomainType {
        DomainValid = -1,
        DomainId,
        DomainName,
        DomainMark
    };
    Q_ENUM(DomainType)

    Q_INVOKABLE QString stringSection(const QString &source,
                                      QChar sep, int start, int end = -1,
                                      QString::SectionFlags flags = QString::SectionDefault) const;
    Q_INVOKABLE QString stringSection(const QString &source,
                                      const QString &in_sep, int start, int end = -1,
                                      QString::SectionFlags flags = QString::SectionDefault) const;
    Q_INVOKABLE QString stringSection(const QString &source,
                                      const QRegExp &reg, int start, int end = -1,
                                      QString::SectionFlags flags = QString::SectionDefault) const;
    Q_INVOKABLE bool matchString(const QString &pattern, const QString &source) const;
    Q_INVOKABLE bool matchString(const QRegExp &regexp, const QString &source) const;
    Q_INVOKABLE QStringList splitString(const QString &sep, const QString &source) const;
    Q_INVOKABLE QStringList splitString(const QRegExp &regexp, const QString &source) const;
    Q_INVOKABLE QString stringArg(const QVariant &value, int fieldwidth = 0, int base = 10,
                                  QChar fillChar = QLatin1Char(' ')) const;

    Q_INVOKABLE quint8 sumOfString(const QString &str);
    Q_INVOKABLE QString generateGuid() const;
    Q_INVOKABLE bool jsValueIsString(const QJSValue &value) const;
    Q_INVOKABLE void destroyObject(QObject *object);
    Q_INVOKABLE qreal currentSecsSinceEpoch() const;
    Q_INVOKABLE void startAwait();
    Q_INVOKABLE void stopAwait();
    Q_INVOKABLE void asyncCall(QJSValue callback, const QJSValue &args = QJSValue());
    Q_INVOKABLE void delayCall(QJSValue callback, const QJSValue &args = QJSValue(), qint64 msecs = 10000);
    Q_INVOKABLE void cancelDelayCall();

    void registerSingletonRelease(SingletonReleaseCallback callback);

    static void initQuickEnv(const QString &configPath = QString());
    static bool initFontDatabase(const QString &fontPath = QString());

    bool initTranslators(const QString &language);

    QString configDir() const;
    int fontSize() const;

    Q_INVOKABLE QString parsePath(const QString &path, bool canonical = false) const;
    Q_INVOKABLE QString restorePath(const QString &path) const;

signals:
    void configDirChanged(const QString &dir);
    void fontSizeChanged(int size);

public slots:
    void setConfigDir(const QString &dir);
    void setFontSize(int size);
    void reset();

protected:
    void customEvent(QEvent *event);

private:
    explicit IcdCore(QObject *parent = nullptr);
    ~IcdCore();

private:
    Q_DISABLE_COPY(IcdCore)
    J_DECLARE_PRIVATE(IcdCore)
    J_DECLARE_SINGLE_INSTANCE(IcdCore)
};

#endif // ICDMETA_GLOBAL_H
