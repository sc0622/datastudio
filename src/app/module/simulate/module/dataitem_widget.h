#ifndef DATAITEM_WIDGET_H
#define DATAITEM_WIDGET_H

#include <QFrame>
#include "icdcore/icd_item.h"

namespace Icd {

template<typename T> class std::shared_ptr;

class Item;
typedef std::shared_ptr<Item> ItemPtr;

class Worker;
typedef std::shared_ptr<Worker> WorkerPtr;

}   // end of namespace Icd

class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;
class QStandardItem;
class QPushButton;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QLabel;
class QSlider;
class QCheckBox;
class JSpinBox;

namespace JUtralEdit { class JView; }

// class DataItemWidget

namespace Simulate {

class DataTableWidget;
class IcdTabWidget;

class DataItemWidget : public QFrame
{
    Q_OBJECT
public:
    enum ItemType {
        ItemTypeInvalid = -1,
        ItemTypeHead,
        ItemTypeCounter,
        ItemTypeCheck,
        ItemTypeFrameCode,
        ItemTypeNumeric,
        ItemTypeArray,
        ItemTypeBitMap,
        ItemTypeBitValue,
        ItemTypeComplex,
        ItemTypeFrame
    };

    explicit DataItemWidget(ItemType itemType, QWidget *parent = nullptr);
    ~DataItemWidget();

    static DataItemWidget *createWidget(const Icd::ItemPtr &dataItem, QWidget *parent = nullptr);

    ItemType itemType() const;
    const Icd::WorkerPtr &worker() const;
    virtual void setWorker(const Icd::WorkerPtr &worker);
    Icd::ItemPtr dataItem() const;
    bool setDataItem(const Icd::ItemPtr &dataItem);
    QStandardItem *item() const;
    void setItem(QStandardItem *item);
    void updateUi();
    virtual bool addDataItem(const QString &domain);
    Q_INVOKABLE virtual int focusItem(const QString &domain);
    Q_INVOKABLE virtual void removeItem(const QString &domain);

signals:
    void send();
    void remove();
    void heightChanged(int height);

public slots:

protected:
    QHBoxLayout *clientLayout();
    QVBoxLayout *itemLayout();
    int titleBarHeight() const;
    virtual void restoreUi(const Icd::ItemPtr &data);
    virtual bool updateUi(const Icd::ItemPtr &data);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    ItemType d_itemType;
    Icd::WorkerPtr d_worker;
    Icd::ItemPtr d_defaultData;
    Icd::ItemPtr d_dataItem;
    QStandardItem *d_item;
    QLabel *d_labelTitle;
    QPushButton *d_buttonSend;
    QHBoxLayout *d_clientLayout;
    QVBoxLayout *d_itemLayout;
};

// class ItemWidgetHead

class ItemWidgetHead : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetHead(QWidget *parent = nullptr);
    ~ItemWidgetHead();

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    JSpinBox *d_spinValue;
};

// class ItemWidgetCounter

class ItemWidgetCounter : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetCounter(QWidget *parent = nullptr);
    ~ItemWidgetCounter();

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    QSpinBox *d_spinValue;
};

// class ItemWidgetCheck

class ItemWidgetCheck : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetCheck(QWidget *parent = nullptr);
    ~ItemWidgetCheck();

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    QSpinBox *d_spinValue;
    QComboBox *d_comboBoxCheckType;
    QSpinBox *d_spinStartPos;
    QSpinBox *d_spinEndPos;
    QLabel *d_labelCheckLength;
};

// class ItemWidgetFrameCode

class ItemWidgetFrameCode : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetFrameCode(QWidget *parent = nullptr);
    ~ItemWidgetFrameCode();

    void setWorker(const Icd::WorkerPtr &worker);

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    QComboBox *d_comboBoxCode;
};

// class ItemWidgetNumeric

class ItemWidgetNumeric : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetNumeric(QWidget *parent = nullptr);
    ~ItemWidgetNumeric();

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    QSlider *d_sliderValue;
    QDoubleSpinBox *d_spinValue;
    QDoubleSpinBox *d_spinData;
    QLabel *d_labelDesc;
    QCheckBox *d_checkBoxLink;
};

// class ItemWidgetArray

class ItemWidgetArray : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetArray(QWidget *parent = nullptr);
    ~ItemWidgetArray();

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    JUtralEdit::JView *dataView_;
    QLabel *d_labelDesc;
};

// class ItemWidgetBitMap

class ItemWidgetBitMap : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetBitMap(QWidget *parent = nullptr);
    ~ItemWidgetBitMap();

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    QMap<int/*offset*/, QCheckBox *> d_checkBoxes;
    QFormLayout *d_formLayout;
    QDoubleSpinBox *d_spinData;
    QLabel *d_labelDesc;
};

// class ItemWidgetBitValue

class ItemWidgetBitValue : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetBitValue(QWidget *parent = nullptr);
    ~ItemWidgetBitValue();

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    QDoubleSpinBox *d_spinData;
    QLabel *d_labelDesc;
};

// class ItemWidgetComplex

class ItemWidgetComplex : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetComplex(QWidget *parent = nullptr);
    ~ItemWidgetComplex();

    bool addDataItem(const QString &domain);
    Q_INVOKABLE int focusItem(const QString &domain);
    Q_INVOKABLE void removeItem(const QString &domain);

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    void fixHeight();

private:
    DataTableWidget *d_tableWidget;
};

// class ItemWidgetFrame

class ItemWidgetFrame : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetFrame(QWidget *parent = nullptr);
    ~ItemWidgetFrame();

    bool addDataItem(const QString &domain);
    Q_INVOKABLE int focusItem(const QString &domain);
    Q_INVOKABLE void removeItem(const QString &domain);

protected:
    void restoreUi(const Icd::ItemPtr &data);
    bool updateUi(const Icd::ItemPtr &data);

private:
    void fixHeight(int index);

private:
    IcdTabWidget *d_tabWidget;
};

}

#endif // DATAITEM_WIDGET_H
