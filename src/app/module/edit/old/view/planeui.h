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
    void setUIData(const _UIData &data);
    void *uiData() const;
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots:
    // �༭���ı����
    void slotTextChanged(const QString &text);
    // ����¼�����
    void slotEditFinished();
    // ȷ��
    void slotConfirm();
    // ȡ��
    void slotCanceled();

protected:
    // ��ʼ����������
    void init();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    stPlane     q_data;     // ������Ϣ
    stPlane     q_old;      // ԭʼ����
    QColor      q_color;    // Ĭ�ϱ���ɫ

    QLineEdit       *q_edtStatus; // ״̬��ʾ
    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
    
};

#endif // PLANEUI_H
