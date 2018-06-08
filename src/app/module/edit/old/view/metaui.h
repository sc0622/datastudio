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
    // ��������
    explicit MetaUI(QWidget *parent = nullptr);
    // ���ý�������
    virtual void setUIData(const _UIData &data);
    // ��ȡ����༭����
    virtual void* uiData() const;
    // �л���������
    virtual void changeDataType(int type);
    // ԭʼ��������
    virtual int originalType() const;
    // ��/ͣ���źŲ�
    virtual void enableConnection(bool enable);
    // ��/ͣ�ý�����ư�ť
    void enableOptionButton(bool enable);

    QVBoxLayout *layoutMain() const;
    QPushButton *buttonConfirm() const;
    QPushButton *buttonCancel() const;
    QLineEdit *editStatus() const;

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
