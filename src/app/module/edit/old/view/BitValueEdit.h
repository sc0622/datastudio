#ifndef BITVALUE_EDIT_H
#define BITVALUE_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdbitdata.h"

class QSpinBox;

class BitValueEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit BitValueEdit(QWidget* parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;
    // �л���������
    void changeDataType(int type) override;
    // ԭʼ��������
    int originalType() const override;

signals:

public slots:
    // ����¼�����
    void slotEditFinished();
    // �༭�ı����
    void slotTextChanged(const QString& text);
    // ��Χ����״̬���
    void slotRangeStateChanged(bool enable);
    // ����
    void slotTableItemPressed(QStandardItem *item);
    // ����
    void slotNew();
    // ɾ��
    void slotDelete();
    // ���
    void slotClear();

private:
    // ȷ��
    void confirm() override;
    // ȡ��
    void cancel() override;

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
    DoubleSpinbox *q_spinMin;
    QCheckBox   *q_checkLower;
    DoubleSpinbox *q_spinMax;
    QCheckBox   *q_checkUpper;
    // Ĭ��ֵ
    DoubleSpinbox *q_spinDefault;
    // ƫ��
    DoubleSpinbox *q_spinOffset;
    // ��λ
    LimitLineEdit   *q_edtUnit;
    // ������
    DoubleSpinbox *q_spinLSB;

    JXmlTable   *q_table;       // ����ֵ��
    QAction     *q_actionNew;   // �Ҽ��˵�����
    QAction     *q_actionClear; // �Ҽ��˵����
};

#endif // BITVALUE_EDIT_H
