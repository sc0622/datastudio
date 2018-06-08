#ifndef BITMAPUI_H
#define BITMAPUI_H

#include<QtWidgets>

#include "metaui.h"
#include "KernelClass/icdbitdata.h"

class JXmlTable;
class LimitTextEdit;
class LimitLineEdit;
// bitMap��
class BitMapUI : public MetaUI
{
    Q_OBJECT
public:
    explicit BitMapUI(QWidget *parent = nullptr);

    int uiType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // �л���������
    void changeDataType(int type) override;
    // ԭʼ��������
    int originalType() const override;

signals:

public slots :
    // ����¼�����
    void slotEditFinished();
    // �༭�ı����
    void slotTextChanged(const QString& text);

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
    // ��������ֵ������
    void updateTable(int newCount, bool behind = true);
    // ������һ������
    void updateMapOne(int index, const _Eigenvalue &data,
                      int option = GlobalDefine::optNew);
    // ��ȡ����ֵ������
    std::map<double, std::string> tableValues() const;

private:
    ICDBitData::smtBit  q_data; // ��������
    ICDBitData::smtBit  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;     // ����
    QLineEdit       *q_edtCode;     // ����
    QSpinBox        *q_spinStart;   // ��ʼλ
    QSpinBox        *q_spinLength;  // ����
    QLineEdit       *q_edtDefault;  // Ĭ��ֵ
    LimitTextEdit   *q_edtDescribe; // ����

    JXmlTable       *q_table;       // ����ֵ��
};

#endif // BITMAPUI_H
