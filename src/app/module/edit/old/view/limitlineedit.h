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
    int maxLength() const {return q_max;};
    // 获取在限定范围内的有效字符串
    QString fuzzyText(const QString &text) const;

protected slots:
    void _textChanged(const QString &text);

protected:

private:
    int     q_max;  // 允许输入的最大字符数
};

#endif // LIMITLINEEDIT_H
