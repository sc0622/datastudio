#ifndef TABLEUI_H
#define TABLEUI_H

#include <QtWidgets>

#include "metaui.h"

class LimitTextEdit;
class LimitLineEdit;
class TableUI : public MetaUI
{
    Q_OBJECT
public:
    explicit TableUI(QWidget* parent = nullptr);

    int uiType() const override;

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

#endif // TABLEUI_H
