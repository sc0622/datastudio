#ifndef OBJECT_EDIT_H
#define OBJECT_EDIT_H

#include <QWidget>
#include "KernelClass/icdmetadata.h"

struct _UIData
{
    void *data;      // 初始化数据
    GlobalDefine::OptionType type;   // 操作类型

    _UIData() : data(0), type(GlobalDefine::optEdit) {}
};

struct _UIPos
{
    int row;
    int column;
    int rowSpan;
    int columnSpan;

    _UIPos() : row(0), column(0), rowSpan(1), columnSpan(1) {}
};

struct _Eigenvalue
{
    double  value;      // 数据
    QString describe;   // 描述

    _Eigenvalue() : value(0.0) {}
};

class QVBoxLayout;
class QFormLayout;
class QGroupBox;
class QLineEdit;
class QPushButton;
class JSplitter;
class LimitLineEdit;
class LimitTextEdit;

class ObjectEdit : public QWidget
{
    Q_OBJECT
public:
    enum WindowType {
        wdUnknown = -1,
        wdHeader,
        wdCounter,
        wdCheck,
        wdFrameCode,
        wdItem,
        wdArray,
        wdNumeric,
        wdBitMap,
        wdBitValue,
        wdFrame,
        wdComplex,
        wdBuffer,
        wdSubTable,
        wdTable,
        wdSystem,
        wdVehicle,
    };
    Q_ENUM(WindowType)

    explicit ObjectEdit(QWidget *parent = nullptr);
    virtual ~ObjectEdit();

    virtual int windowType() const;
    virtual void changeType(int type);
    virtual int originalType() const;
    virtual void *nonData();
    virtual void *nonOldData();
    virtual bool setData(const _UIData &data);

    void enableCommit(bool enabled);

    static ObjectEdit *create(int windowType);

signals:
    void confirmed(bool &);
    void canceled();

protected slots:
    virtual bool onEditFinished();
    virtual bool onTextChanged(const QString &text);

protected:
    QVBoxLayout *layoutMain() const;
    JSplitter *splitterBase() const;
    QFormLayout *layoutBase() const;
    void addFormRow(QWidget *label, QWidget *field) const;
    void addFormRow(QWidget *label, QLayout *field) const;
    void addFormRow(const QString &labelText, QWidget *field) const;
    void addFormRow(const QString &labelText, QLayout *field) const;

    virtual bool init();
    virtual void enableConnect(bool enabled);
    virtual bool confirm();
    virtual void cancel();
    virtual bool validate();

    void enableConfirm(bool enabled);
    void enabledCancel(bool enabled);
    void setStatus(const QString &text = QString());

    virtual ICDMetaData *data();
    virtual ICDMetaData *oldData();
    bool setExtData(const _UIData &data, bool base);

    LimitTextEdit *editDesc() const;

    virtual QString name() const;
    virtual void setName(const QString &text);

    virtual QString mark() const;
    virtual void setMark(const QString &text);

    virtual QString desc() const;
    virtual void setDesc(const QString &text);

private:
    bool _init();
    bool _validate();

private:
    ICDMetaData::smtMeta data_;
    ICDMetaData::smtMeta oldData_;
    QVBoxLayout *layoutMain_;
    JSplitter *splitterBase_;
    QGroupBox *groupBoxBase_;
    QFormLayout *layoutBase_;
    QLineEdit *editName_;
    QLineEdit *editMark_;
    LimitTextEdit *editDesc_;
    QPushButton *buttonConfirm_;
    QPushButton *buttonCancel_;
    QLineEdit *editStatus_;
    bool isBaseData_;
};

#endif // OBJECT_EDIT_H
