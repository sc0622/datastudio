#ifndef TABLE_EDIT_H
#define TABLE_EDIT_H

#include "ItemEdit.h"

class LimitTextEdit;
class LimitLineEdit;
class TableEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit TableEdit(QWidget* parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;

signals:

private slots:
    // �༭���ı����
    void slotTextChanged(const QString& text);
    // ����¼�����
    void slotEditFinished() ;
    //
    void slotCheckStateChanged(bool checked);

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
    stICDBase       q_data;     // ������Ϣ
    stICDBase       q_old;      // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QSpinBox        *q_spinLength;
    QCheckBox       *q_check;
    LimitTextEdit   *q_edtDescribe;
};

#endif // TABLE_EDIT_H
