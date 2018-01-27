#ifndef LIMITTEXTEDIT_H
#define LIMITTEXTEDIT_H

#include <QTextEdit>

class LimitTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit LimitTextEdit(QWidget *parent);
    ~LimitTextEdit();

    void setMaxLength(int maxLength);
    int maxLength() const {return q_max;};
    // ��ȡ���޶���Χ�ڵ���Ч�ַ���
    QString fuzzyText(const QString &text) const;

protected slots:
    void _textChanged();

protected:

private:
    int     q_max;  // �������������ַ���
};

#endif // LIMITTEXTEDIT_H
