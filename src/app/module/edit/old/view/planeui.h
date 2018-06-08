#ifndef PLANEUI_H
#define PLANEUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/globalstruct.h"

class LimitTextEdit;
class LimitLineEdit;
/**
* @brief ���ͽ�����
*/
class PlaneUI : public MetaUI
{
    Q_OBJECT

public:
    PlaneUI(QWidget *parent = nullptr);
    ~PlaneUI();

    // MetaUI interface
public:
    void setUIData(const _UIData &data) override;
    void *uiData() const override;

signals:

protected slots:
    // �༭���ı����
    void slotTextChanged(const QString &text);
    // ����¼�����
    void slotEditFinished();

protected:
    // ȷ��
    void confirm() override;
    // ȡ��
    void cancel() override;

    // ��ʼ����������
    void init();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable) override;
    // У���������
    bool dataValid();

private:
    stPlane     q_data;     // ������Ϣ
    stPlane     q_old;      // ԭʼ����
    QColor      q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;
    
};

#endif // PLANEUI_H
