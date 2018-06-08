#ifndef SUBTABLEUI_H
#define SUBTABLEUI_H

#include <QtWidgets>

#include "metaui.h"

class LimitTextEdit;
class LimitLineEdit;
class JLargeSpinBox;
class SubTableUI : public MetaUI
{
    Q_OBJECT
public:
    explicit SubTableUI(QWidget* parent = nullptr);
    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;

signals:

protected slots:
    // �༭���ı����
    void slotTextChanged(const QString& text);
    // ֡�����ݱ��
    void slotCodeChanged(const qulonglong &value);
    // ����¼�����
    void slotEditFinished();

protected:
    // ȷ��
    void confirm() override;
    // ȡ��
    void cancel() override;

    // ��ʼ����������
    void init();
    // ��ʼ��֡�볤��������
    void initBoxLenght();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable) override;
    // У���������
    bool dataValid();

private:
    stICDBase       q_data;     // ������Ϣ
    stICDBase       q_old;      // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QComboBox       *q_boxType;   // ֡�볤��
    JLargeSpinBox   *q_spinCode;
    QSpinBox        *q_spinSequence;
    LimitTextEdit   *q_edtDescribe;
};

#endif // SUBTABLEUI_H
