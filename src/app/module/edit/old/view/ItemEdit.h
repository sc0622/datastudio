#ifndef ITEM_EDIT_H
#define ITEM_EDIT_H

#include "ObjectEdit.h"
#include "KernelClass/icdcommondata.h"

class QComboBox;
class QCheckBox;
class QSpinBox;
class JXmlTable;
class LimitTextEdit;
class LimitLineEdit;
class DoubleSpinbox;

class ItemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ItemEdit(QWidget* parent = nullptr);
    explicit ItemEdit(int windowType, QWidget* parent = nullptr);
    virtual ~ItemEdit();

    virtual int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;
    // �л���������
    void changeDataType(int type) override;
    // ԭʼ��������
    int originalType() const override;

signals:

private slots:
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
    void setupUI(int windowType);
    // ȷ��
    void confirm() override;
    // ȡ��
    void cancel() override;

    // ��ʼ����������
    void init();
    // ��ʼ����ֵ����������
    void initBoxType();
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
    ICDCommonData::smtCommon    q_data; // ��������
    ICDCommonData::smtCommon    q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxNumericType;  // ��ֵ����
    LimitTextEdit   *q_edtRemak;

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

    JXmlTable   *q_table;   // ����ֵ��
    QAction     *q_actionNew;   // �Ҽ��˵�����
    QAction     *q_actionClear; // �Ҽ��˵����
};

#endif // ITEM_EDIT_H
