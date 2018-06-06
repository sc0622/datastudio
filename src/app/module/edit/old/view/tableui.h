#ifndef TABLEUI_H
#define TABLEUI_H

#include <QtWidgets>

#include "metaui.h"

class LimitTextEdit;
class LimitLineEdit;
class TableUI : public MetaUI
{
    Q_OBJECT
public:
    explicit TableUI(QWidget* parent = nullptr);
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
    // ����¼�����
    void slotEditFinished();
    //
    void slotCheckStateChanged(bool checked);
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
    stICDBase       q_data;     // ������Ϣ
    stICDBase       q_old;      // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    QLineEdit       *q_edtStatus; // ״̬��ʾ
    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QSpinBox        *q_spinLength;
    QCheckBox       *q_check;
    LimitTextEdit   *q_edtDescribe;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
};

#endif // TABLEUI_H
