#ifndef ICDMETA_GLOBAL_H
#define ICDMETA_GLOBAL_H

#ifdef ICDCORE_LIB
#include "icdcore/icdcore_global.h"
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

// - common -

// - private pointer
#ifdef J_DECLARE_PRIVATE
#undef J_DECLARE_PRIVATE
#endif
#define J_DECLARE_PRIVATE(name) \
    Q_DECLARE_PRIVATE(name) \
    name ## Private *d_ptr;

#ifdef J_DECLARE_PUBLIC
#undef J_DECLARE_PUBLIC
#endif
#define J_DECLARE_PUBLIC(name) \
    Q_DECLARE_PUBLIC(name) \
    name *q_ptr;

#ifdef J_DECLARE_SINGLE_INSTANCE
#undef J_DECLARE_SINGLE_INSTANCE
#endif
#define J_DECLARE_SINGLE_INSTANCE(Class) \
    public: \
    static Class *instance(); \
    static void releaseInstance(); \
    private: \
    static Class *_instance;

#ifdef J_IMPLEMENT_SINGLE_INSTANCE
#undef J_IMPLEMENT_SINGLE_INSTANCE
#endif
#define J_IMPLEMENT_SINGLE_INSTANCE(Class, GlobalClass) \
    \
    static void __ ## Class ## _releaseInstance() { \
        Class::releaseInstance(); \
    } \
    Class *Class::_instance = 0; \
    \
    Class *Class::instance() { \
        if (Class::_instance == 0) { \
            Class::_instance = new Class; \
        } \
        if (QLatin1String(QT_STRINGIFY(Class)) != #GlobalClass) { \
            GlobalClass::instance()->registerSingletonRelease(__ ## Class ## _releaseInstance); \
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

#ifdef J_QML_IMPLEMENT_SINGLE_INSTANCE
#undef J_QML_IMPLEMENT_SINGLE_INSTANCE
#endif
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

#ifndef J_SINGLE_RELEASE_CALLBACK
#define J_SINGLE_RELEASE_CALLBACK
typedef void(*SingletonReleaseCallback)();
#endif

// --

#include <vector>

#ifndef J_TYPEDEF_QT_SHAREDPTR
#undef J_TYPEDEF_QT_SHAREDPTR
#endif
#define J_TYPEDEF_QT_SHAREDPTR(_class_) \
    class _class_; \
    typedef QSharedPointer<_class_> _class_ ## Ptr; \
    typedef QList<_class_ ## Ptr> _class_ ## PtrArray;

////////////////////////////////

// qml register

#ifdef QT_QML_LIB
#include <qqml.h>
#include <QJSValue>
#else
class QJSValue;
class QJSValueList;
#endif

#define ICDMETA_QML_VER_MAJOR 1
#define ICDMETA_QML_VER_MINOR 0
#define ICDMETA_DOMAIN "Icd.Core"

#ifdef jRegisterSingletonType2
#undef jRegisterSingletonType2
#endif
#define jRegisterSingletonType2(T, callback) \
    jRegisterSingletonType(T, QT_STRINGIFY(T), callback)

#ifdef jRegisterSingletonTypeCpp
#undef jRegisterSingletonTypeCpp
#endif
#define jRegisterSingletonTypeCpp(T, qmlName) \
    jRegisterSingletonType(T, qmlName, __ ## T ## _Singleton_Callback)

#ifdef jRegisterSingletonTypeCpp2
#undef jRegisterSingletonTypeCpp2
#endif
#define jRegisterSingletonTypeCpp2(T) \
    jRegisterSingletonType2(T, __ ## T ## _Singleton_Callback)

#ifdef jRegisterSingletonType
#undef jRegisterSingletonType
#endif
#define jRegisterSingletonType(T, qmlName, callback) \
    qmlRegisterSingletonType<T>(ICDMETA_DOMAIN, \
    ICDMETA_QML_VER_MAJOR, ICDMETA_QML_VER_MINOR, qmlName, callback)

#ifdef jRegisterUncreatableType
#undef jRegisterUncreatableType
#endif
#define jRegisterUncreatableType(T) \
    qmlRegisterUncreatableType<T>(ICDMETA_DOMAIN, ICDMETA_QML_VER_MAJOR, \
    ICDMETA_QML_VER_MINOR, QT_STRINGIFY(T), "created by "ICDMETA_DOMAIN)

#ifdef jRegisterType
#undef jRegisterType
#endif
#define jRegisterType(T) \
    qmlRegisterType<T>(ICDMETA_DOMAIN, ICDMETA_QML_VER_MAJOR, \
    ICDMETA_QML_VER_MINOR, QT_STRINGIFY(T))

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
        ItemInvalid = -1,   /**< ��Ч */
        ItemHead,           /**< ��ͷ */
        ItemCounter,        /**< ֡���� */
        ItemCheck,          /**< У�� */
        ItemFrameCode,      /**< ֡ʶ���� */
        ItemNumeric,        /**< ��ֵ���� */
        ItemBitMap,         /**< BITMAP */
        ItemBitValue,       /**< BITVALUE */
        ItemComplex,        /**< �������� */
        ItemFrame,          /**< ��֡���� */
        ItemTotal           /**< �������� */
    };
    Q_ENUM(ItemType)

    /**
     * @brief The CheckType enum
     */
    enum CheckType {
        CheckNone,      /**< ��У�� */
        CheckSum8,      /**< 8λ��У�� */
        CheckSum16,     /**< 16λ��У�� */
        CheckCrc8,      /**< Crc8У�� */
        CheckCrc16,     /**< Crc16У�� */
        CheckXor8,      /**< Xor8У�� */
        CheckXor16,     /**< Xor16У�� */
        CheckTotal      /**< �������� */
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

    static void initQuickEnv();
    static bool initFontDatabase();

    bool initTranslators(const QString &language);

    QString configDir() const;

    Q_INVOKABLE QString parsePath(const QString &path, bool canonical = false) const;
    Q_INVOKABLE QString restorePath(const QString &path) const;

signals:
    void configDirChanged(const QString &dir);

public slots:
    void setConfigDir(const QString &dir);
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