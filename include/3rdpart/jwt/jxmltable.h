#ifndef JXMLTABLE_H
#define JXMLTABLE_H

#include "jwt_global.h"
#include "jtableview.h"

// - class JXmlTable -

class JXmlTablePrivate;
class QDomElement;

class JWT_EXPORT JXmlTable : public JTableView
{
    Q_OBJECT
public:
    explicit JXmlTable(QWidget *parent);
    JXmlTable(const QString &filePath, QWidget *parent = 0);
    virtual ~JXmlTable();

    bool loadConfig(const QString &filePath, const QString &tableName);
    bool loadConfig(const char *text, int size);
    bool loadConfig(const QByteArray &text);
    bool loadConfig(const QDomElement *emTable);

    QString version() const;

    QString config() const;
    void setConfig(const QString &filePath);

    QString tableName() const;
    bool setTableName(const QString &value);

    QVariant itemValue(int row, int column, int role = Qt::DisplayRole) const;
    bool setItemValue(int row, int column, const QVariant &value, int role = Qt::EditRole);

    QObject *itemObject(int row, int column) const;
    QVariant itemProperty(int row, int column, const char *name) const;
    void setItemProperty(int row, int column, const char *name, const QVariant &value);

    Qt::Orientations orientations() const;

    QByteArray pack(int row = -1, int column = -1) const;
    bool unpack(const QByteArray &privateData, int row = -1, int column = -1);

    QVariant unpackFromItem(const QByteArray &privateData, int row, int column) const;
    QByteArray packFromItem(const QVariant &value, int row, int column) const;

    bool setHighlight(const QColor &color, int row = -1, int column = -1);
    void removeHighlight(int row = -1, int column = -1);

    bool isReadOnly() const;

    QObject *configObject();

Q_SIGNALS:
    void tableChanged(const QString &);
    void contentChanged();
    void filterChanged(int column, int type, bool visible);
    void readOnlyChanged(bool);

public Q_SLOTS:
    void setReadOnly(bool value = true);

private:
    Q_DISABLE_COPY(JXmlTable)
    J_DECLARE_PRIVATE(JXmlTable)
    friend class JXmlItemDelegate;
};

#endif // JXMLTABLE_H
