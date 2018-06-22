#ifndef COMPLEX_EDIT_H
#define COMPLEX_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcomplexdata.h"

class ComplexEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ComplexEdit(QWidget *parent = nullptr);

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
    ICDComplexData::smtComplex  q_data; // ��������
    ICDComplexData::smtComplex  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;     // ����
    QLineEdit       *q_edtCode;     // ����
    QSpinBox        *q_spinLength;  // ����
    LimitTextEdit   *q_edtDescribe; // ����
};

#endif // COMPLEX_EDIT_H
