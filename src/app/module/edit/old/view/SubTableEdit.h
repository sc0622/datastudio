#ifndef SUBTABLE_EDIT_H
#define SUBTABLE_EDIT_H

#include "ItemEdit.h"

class JLargeSpinBox;

class SubTableEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit SubTableEdit(QWidget* parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;

signals:

private slots:
    // �༭���ı����
    void slotTextChanged(const QString& text);
    // ֡�����ݱ��
    void slotCodeChanged(const qulonglong &value);
    // ����¼�����
    void slotEditFinished();

private:
    // ȷ��
    void confirm() override;
    // ȡ��
    void cancel() override;

    // ��ʼ����������
    void init();
    // ��ʼ��֡�볤��������
    void initBoxLenght();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    stICDBase       q_data;     // ������Ϣ
    stICDBase       q_old;      // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QComboBox       *q_boxType;   // ֡�볤��
    JLargeSpinBox   *q_spinCode;
    QSpinBox        *q_spinSequence;
    LimitTextEdit   *q_edtDescribe;
};

#endif // SUBTABLE_EDIT_H
