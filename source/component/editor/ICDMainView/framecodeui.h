#ifndef FRAMECODEUI_H
#define FRAMECODEUI_H

#include <QtWidgets>

#include "metaui.h"
#include "icdframecodedata.h"

class LimitTextEdit;
class LimitLineEdit;
// ֡ʶ����
class FrameCodeUI : public MetaUI
{
    Q_OBJECT
public:
    explicit FrameCodeUI(IJAttempter &attempter, QWidget *parent = 0);
    // MetaUI interface
    void setUIData(const _UIData &data);
    void*uiData() const;
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
    // ��ʼ��֡�볤��������
    void initBoxLenght();
    // ��ʼ��֡����������
    void initBoxData();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    IJAttempter     &q_attempter;

    ICDFrameCodeData::smtFrameCode  q_data; // ��������
    ICDFrameCodeData::smtFrameCode  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxLength;   // ֡�볤��
    QComboBox       *q_boxData;     // ֡����
    LimitTextEdit   *q_edtRemak;

    QLineEdit   *q_edtStatus;       // ״̬��ʾ
    QPushButton *q_btnConfirm;
    QPushButton *q_btnCancel;
};

#endif // FRAMECODEUI_H
