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
    // 获取在限定范围内的有效字符串
    QString fuzzyText(const QString &text) const;

protected slots:
    void _textChanged();

protected:

private:
    int     q_max;  // 允许输入的最大字符数
};

#endif // LIMITTEXTEDIT_H
