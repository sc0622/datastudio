#ifndef COUNTERUI_H
#define COUNTERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcounterdata.h"

class LimitTextEdit;
class LimitLineEdit;
class CounterUI : public MetaUI
{
    Q_OBJECT
public:
    explicit CounterUI(QWidget *parent = nullptr);

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
    ICDCounterData::smtCounter  q_data; // ��������
    ICDCounterData::smtCounter  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxCounterType;  // У������
    LimitTextEdit   *q_edtRemak;
};

#endif // COUNTERUI_H
