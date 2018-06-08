#ifndef BUFFERUI_H
#define BUFFERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcustomizeddata.h"

class LimitTextEdit;
class LimitLineEdit;
// ����Ԥ��������
class BufferUI : public MetaUI
{
    Q_OBJECT

public:
    BufferUI(QWidget *parent = nullptr);
    ~BufferUI();

    int uiType() const override;

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
    ICDCustomizedData::smtCustom    q_data; // ��������
    ICDCustomizedData::smtCustom    q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;     // ����
    QLineEdit       *q_edtCode;     // ����
    QSpinBox        *q_spinLength;  // ����
    LimitTextEdit   *q_edtDescribe; // ����
};

#endif // BUFFERUI_H
