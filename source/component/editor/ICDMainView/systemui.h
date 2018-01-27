#ifndef SYSTEMUI_H
#define SYSTEMUI_H

#include <QtWidgets>

#include "metaui.h"
#include "globalstruct.h"

class LimitTextEdit;
class LimitLineEdit;
/**
* @brief ϵͳ�ṹ������
*/
class SystemUI : public MetaUI
{
    Q_OBJECT

public:
    explicit SystemUI(IJAttempter &attempter, QWidget *parent = 0);
    ~SystemUI();

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
    // �༭������¼�����
    void slotEditFinished();
    // �༭���ı����
    void slotTextChanged(const QString &text);
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
    IJAttempter &q_attempter;

    stSystem    q_data;     // ������Ϣ
    stSystem    q_old;      // ԭʼ����
    QColor      q_color;    // Ĭ�ϱ���ɫ

    QLineEdit       *q_edtStatus; // ״̬��ʾ
    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;

    QGridLayout     *q_gridLayout;
};

#endif // SYSTEMUI_H
