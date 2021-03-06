#include "precomp.h"
#include "JLangModel.h"
#include "icdcore/3rdpart/jsoncpp/json_tool.h"

namespace icdmeta {

// class JLangModelPrivate

class JLangModelPrivate
{
public:
    JLangModelPrivate(JLangModel *q)
        : J_QPTR(q)
        , langType(JLangModel::LangInvalid)
        , trSystem(nullptr)
        , trApp(nullptr)
    {

    }

    void init();
    static bool parseLanguage(QLocale &locale, const QString &name);

private:
    J_DECLARE_PUBLIC(JLangModel)
    JLangModel::LangType langType;
    QString configFile;
    QTranslator *trSystem;
    QTranslator *trApp;
};

void JLangModelPrivate::init()
{
    Q_Q(JLangModel);

    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
    configFile = settings.value("CONFIG_FILE").toString();
    settings.endGroup();

    trSystem = new QTranslator(q);
    trApp = new QTranslator(q);
}

bool JLangModelPrivate::parseLanguage(QLocale &locale, const QString &name)
{
    if (name.isEmpty()) {
        return false;
    }

    const QString _name = name.trimmed().replace(QRegExp("[\\(\\)]"), " ").toLower();
    if (_name.isEmpty()) {
        return false;
    }

    const QString language = _name.section(' ', 0, 0, QString::SectionSkipEmpty).trimmed();
    const QString country = _name.section(' ', 1, 1, QString::SectionSkipEmpty).trimmed();
    if (language.isEmpty() || country.isEmpty()) {
        return false;
    }

    QFile file(":/icdmeta/lang/langdict.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    Json::Value rootJson;
    if (!Json::parse(file.readAll().constData(), rootJson)) {
        return false;
    }

    const Json::Value languageJson = rootJson["language"];
    if (languageJson.isNull()) {
        return false;
    }

    int languageCode = -1;
    for (Json::ArrayIndex i = 0; i < languageJson.size(); ++i) {
        const Json::Value itemJson = languageJson[i];
        const QString name = QString::fromStdString(itemJson["name"].asString()).toLower();
        if (name == language) {
            languageCode = itemJson["code"].asInt();
            break;
        }
    }

    const Json::Value countrysJson = rootJson["country"];
    if (countrysJson.isNull()) {
        return false;
    }

    int countryCode = -1;
    for (Json::ArrayIndex i = 0; i < countrysJson.size(); ++i) {
        const Json::Value itemJson = countrysJson[i];
        const QString name = QString::fromStdString(itemJson["name"].asString()).toLower();
        if (name == country) {
            countryCode = itemJson["code"].asInt();
            break;
        }
    }

    if (languageCode < 0 || countryCode < 0) {
        return false;
    }

    locale = QLocale((QLocale::Language)languageCode, (QLocale::Country)countryCode);

    return true;
}

// class JLangModel

J_QML_IMPLEMENT_SINGLE_INSTANCE(JLangModel, QQmlEngine::CppOwnership, IcdCore)

JLangModel::JLangModel(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JLangModelPrivate(this))
{
    Q_D(JLangModel);
    d->init();
}

JLangModel::~JLangModel()
{
    Q_D(JLangModel);
    delete d;
}

void JLangModel::registerQmlType()
{
    //
    IcdMetaRegisterSingletonType3(JLangModel);

    //
}

JLangModel::LangType JLangModel::langType() const
{
    Q_D(const JLangModel);
    return d->langType;
}

QStringList JLangModel::languages() const
{
    QStringList languages;
    languages.append(QStringLiteral("Chinese (China)"));
    languages.append(QStringLiteral("English (UnitedStates)"));
    return languages;
}

QLocale JLangModel::locale() const
{
    QLocale locale;
    if (!JLangModelPrivate::parseLanguage(locale, language())) {
        return QLocale();
    }
    return locale;
}

QString JLangModel::localeName() const
{
    return locale().name();
}

QString JLangModel::language() const
{
    Q_D(const JLangModel);
    return QString::fromStdString(Json::resolve(d->configFile.toLocal8Bit(),
                                                "global.language").asString());
}

QString JLangModel::localeLanguage(const QString &language) const
{
    QLocale locale;
    if (!JLangModelPrivate::parseLanguage(locale, language)) {
        return QString();
    }

    return locale.nativeLanguageName() + " (" + locale.nativeCountryName() + ')';
}

void JLangModel::installTranslator()
{
    const QString localeName = this->localeName();
    if (localeName.isEmpty()) {
        return;
    }

    installTranslators(localeName);
}

void JLangModel::installTranslators(const QString &language)
{
    //
    removeTranslators();

    if (language.isEmpty()) {
        return;
    }

    Q_D(JLangModel);

    //
    if (language == "zh_CN") {
        setLangType(LangChineseSimplify);
    } else if (language == "en_US") {
        setLangType(LangEnglishUS);
    } else {
        return;
    }

    QLocale locale(language);

    // icd-meta
    IcdCore::instance()->initTranslators(language);

    // translator - qt
    if (d->trSystem->load(locale, ":/pma/lang/qt_")) {
        qApp->installTranslator(d->trSystem);
    } else {
        //qWarning() << "qt translator load failure.";
    }

    // translator - application
    if (d->trApp->load(locale, ":/pma/lang/app_")) {
        qApp->installTranslator(d->trApp);
    } else {
        //qWarning() << "application translator load failure.";
    }
}

void JLangModel::removeTranslators()
{
    Q_D(JLangModel);
    qApp->removeTranslator(d->trSystem);
    qApp->removeTranslator(d->trApp);
}

void JLangModel::setLangType(JLangModel::LangType value)
{
    Q_D(JLangModel);
    if (value != d->langType) {
        d->langType = value;
        emit langTypeChanged(value);
    }
}

void JLangModel::setLanguage(const QString &value)
{
    const QString language = this->language();
    if (language != value) {
        QLocale locale;
        if (!JLangModelPrivate::parseLanguage(locale, value)) {
            return;
        }
        Q_D(JLangModel);
        Json::make(d->configFile.toLocal8Bit(), "global.language", value.toStdString(),
                   true, false);
        emit languageChanged(value);
    }
}

}
