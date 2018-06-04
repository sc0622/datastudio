#ifndef JLANGMODEL_H
#define JLANGMODEL_H

#include "../icdmeta_global.h"

namespace icdmeta {

class JLangModelPrivate;

class ICDMETA_EXPORT JLangModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LangType langType READ langType WRITE setLangType NOTIFY langTypeChanged)
    Q_PROPERTY(QStringList languages READ languages NOTIFY languagesChanged)
    Q_PROPERTY(QString localeName READ localeName NOTIFY localeNameChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
public:
    /**
     * @brief The LangType enum
     */
    enum LangType {
        LangInvalid = -1,
        LangChineseSimplify,
        LangEnglishUS
    };
    Q_ENUM(LangType)

    explicit JLangModel(QObject *parent = nullptr);
    ~JLangModel();

    static void registerQmlType();

    /**
     * @brief langType
     * @return
     */
    LangType langType() const;
    QStringList languages() const;
    QLocale locale() const;
    QString localeName() const;
    QString language() const;

    void installTranslator();
    void installTranslators(const QString &language);
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
    Q_DISABLE_COPY(JLangModel)
    J_DECLARE_PRIVATE(JLangModel)
    J_DECLARE_SINGLE_INSTANCE(JLangModel)
};

}

#endif // JLANGMODEL_H
