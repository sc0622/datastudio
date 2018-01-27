#ifndef HEADERUI_H
#define HEADERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "icdcommondata.h"

class LimitTextEdit;
class LimitLineEdit;
class hexSpinBox;
class HeaderUI : public MetaUI
{
    Q_OBJECT
public:
    explicit HeaderUI(IJAttempter &attempter, QWidget *parent = 0);
    // MetaUI interface
    void setUIData(const _UIData &data);
    void *uiData() const;
    // �л���������
    void changeDataType(int type);
    // ԭʼ��������
    int originalType() const;
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots:
    // �༭������¼�����
    void slotEditFinished();
    // �༭���ı����
    void slotTextChanged(const QString& text);
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
    IJAttempter                 &q_attempter;

    ICDCommonData::smtCommon    q_data; // ��������
    ICDCommonData::smtCommon    q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    QLineEdit       *q_edtStatus; // ״̬��ʾ
    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    hexSpinBox      *q_spinDefault;
    LimitTextEdit   *q_edtRemak;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
};

#endif // HEADERUI_H
