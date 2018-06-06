#ifndef DISCERNUI_H
#define DISCERNUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcomplexdata.h"

class LimitTextEdit;
class LimitLineEdit;
// ֡����
class DiscernUI : public MetaUI
{
    Q_OBJECT

public:
    explicit DiscernUI(QWidget *parent = nullptr);
    ~DiscernUI();
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
//    QSpinBox        *q_spinLength;      // ����������
    QSpinBox        *q_spinSequence;    // ʱ����
    LimitTextEdit   *q_edtRemak;

    QLineEdit   *q_edtStatus;       // ״̬��ʾ
    QPushButton *q_btnConfirm;
    QPushButton *q_btnCancel;
};

#endif // DISCERNUI_H
