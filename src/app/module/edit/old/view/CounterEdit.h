#ifndef COUNTER_EDIT_H
#define COUNTER_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcounterdata.h"

class CounterEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit CounterEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
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
    ICDCounterData::smtCounter  q_data; // ��������
    ICDCounterData::smtCounter  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxCounterType;  // У������
    LimitTextEdit   *q_edtRemak;
};

#endif // CounterEdit_H
