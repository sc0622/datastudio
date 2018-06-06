#ifndef BUFFERUI_H
#define BUFFERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcustomizeddata.h"

class LimitTextEdit;
class LimitLineEdit;
// ����Ԥ��������
class BufferUI : public MetaUI
{
    Q_OBJECT

public:
    BufferUI(QWidget *parent = nullptr);
    ~BufferUI();

    // MetaUI interface
    void setUIData(const _UIData &data);
    void*uiData() const;
    // �л���������
    void changeDataType(int type);
    // ԭʼ��������
    int originalType() const;
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots :
    // ����¼�����
    void slotEditFinished();
    // �༭�ı����
    void slotTextChanged(const QString& text);
    // ȷ��
    void slotConfirm();
    // ȡ��
    void slotCanceled();

protected:
    // ��ʼ����������
    void init();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    ICDCustomizedData::smtCustom    q_data; // ��������
    ICDCustomizedData::smtCustom    q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;     // ����
    QLineEdit       *q_edtCode;     // ����
    QSpinBox        *q_spinLength;  // ����
    LimitTextEdit   *q_edtDescribe; // ����

    QLineEdit       *q_edtStatus;   // ״̬��ʾ
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
};

#endif // BUFFERUI_H
