#include "precomp.h"
#include "limitlineedit.h"

#ifndef CHSCALE
#define CHSCALE     2
#endif  // CHSCALE(单个中文字符字节长度)

LimitLineEdit::LimitLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , q_max(-1)
{
    connect(this, SIGNAL(textChanged(const QString &)),
            SLOT(_textChanged(const QString &)));
}

LimitLineEdit::~LimitLineEdit()
{

}

void LimitLineEdit::_textChanged(const QString &text)
{
    if (maxLength() > 0) {
        QString newText = fuzzyText(text);
        if (text.size() > newText.size()) {
            int position = cursorPosition();
            int offset = text.size() - newText.size();
            setText(newText);
            setCursorPosition(position - offset);
        }
    }
}

void LimitLineEdit::setMaxLength( int maxLength )
{
    if (maxLength > 0) {
        q_max = maxLength;
    }
}

int LimitLineEdit::maxLength() const
{
    return q_max;
}

QString LimitLineEdit::fuzzyText(const QString &text) const
{
    // Qt5.6中文字符串用3个字节表示，但数据库是2字节，所以，这里要进行
    // 特定判断，是否超过了字符限制
    QString result = text;
    if (text.toLocal8Bit().size() < maxLength()) {  // 简要判断
        return result;  // 尚未超过限制
    }
    // 具体数据判定
    int realSize = text.toLocal8Bit().size();
    int cursorPos = cursorPosition();
    int index = cursorPos - 1;
    int removeSize = 0; // 需要删除的字符数
    while (realSize > maxLength()) {
        if (index < 0) {
            break;
        }
        QChar single = text.at(index--);
        if (single >= 0x4e00 && single <= 0x9fa5) { // 中文
            realSize -= CHSCALE;
        } else {
            realSize -= QString(single).trimmed().toLocal8Bit().size();
        }
        removeSize++;
    }
    result.remove(cursorPos - removeSize, removeSize);

    return result;
}
