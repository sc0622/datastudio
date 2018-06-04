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
        ItemTypeBitMap,
        ItemTypeBitValue,
        ItemTypeComplex,
        ItemTypeFrame
    };

    /**
     * @brief DataItemWidget
     * @param itemType
     * @param parent
     */
    explicit DataItemWidget(ItemType itemType, QWidget *parent = 0);

    ~DataItemWidget();

    /**
     * @brief createWidget
     * @param dataItem
     * @param parent
     * @return
     */
    static DataItemWidget *createWidget(const Icd::ItemPtr &dataItem,
                                        QWidget *parent = 0);

    /**
     * @brief itemType
     * @return
     */
    ItemType itemType() const;

    /**
     * @brief worker
     * @return
     */
    const Icd::WorkerPtr &worker() const;

    /**
     * @brief setWorker
     * @param worker
     */
    virtual void setWorker(const Icd::WorkerPtr &worker);

    /**
     * @brief dataItem
     * @return
     */
    Icd::ItemPtr dataItem() const;

    /**
     * @brief setDataItem
     * @param worker
     * @return
     */
    bool setDataItem(const Icd::ItemPtr &dataItem);

    /**
     * @brief item
     * @return
     */
    QStandardItem *item() const;

    /**
     * @brief setItem
     * @param item
     */
    void setItem(QStandardItem *item);

    /**
     * @brief updateUi
     */
    void updateUi();

    /**
     * @brief addDataItem
     * @param domain
     * @return
     */
    virtual bool addDataItem(const QString &domain);

    /**
     * @brief focusItem
     * @param domain
     * @return
     */
    Q_INVOKABLE virtual int focusItem(const QString &domain);

    /**
     * @brief removeItem
     * @param domain
     */
    Q_INVOKABLE virtual void removeItem(const QString &domain);

signals:
    void send();
    void remove();
    void heightChanged(int height);

public slots:

protected:
    /**
     * @brief clientLayout
     * @return
     */
    QHBoxLayout *clientLayout();

    /**
     * @brief itemLayout
     * @return
     */
    QVBoxLayout *itemLayout();

    /**
     * @brief titleBarHeight
     * @return
     */
    int titleBarHeight() const;

    /**
     * @brief restoreUi
     * @param data
     */
    virtual void restoreUi(const Icd::ItemPtr &data);

    /**
     * @brief updateUi
     * @param data
     * @return
     */
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
    explicit ItemWidgetHead(QWidget *parent = 0);

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
    explicit ItemWidgetCounter(QWidget *parent = 0);

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
    explicit ItemWidgetCheck(QWidget *parent = 0);

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
    explicit ItemWidgetFrameCode(QWidget *parent = 0);

    ~ItemWidgetFrameCode();

    /**
     * @brief setWorker
     * @param worker
     */
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
    explicit ItemWidgetNumeric(QWidget *parent = 0);

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

// class ItemWidgetBitMap

class ItemWidgetBitMap : public DataItemWidget
{
    Q_OBJECT
public:
    explicit ItemWidgetBitMap(QWidget *parent = 0);

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
    explicit ItemWidgetBitValue(QWidget *parent = 0);

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
    explicit ItemWidgetComplex(QWidget *parent = 0);

    ~ItemWidgetComplex();

    /**
     * @brief addDataItem
     * @param domain
     * @return
     */
    bool addDataItem(const QString &domain);

    /**
     * @brief focusItem
     * @param domain
     * @return
     */
    Q_INVOKABLE int focusItem(const QString &domain);

    /**
     * @brief removeItem
     * @param domain
     */
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
    explicit ItemWidgetFrame(QWidget *parent = 0);

    ~ItemWidgetFrame();

    /**
     * @brief addDataItem
     * @param domain
     * @return
     */
    bool addDataItem(const QString &domain);

    /**
     * @brief focusItem
     * @param domain
     * @return
     */
    Q_INVOKABLE int focusItem(const QString &domain);

    /**
     * @brief removeItem
     * @param domain
     */
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
