#ifndef BITMAPU_EDIT_H
#define BITMAPU_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdbitdata.h"

class QSpinBox;

class BitMapEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit BitMapEdit(QWidget *parent = nullptr);

signals:

public slots:

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    bool confirm() override;
    bool validate();
    ICDBitData *data() override;
    ICDBitData *oldData() override;

private:
    // 更新特征值表数据
    void updateTable(int newCount, bool behind = true);
    // 向表插入一条数据
    void updateMapOne(int index, const _Eigenvalue &data, int option = GlobalDefine::optNew);
    // 获取特征值表数据
    std::map<double, std::string> tableValues() const;

private:
    QSpinBox *spinBitStart_;        // 起始位
    QSpinBox *spinBitCount_;        // 长度
    QLineEdit *editDefault_;        // 默认值
    JXmlTable *tableView_;          // 特征值表
};

#endif // BITMAPU_EDIT_H
