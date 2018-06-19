#ifndef METAUI_H
#define METAUI_H

#include <QWidget>
#include "KernelClass/globalstruct.h"

/**
* @brief ��ʼ�����ݽṹ
*/
struct _UIData
{
    void    *data;      // ��ʼ������
    GlobalDefine::OptionType   type;   // ��������

    _UIData() :data(0), type(GlobalDefine::optEdit){}
};

struct _UIPos
{
    int row;
    int column;
    int rowSpan;
    int columnSpan;

    _UIPos() :row(0), column(0), rowSpan(1), columnSpan(1)
    {}
};

struct _Eigenvalue
{
    double  value;      // ����
    QString describe;   // ����

    _Eigenvalue() : value(0.0)
    {}
};

class QVBoxLayout;
class QPushButton;
class QLineEdit;

class MetaUI : public QWidget
{
    Q_OBJECT
public:
    enum MetaUiType {
        wdUnknown = -1,
        wdHeader,   // ��ͷ����
        wdCounter,  // ֡��������
        wdCheck,    // У�����
        wdFrameCode,// ֡ʶ����
        wdCommon,   // ͨ�ý���
        wdArray,    // �������
        wdBitMap,   // bitMap����
        wdBitValue, // bitValue����
        wdDiscern,  // ֡���ݽ���
        wdComplex,  // �������ݽ���
        wdBuffer,   // ����Ԥ��������
        wdSubTable, // �ӱ����
        wdTable,    // �����
        wdSystem,   // ϵͳ����
        wdPlane,    // ���ͽ���
    };
    Q_ENUM(MetaUiType)

    // ��������
    explicit MetaUI(QWidget *parent = nullptr);

    virtual int uiType() const;

    // ���ý�������
    virtual void setUIData(const _UIData &data);
    // ��ȡ����༭����
    virtual void* uiData() const;
    // �л���������
    virtual void changeDataType(int type);
    // ԭʼ��������
    virtual int originalType() const;
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

    QVBoxLayout *layoutMain() const;
    QPushButton *buttonConfirm() const;
    QPushButton *buttonCancel() const;
    QLineEdit *editStatus() const;

    static MetaUI *create(int metaUiType);

signals:
    void confirmed(bool &);
    void canceled();

public slots:

protected:
    virtual void confirm();
    virtual void cancel();

private:
    QVBoxLayout *layoutMain_;
    QPushButton *buttonConfirm_;
    QPushButton *buttonCancel_;
    QLineEdit   *editStatus_;
};

#endif // METAUI_H
