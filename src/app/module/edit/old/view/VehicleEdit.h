#ifndef VEHICLE_EDIT_H
#define VEHICLE_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/globalstruct.h"

class VehicleEdit : public ObjectEdit
{
    Q_OBJECT
public:
    VehicleEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
public:
    void setUIData(const _UIData &data) override;
    void *uiData() const override;

signals:

private slots:
    // �༭���ı����
    void slotTextChanged(const QString &text);
    // ����¼�����
    void slotEditFinished();

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
    stPlane     q_data;     // ������Ϣ
    stPlane     q_old;      // ԭʼ����
    QColor      q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;
    
};

#endif // VEHICLE_EDIT_H
