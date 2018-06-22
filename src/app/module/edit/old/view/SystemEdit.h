#ifndef SYSTEM_EDIT_H
#define SYSTEM_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/globalstruct.h"

class QGridLayout;

class SystemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit SystemEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
public:
    void setUIData(const _UIData &data) override;
    void *uiData() const override;

signals:

private slots:
    // �༭������¼�����
    void slotEditFinished();
    // �༭���ı����
    void slotTextChanged(const QString &text);

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
    stSystem    q_data;     // ������Ϣ
    stSystem    q_old;      // ԭʼ����
    QColor      q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;

    QGridLayout     *q_gridLayout;
};

#endif // SYSTEM_EDIT_H
