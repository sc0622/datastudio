#ifndef BUFFER_EDIT_H
#define BUFFER_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcustomizeddata.h"

class BufferEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit BufferEdit(QWidget *parent = nullptr);
    ~BufferEdit();

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // �л���������
    void changeDataType(int type) override;
    // ԭʼ��������
    int originalType() const override;

signals:

private slots :
    // ����¼�����
    void slotEditFinished();
    // �༭�ı����
    void slotTextChanged(const QString& text);

private:
    // ȷ��
    void confirm() override;
    // ȡ��
    void cancel() override;

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
};

#endif // BUFFER_EDIT_H
