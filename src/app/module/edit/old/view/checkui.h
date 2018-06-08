#ifndef CHECKUI_H
#define CHECKUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcheckdata.h"

class LimitTextEdit;
class LimitLineEdit;
class CheckUI : public MetaUI
{
    Q_OBJECT
public:
    explicit CheckUI(QWidget *parent = nullptr);
    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // �л���������
    void changeDataType(int type) override;
    // ԭʼ��������
    int originalType() const override;

signals:

protected slots:
    // �༭������¼�����
    void slotEditFinished();
    // �༭���ı����
    void slotTextChanged(const QString& text);

protected:
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

#endif // CHECKUI_H
