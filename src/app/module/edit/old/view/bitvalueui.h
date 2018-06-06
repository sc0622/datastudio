#ifndef BITVALUEUI_H
#define BITVALUEUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdbitdata.h"

class JXmlTable;
class LimitTextEdit;
class LimitLineEdit;
class CustomDoubleSpinbox;
// bitValue��
class BitValueUI : public MetaUI
{
    Q_OBJECT
public:
    explicit BitValueUI(QWidget* parent = nullptr);
    // MetaUI interface
    void setUIData(const _UIData &data);
    void* uiData() const;
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
    // ����¼�����
    void slotEditFinished();
    // �༭�ı����
    void slotTextChanged(const QString& text);
    // ��Χ����״̬���
    void slotRangeStateChanged(bool enable);
    // ����
    void slotTableItemPressed(QStandardItem *item);
    // ȷ��
    void slotConfirm();
    // ȡ��
    void slotCanceled();
    // ����
    void slotNew();
    // ɾ��
    void slotDelete();
    // ���
    void slotClear();

protected:
    // ��ʼ����������
    void init();
    // ��/ͣ���źŲ�
    void enableConnection(bool enable);
    // У���������
    bool dataValid();
    // ��ʾ�������Ҽ��˵�
    void showMenu();
    // ������˵�����
    void dealAction(QAction *action);
    // ������ֵ�����һ������
    void insertValueOne(int index, const _Eigenvalue &data);

private:
    ICDBitData::smtBit  q_data; // ��������
    ICDBitData::smtBit  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtRemak;

    QSpinBox    *q_spinStart;   // ��ʼλ
    QSpinBox    *q_spinLength;  // ����
    // ��Χ
    CustomDoubleSpinbox *q_spinMin;
    QCheckBox   *q_checkLower;
    CustomDoubleSpinbox *q_spinMax;
    QCheckBox   *q_checkUpper;
    // Ĭ��ֵ
    CustomDoubleSpinbox *q_spinDefault;
    // ƫ��
    CustomDoubleSpinbox *q_spinOffset;
    // ��λ
    LimitLineEdit   *q_edtUnit;
    // ������
    CustomDoubleSpinbox *q_spinLSB;
    // ��ʾ��Ϣ
    QLineEdit   *q_edtStatus;
    QPushButton *q_btnConfirm;
    QPushButton *q_btnCancel;

    JXmlTable   *q_table;       // ����ֵ��
    QAction     *q_actionNew;   // �Ҽ��˵�����
    QAction     *q_actionClear; // �Ҽ��˵����
};

#endif // BITVALUEUI_H
