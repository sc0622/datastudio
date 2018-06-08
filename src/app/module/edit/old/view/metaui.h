#ifndef METAUI_H
#define METAUI_H

#include <QWidget>
#include "KernelClass/globalstruct.h"

/**
* @brief 初始化数据结构
*/
struct _UIData
{
    void    *data;      // 初始化数据
    GlobalDefine::OptionType   type;   // 操作类型

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
    double  value;      // 数据
    QString describe;   // 描述

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
    // 创建界面
    explicit MetaUI(QWidget *parent = nullptr);
    // 设置界面数据
    virtual void setUIData(const _UIData &data);
    // 获取界面编辑数据
    virtual void* uiData() const;
    // 切换数据类型
    virtual void changeDataType(int type);
    // 原始数据类型
    virtual int originalType() const;
    // 启/停用信号槽
    virtual void enableConnection(bool enable);
    // 启/停用界面控制按钮
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
