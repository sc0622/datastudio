#ifndef JCHECKCOMBOBOX_H
#define JCHECKCOMBOBOX_H

#include "jwt_global.h"
#include <QComboBox>

// - class JCheckComboBox -

class JCheckComboBoxPrivate;

class JWT_EXPORT JCheckComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit JCheckComboBox(QWidget *parent = 0);
    ~JCheckComboBox();

    void addItem(const QString &text);
    void addItem(const QString &text, const qulonglong &encode);
    void removeItem(int index);
    void removeItem(const qulonglong &encode);
    void removeItem(const QString &text);
    void clearItem();

    qulonglong encode(int index = -1) const;
    QString text(int index = -1) const;
    QStringList checkedItems() const;

Q_SIGNALS:
    void encodeChanged(const qulonglong &);
    void encodeChanged(const QString &);

public Q_SLOTS:
    void setEncode(const qulonglong &value);
    void setEncode(const QString &value);

protected:
    bool eventFilter(QObject *o, QEvent *e);

private Q_SLOTS:
    void onStateChanged(int state);

private:
    Q_DISABLE_COPY(JCheckComboBox)
    J_DECLARE_PRIVATE(JCheckComboBox)
};

#endif // JCHECKCOMBOBOX_H
