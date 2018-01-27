#include "limittextedit.h"

#ifndef CHSCALE
#   define     CHSCALE     2
#endif  // CHSCALE(���������ַ��ֽڳ���)

LimitTextEdit::LimitTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , q_max(-1)
{
    connect(this, SIGNAL(textChanged()), SLOT(_textChanged()));
}

LimitTextEdit::~LimitTextEdit()
{

}

void LimitTextEdit::_textChanged()
{
     if (maxLength() > 0) {
        QString text = toPlainText();
        QString newText = fuzzyText(text);
        if (text.size() > newText.size()) {
            QTextCursor cursor = textCursor();
            int position = cursor.position();
            int offset = text.size() - newText.size();
            setText(newText);
            cursor.setPosition(position - offset);
            setTextCursor(cursor);
        }
    }
}

void LimitTextEdit::setMaxLength( int maxLength )
{
    if (maxLength > 0) {
        q_max = maxLength;
    }
}

QString LimitTextEdit::fuzzyText(const QString &text) const
{
    // Qt5.6�����ַ�����3���ֽڱ�ʾ�������ݿ���2�ֽڣ����ԣ�����Ҫ����
    // �ض��жϣ��Ƿ񳬹����ַ�����
    QString result = text;
    if (text.toLocal8Bit().size() < maxLength()) {  // ��Ҫ�ж�
        return result;  // ��δ��������
    }
    // ���������ж�
    int realSize = text.toLocal8Bit().size();
    int cursorPos = textCursor().position();
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
