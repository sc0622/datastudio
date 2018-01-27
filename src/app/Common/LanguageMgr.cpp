#include "precomp.h"
#include "LanguageMgr.h"

// class LanguageMgrPrivate

class LanguageMgrPrivate
{
public:
    LanguageMgrPrivate(LanguageMgr *q)
        : q_ptr(q)
        , langType(LanguageMgr::LangInvalid)
        , trSystem(Q_NULLPTR)
        , trApp(Q_NULLPTR)
    {

    }

    void init();
    static bool parseLanguage(QLocale &locale, const QString &name);

private:
    J_DECLARE_PUBLIC(LanguageMgr)
    LanguageMgr::LangType langType;
    QTranslator *trSystem;
    QTranslator *trApp;
};

void LanguageMgrPrivate::init()
{
    Q_Q(LanguageMgr);
    trSystem = new QTranslator(q);
    trApp = new QTranslator(q);
}

bool LanguageMgrPrivate::parseLanguage(QLocale &locale, const QString &name)
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

    const QJsonDocument document = JJson::document(":/datastudio/lang/langdict.json", false);
    if (document.isNull()) {
        return false;
    }

    const QJsonObject rootJson = document.object();

    const QJsonArray languagesJson = rootJson["language"].toArray();
    if (languagesJson.isEmpty()) {
        return false;
    }

    int languageCode = -1;
    foreach (auto itemValue, languagesJson) {
        const QJsonObject itemObject = itemValue.toObject();
        const QString name = itemObject["name"].toString().toLower();
        if (name == language) {
            languageCode = itemObject["code"].toInt();
            break;
        }
    }

    const QJsonArray countrysJson = rootJson["country"].toArray();
    if (countrysJson.isEmpty()) {
        return false;
    }

    int countryCode = -1;
    foreach (auto itemValue, countrysJson) {
        const QJsonObject itemObject = itemValue.toObject();
        const QString name = itemObject["name"].toString().toLower();
        if (name == country) {
            countryCode = itemObject["code"].toInt();
            break;
        }
    }

    if (languageCode < 0 || countryCode < 0) {
        return false;
    }

    locale = QLocale((QLocale::Language)languageCode, (QLocale::Country)countryCode);

    return true;
}

// class LanguageMgr

J_IMPLEMENT_SINGLE_INSTANCE(LanguageMgr, JMain)

LanguageMgr::LanguageMgr(QObject *parent)
    : QObject(parent)
    , d_ptr(new LanguageMgrPrivate(this))
{
    Q_D(LanguageMgr);
    d->init();
}

LanguageMgr::~LanguageMgr()
{
    Q_D(LanguageMgr);
    delete d;
}

LanguageMgr::LangType LanguageMgr::langType() const
{
    Q_D(const LanguageMgr);
    return d->langType;
}

QStringList LanguageMgr::languages() const
{
    QStringList languages;
    languages.append(QStringLiteral("Chinese (China)"));
    languages.append(QStringLiteral("English (UnitedStates)"));
    return languages;
}

QLocale LanguageMgr::locale() const
{
    QLocale locale;
    if (!LanguageMgrPrivate::parseLanguage(locale, language())) {
        return QLocale();
    }
    return locale;
}

QString LanguageMgr::localeName() const
{
    return locale().name();
}

QString LanguageMgr::language() const
{
    return JJson::value(JMain::instance()->configFile(), "global/language").toString();
}

QString LanguageMgr::localeLanguage(const QString &language) const
{
    QLocale locale;
    if (!LanguageMgrPrivate::parseLanguage(locale, language)) {
        return QString();
    }

    return locale.nativeLanguageName() + " (" + locale.nativeCountryName() + ')';
}

bool LanguageMgr::installTranslator()
{
    const QString localeName = this->localeName();
    if (localeName.isEmpty()) {
        return false;
    }

    return installTranslators(localeName);
}

bool LanguageMgr::installTranslators(const QString &language)
{
    //
    removeTranslators();

    if (language.isEmpty()) {
        return false;
    }

    Q_D(LanguageMgr);

    //
    if (language == "zh_CN") {
        setLangType(LangChineseSimplify);
    } else if (language == "en_US") {
        setLangType(LangEnglishUS);
    } else {
        return false;
    }

    QLocale locale(language);

    bool result = true;

    // translator - qt
    if (d->trSystem->load(locale, ":/datastudio/lang/qt_")) {
        qApp->installTranslator(d->trSystem);
    } else {
        //qWarning() << "qt translator load failure.";
        result = false;
    }

    // translator - application
    if (d->trApp->load(locale, ":/datastudio/lang/app_")) {
        qApp->installTranslator(d->trApp);
    } else {
        //qWarning() << "application translator load failure.";
        result = false;
    }

    return true;
}

void LanguageMgr::removeTranslators()
{
    Q_D(LanguageMgr);
    qApp->removeTranslator(d->trSystem);
    qApp->removeTranslator(d->trApp);
}

void LanguageMgr::setLangType(LanguageMgr::LangType value)
{
    Q_D(LanguageMgr);
    if (value != d->langType) {
        d->langType = value;
        emit langTypeChanged(value);
    }
}

void LanguageMgr::setLanguage(const QString &value)
{
    const QString language = this->language();
    if (language != value) {
        QLocale locale;
        if (!LanguageMgrPrivate::parseLanguage(locale, value)) {
            return;
        }
        JJson::merge(JMain::instance()->configFile(), "global/language", value, true, false);
        emit languageChanged(value);
    }
}