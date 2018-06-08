#ifndef HEADERUI_H
#define HEADERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcommondata.h"

class LimitTextEdit;
class LimitLineEdit;
class hexSpinBox;
class HeaderUI : public MetaUI
{
    Q_OBJECT
public:
    explicit HeaderUI(QWidget *parent = nullptr);

    int uiType() const override;

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
    hexSpinBox      *q_spinDefault;
    LimitTextEdit   *q_edtRemak;
};

#endif // HEADERUI_H
