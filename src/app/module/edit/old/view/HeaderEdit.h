#ifndef HEADER_EDIT_H
#define HEADER_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcommondata.h"

class HexSpinBox;

class HeaderEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit HeaderEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void *uiData() const override;
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
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    ICDCommonData::smtCommon    q_data; // ��������
    ICDCommonData::smtCommon    q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    HexSpinBox      *q_spinDefault;
    LimitTextEdit   *q_edtRemak;
};

#endif // HEADER_EDIT_H
