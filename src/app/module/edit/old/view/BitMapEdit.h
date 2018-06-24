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
    // ��������ֵ������
    void updateTable(int newCount, bool behind = true);
    // ������һ������
    void updateMapOne(int index, const _Eigenvalue &data, int option = GlobalDefine::optNew);
    // ��ȡ����ֵ������
    std::map<double, std::string> tableValues() const;

private:
    QSpinBox *spinBitStart_;        // ��ʼλ
    QSpinBox *spinBitCount_;        // ����
    QLineEdit *editDefault_;        // Ĭ��ֵ
    JXmlTable *tableView_;          // ����ֵ��
};

#endif // BITMAPU_EDIT_H
