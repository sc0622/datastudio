#ifndef CHECK_EDIT_H
#define CHECK_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcheckdata.h"

class CheckEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit CheckEdit(QWidget *parent = nullptr);

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
    // ��ʼ��У������������
    void initBoxType();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    ICDCheckData::smtCheck      q_data; // ��������
    ICDCheckData::smtCheck      q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxCheckType;    // У������
    QSpinBox        *q_spinStart;       // У����ʼλ
    QSpinBox        *q_spinEnd;         // У����ֹλ
    LimitTextEdit   *q_edtRemak;
};

#endif // CHECK_EDIT_H
