#ifndef ARRAYUI_H
#define ARRAYUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdarraydata.h"

class LimitTextEdit;
class LimitLineEdit;

class ArrayUI : public MetaUI
{
    Q_OBJECT
public:
    explicit ArrayUI(QWidget *parent = nullptr);

    int uiType() const override;

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
    ICDArrayData::smtArray  q_data;     // ��������
    ICDArrayData::smtArray  q_old;      // ԭʼ����

    QColor          q_color;            // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxArrayType;    // ��������
    QSpinBox        *q_spinCount;       // ���鳤��
    LimitTextEdit   *q_edtRemak;
};

#endif // ARRAYUI_H
