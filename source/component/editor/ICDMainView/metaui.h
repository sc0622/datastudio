#ifndef METAUI_H
#define METAUI_H

#include <QWidget>
#include "globalstruct.h"

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

class MetaUI : public QDialog
{
    Q_OBJECT
public:
    // ��������
    explicit MetaUI(QWidget *parent = 0);
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
    virtual void enableOptionButton(bool enable);

protected:

signals:

public slots:
};

#endif // METAUI_H
