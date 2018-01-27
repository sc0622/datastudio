#ifndef BITMAPUI_H
#define BITMAPUI_H

#include<QtWidgets>

#include "metaui.h"
#include "icdbitdata.h"

class JXmlTable;
class LimitTextEdit;
class LimitLineEdit;
// bitMap��
class BitMapUI : public MetaUI
{
    Q_OBJECT
public:
    explicit BitMapUI(IJAttempter &attempter, QWidget *parent = 0);
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

protected slots :
    // ����¼�����
    void slotEditFinished();
    // �༭�ı����
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
    // ��������ֵ������
    void updateTable(int newCount, bool behind = true);
    // ������һ������
    void updateMapOne(int index, const _Eigenvalue &data,
                      int option = GlobalDefine::optNew);
    // ��ȡ����ֵ������
    std::map<double, std::string> tableValues() const;

private:
    IJAttempter     &q_attempter;

    ICDBitData::smtBit  q_data; // ��������
    ICDBitData::smtBit  q_old;  // ԭʼ����

    QColor          q_color;    // Ĭ�ϱ���ɫ

    LimitLineEdit   *q_edtName;     // ����
    QLineEdit       *q_edtCode;     // ����
    QSpinBox        *q_spinStart;   // ��ʼλ
    QSpinBox        *q_spinLength;  // ����
    QLineEdit       *q_edtDefault;  // Ĭ��ֵ
    LimitTextEdit   *q_edtDescribe; // ����

    QLineEdit       *q_edtStatus;   // ״̬��ʾ
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;

    JXmlTable       *q_table;       // ����ֵ��
};

#endif // BITMAPUI_H
