#ifndef LIMITTEXTEDIT_H
#define LIMITTEXTEDIT_H

#include <QTextEdit>

class LimitTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit LimitTextEdit(QWidget *parent = nullptr);

    void setMaxLength(int maxLength);
    int maxLength() const;

    // ��ȡ���޶���Χ�ڵ���Ч�ַ���
    QString fuzzyText(const QString &text) const;

protected slots:
    void _textChanged();

private:
    int  q_max;  // �������������ַ���
};

#endif // LIMITTEXTEDIT_H
