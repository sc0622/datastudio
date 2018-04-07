#ifndef LANGUAGEMGR_H
#define LANGUAGEMGR_H

#include "main_global.h"

class LanguageMgrPrivate;

class LanguageMgr : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LangType langType READ langType WRITE setLangType NOTIFY langTypeChanged)
    Q_PROPERTY(QStringList languages READ languages NOTIFY languagesChanged)
    Q_PROPERTY(QString localeName READ localeName NOTIFY localeNameChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
public:
    enum LangType {
        LangInvalid = -1,
        LangChineseSimplify,
        LangEnglishUS
    };
    Q_ENUM(LangType)

    explicit LanguageMgr(QObject *parent = 0);
    ~LanguageMgr();

    LangType langType() const;
    QStringList languages() const;
    QLocale locale() const;
    QString localeName() const;
    QString language() const;

    bool installTranslator();
    bool installTranslators(const QString &language);
    void removeTranslators();

    Q_INVOKABLE QString localeLanguage(const QString &language) const;

signals:
    void langTypeChanged(LangType value);
    void languagesChanged();
    void localeNameChanged(const QString &value);
    void languageChanged(const QString &value);

public slots:
    void setLangType(LangType value);
    void setLanguage(const QString &value);

private:
    Q_DISABLE_COPY(LanguageMgr)
    J_DECLARE_PRIVATE(LanguageMgr)
    J_DECLARE_SINGLE_INSTANCE(LanguageMgr)
};

#endif // LANGUAGEMGR_H
