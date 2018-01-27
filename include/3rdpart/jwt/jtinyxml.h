#ifndef JTINYXML_H
#define JTINYXML_H

#include "jwt_global.h"

// class JXmlDocument

class TiXmlDocument;
class JXmlDocumentData;

class JWT_EXPORT JXmlDocument
{
public:
    JXmlDocument();
    explicit JXmlDocument(TiXmlDocument *document);
    ~JXmlDocument();

    JXmlDocument(JXmlDocument &other);
    JXmlDocument &operator =(const JXmlDocument &other);

    TiXmlDocument *data() const;
    bool isNull() const { return !data(); }
    bool operator !() const { return isNull(); }
    TiXmlDocument &operator *() const { return *data(); }
    TiXmlDocument *operator ->() const { return data(); }

private:
    JXmlDocumentData *d;
};

// class JXmlElement

class TiXmlElement;
class JXmlElementData;

class JWT_EXPORT JXmlElement
{
public:
    JXmlElement();
    explicit JXmlElement(TiXmlElement *element);
    ~JXmlElement();

    JXmlElement(JXmlElement &other);
    JXmlElement &operator =(const JXmlElement &other);

    TiXmlElement *data() const;
    bool isNull() const { return !data(); }
    bool operator !() const { return isNull(); }
    TiXmlElement &operator *() const { return *data(); }
    TiXmlElement *operator ->() const { return data(); }

private:
    JXmlElementData *d;
};

// class JXml;

class QVariant;

class JWT_EXPORT JXml
{
public:
    static JXmlElement rootElement(const QString &filePath, bool create = false);
    static JXmlElement elementByPath(const QString &filePath,
                                     const QString &path, bool create = false);

    static QVariant elementAttribute(const QString &filePath, const QString &path,
                                     const QString &attrName);
    static bool setElementAttribute(const QString &filePath, const QString &path,
                                    const QString &attrName, const QVariant &value, bool create = false);

    static JXmlElement elementByDomain(const QString &filePath, const QString &domain, const QString &domainAttrName);

    static QVariant elementAttribute(const QString &filePath, const QString &domain, const QString &domainAttrName,
                                     const QString &attrName);
    static bool setElementAttribute(const QString &filePath, const QString &domain, const QString &domainAttrName,
                                    const QString &attrName, const QVariant &value);

private:
    static JXmlElement elementByPath(JXmlElement &emParent, const QString &path, bool create);
    static JXmlElement elementByDomain(JXmlElement &emParent, const QString &domain, const QString &domainAttrName);
    static JXmlElement elementByAttr(const JXmlElement &element, const QString &attrName, const QVariant &value);
};

#endif // JTINYXML_H
