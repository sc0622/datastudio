#ifndef CHECKUI_H
#define CHECKUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcheckdata.h"

class LimitTextEdit;
class LimitLineEdit;
class CheckUI : public MetaUI
{
    Q_OBJECT
public:
    explicit CheckUI(QWidget *parent = nullptr);
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
    // ��ʼ��У������������
    void initBoxType();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();

private:
    ICDCheckData::smtCheck      q_data; // ��������
    ICDCheckData::smtCheck      q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxCheckType;    // У������
    QSpinBox        *q_spinStart;       // У����ʼλ
    QSpinBox        *q_spinEnd;         // У����ֹλ
    LimitTextEdit   *q_edtRemak;

    QLineEdit   *q_edtStatus;       // ״̬��ʾ
    QPushButton *q_btnConfirm;
    QPushButton *q_btnCancel;
};

#endif // CHECKUI_H
