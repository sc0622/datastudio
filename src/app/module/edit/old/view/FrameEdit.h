#ifndef FRAME_EDIT_H
#define FRAME_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcomplexdata.h"

class FrameEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit FrameEdit(QWidget *parent = nullptr);

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
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    ICDComplexData::smtComplex  q_data; // ��������
    ICDComplexData::smtComplex  q_old;  // ԭʼ����

    QColor      q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QSpinBox        *q_spinSequence;    // ʱ����
    LimitTextEdit   *q_edtRemak;
};

#endif // FRAME_EDIT_H
