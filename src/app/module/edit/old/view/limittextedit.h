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

    // 获取在限定范围内的有效字符串
    QString fuzzyText(const QString &text) const;

protected slots:
    void _textChanged();

private:
    int  q_max;  // 允许输入的最大字符数
};

#endif // LIMITTEXTEDIT_H
