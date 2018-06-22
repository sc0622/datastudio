#ifndef ARRAY_EDIT_H
#define ARRAY_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdarraydata.h"

class QSpinBox;

class ArrayEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ArrayEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // ObjectEdit interface
    void setUIData(const _UIData &data);
    void*uiData() const;
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
    // ��ʼ��У������������
    void initBoxType();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    ICDArrayData::smtArray  q_data;     // ��������
    ICDArrayData::smtArray  q_old;      // ԭʼ����

    QColor          q_color;            // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxArrayType;    // ��������
    QSpinBox        *q_spinCount;       // ���鳤��
    LimitTextEdit   *q_edtRemak;
};

#endif // ARRAY_EDIT_H
