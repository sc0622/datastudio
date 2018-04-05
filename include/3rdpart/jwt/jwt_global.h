#ifndef JWT_GLOBAL_H
#define JWT_GLOBAL_H

#include <qglobal.h>
#include <QString>

// JWT_VERSION is (major << 16) + (minor << 8) + patch.

#define JWT_VERSION       0x000001
#define JWT_VERSION_STR   "0.0.1"

#ifdef JWT_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef JWT_BUILD
#       define JWT_EXPORT  Q_DECL_EXPORT
#   else
#       define JWT_EXPORT  Q_DECL_IMPORT
#       if defined(_DEBUG) || defined(DEBUG)
#           pragma comment(lib, "jwtd.lib")
#       else
#           pragma comment(lib, "jwt.lib")
#       endif
#   endif // !JWT_BUILD
#endif // _MSC_VER || ...
#endif // JWT_LIB

#ifndef JWT_EXPORT
#define JWT_EXPORT
#endif

// - namespace JWT -
#define JWT_NAMESPACE JWT
#define JWT_BEGIN_NAMESPACE namespace JWT_NAMESPACE {
#define JWT_END_NAMESPACE }
#define JWT_USE_NAMESPACE using namespace JWT_NAMESPACE;

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
#endif

#ifndef J_SINGLE_RELEASE_CALLBACK
#define J_SINGLE_RELEASE_CALLBACK
typedef void(*SingletonReleaseCallback)();
#endif

namespace JWT_NAMESPACE {
static const double jDoubleEpsion = 1E-6;
static const double jFDoubleEpsion = 1E-6f;

// ==
static inline bool jEqual(double a, double b)
{ return (a - b > -jDoubleEpsion || a - b < jDoubleEpsion) ? true : false; }
static inline bool fEqual(float a, float b)
{ return (a - b > -jFDoubleEpsion || a - b < jFDoubleEpsion) ? true : false; }

// >
static inline bool fGreater(double a, double b)
{ return (a - b > jDoubleEpsion) ? true : false; }
static inline bool fGreater(float a, float b)
{ return (a - b > jFDoubleEpsion) ? true : false; }

// <
static inline bool fLess(double a, double b)
{ return (a - b < -jDoubleEpsion) ? true : false; }
static inline bool fLess(float a, float b)
{ return (a - b < -jFDoubleEpsion) ? true : false; }

// >=
static inline bool fGreaterOrEqual(double a, double b)
{ return ((a - b > jDoubleEpsion) ||  (a - b >= -jDoubleEpsion && a - b <= jDoubleEpsion)) ? true : false; }
static inline bool fGreaterOrEqual(float a, float b)
{ return ((a - b > jFDoubleEpsion) ||  (a - b >= -jFDoubleEpsion && a - b <= jFDoubleEpsion)) ? true : false; }

// <=
static inline bool fLessOrEqual(double a, double b)
{ return ((a - b < -jDoubleEpsion) || (a - b >= -jDoubleEpsion && a - b <= jDoubleEpsion)) ? true : false; }
static inline bool fLessOrEqual(float a, float b)
{ return ((a - b < -jFDoubleEpsion) || (a - b >= -jFDoubleEpsion && a - b <= jFDoubleEpsion)) ? true : false; }
}

// - class JwtCore -

#include <QObject>

class JwtCorePrivate;

class JWT_EXPORT JwtCore : public QObject
{
public:
    void init();

    bool loadSystemLang(const QString &systemName = QString());

    void registerSingletonRelease(SingletonReleaseCallback callback);

private:
    explicit JwtCore(QObject *parent = nullptr);
    ~JwtCore();

private:
    Q_DISABLE_COPY(JwtCore)
    J_DECLARE_PRIVATE(JwtCore)
    J_DECLARE_SINGLE_INSTANCE(JwtCore)
};

// - class JAutoCursor -

class JWT_EXPORT JAutoCursor
{
public:
    explicit JAutoCursor(Qt::CursorShape shape);
    ~JAutoCursor();

    void restore();
};

// class JJson;

class QJsonDocument;
class QJsonObject;
class QJsonArray;
class QJsonValue;

class JWT_EXPORT JJson
{
public:
    static QJsonDocument document(const QString &filePath, bool create = false);
    static QJsonObject object(const QJsonDocument &doc, const QString &domain, bool create = false);
    static QJsonObject object(const QString &filePath, const QString &domain, bool create = false);
    static QJsonArray array(const QJsonDocument &doc, const QString &domain, bool create = false);
    static QJsonArray array(const QString &filePath, const QString &domain, bool create = false);

    static QJsonObject read(const QString &filePath, const QString &domain, bool create = false);
    static QJsonValue value(const QJsonDocument &doc, const QString &domain, bool create = false);
    static QJsonValue value(const QString &filePath, const QString &domain, bool create = false);
    static bool setValue(const QString &filePath, const QString &domain,
                         const QJsonValue &value, bool create = false, bool fast = false);
    static bool merge(const QString &filePath, const QString &domain,
                      const QJsonValue &json, bool create = false, bool fast = false);
    static bool setArrayValue(const QString &filePath, const QString &domain,
                              const QString &keyName, const QVariant &keyValue, const QJsonValue &value,
                              bool create = false, bool fast = false);

    static bool save(const QString &filePath, const QJsonDocument &document, bool fast = false);

private:
    static bool setValue(QJsonObject &parentJson, const QString &domain, const QJsonValue &value);
    static bool merge(QJsonObject &parentObject, const QString &domain,
                      const QJsonValue &value);
    static bool setArrayValue(QJsonObject &parentJson, const QString &domain, const QString &keyName,
                              const QVariant &keyValue, const QJsonValue &value);
    static bool merge(const QJsonObject &source, QJsonObject &target);
};

#endif // JWT_GLOBAL_H
