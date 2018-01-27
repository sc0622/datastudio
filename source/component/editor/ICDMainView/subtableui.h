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
    explicit SubTableUI(IJAttempter &attempter, QWidget* parent = 0);
    // MetaUI interface
    void setUIData(const _UIData &data);
    void* uiData() const;
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots:
    // �༭���ı����
    void slotTextChanged(const QString& text);
    // ֡�����ݱ��
    void slotCodeChanged(const qulonglong &value);
    // ����¼�����
    void slotEditFinished();
    // ȷ��
    void slotConfirm();
    // ȡ��
    void slotCanceled();

protected:
    // ��ʼ����������
    void init();
    // ��ʼ��֡�볤��������
    void initBoxLenght();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    IJAttempter     &q_attempter;

    stICDBase       q_data;     // ������Ϣ
    stICDBase       q_old;      // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    QLineEdit       *q_edtStatus;   // ״̬��ʾ
    LimitLineEdit   *q_edtName;
    QComboBox       *q_boxType;   // ֡�볤��
    JLargeSpinBox   *q_spinCode;
    QSpinBox        *q_spinSequence;
    LimitTextEdit   *q_edtDescribe;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
};

#endif // SUBTABLEUI_H
