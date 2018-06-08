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

    int uiType() const override;

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

    JXmlTable   *q_table;       // ����ֵ��
    QAction     *q_actionNew;   // �Ҽ��˵�����
    QAction     *q_actionClear; // �Ҽ��˵����
};

#endif // BITVALUEUI_H
