#include "precomp.h"
#include "limitlineedit.h"

#ifndef CHSCALE
#   define     CHSCALE     2
#endif  // CHSCALE(���������ַ��ֽڳ���)

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

QString LimitLineEdit::fuzzyText(const QString &text) const
{
    // Qt5.6�����ַ�����3���ֽڱ�ʾ�������ݿ���2�ֽڣ����ԣ�����Ҫ����
    // �ض��жϣ��Ƿ񳬹����ַ�����
    QString result = text;
    if (text.toLocal8Bit().size() < maxLength()) {  // ��Ҫ�ж�
        return result;  // ��δ��������
    }
    // ���������ж�
    int realSize = text.toLocal8Bit().size();
    int cursorPos = cursorPosition();
    int index = cursorPos - 1;
    int removeSize = 0; // ��Ҫɾ�����ַ���
    while (realSize > maxLength()) {
        if (index < 0) {
            break;
        }
        QChar single = text.at(index--);
        if (single >= 0x4e00 && single <= 0x9fa5) { // ����
            realSize -= CHSCALE;
        } else {
            realSize -= QString(single).trimmed().toLocal8Bit().size();
        }
        removeSize++;
    }
    result.remove(cursorPos - removeSize, removeSize);

    return result;
}
