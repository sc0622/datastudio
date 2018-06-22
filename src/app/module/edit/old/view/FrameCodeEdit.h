#ifndef FRAMECODE_EDIT_H
#define FRAMECODE_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdframecodedata.h"

class FrameCodeEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit FrameCodeEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // �л���������
    void changeDataType(int type) override;
    // ԭʼ��������
    int originalType() const override;

signals:

private slots:
    // �༭������¼�����
    void slotEditFinished();
    // �༭���ı����
    void slotTextChanged(const QString& text);

private:
    // ȷ��
    void confirm() override;
    // ȡ��
    void cancel() override;

    // ��ʼ����������
    void init();
    // ��ʼ��֡�볤��������
    void initBoxLenght();
    // ��ʼ��֡����������
    void initBoxData();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    ICDFrameCodeData::smtFrameCode  q_data; // ��������
    ICDFrameCodeData::smtFrameCode  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxLength;   // ֡�볤��
    QComboBox       *q_boxData;     // ֡����
    LimitTextEdit   *q_edtRemak;
};

#endif // FRAMECODE_EDIT_H
