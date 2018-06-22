#ifndef LIMITLINEEDIT_H
#define LIMITLINEEDIT_H

#include <QLineEdit>

class LimitLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LimitLineEdit(QWidget *parent);
    ~LimitLineEdit();

    void setMaxLength(int maxLength);
    int maxLength() const;

    // ��ȡ���޶���Χ�ڵ���Ч�ַ���
    QString fuzzyText(const QString &text) const;

protected slots:
    void _textChanged(const QString &text);

protected:

private:
    int     q_max;  // �������������ַ���
};

#endif // LIMITLINEEDIT_H
