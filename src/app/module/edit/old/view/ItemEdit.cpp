#include "precomp.h"
#include "ItemEdit.h"
#include "DoubleSpinBox.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"
#include "KernelClass/icdnumericdata.h"
#include "jwt/jxmltable.h"
#include "jwt/jsplitter.h"

ItemEdit::ItemEdit(QWidget *parent)
    : ObjectEdit(parent)
    , comboNumericType_(nullptr)
{
    setupUI(windowType());
}

ItemEdit::ItemEdit(int windowType, QWidget *parent)
    : ObjectEdit(parent)
    , comboNumericType_(nullptr)
{
    setupUI(windowType);
}

ItemEdit::~ItemEdit()
{

}

void ItemEdit::onRangeStateChanged(bool enabled)
{
    QStringList lstRange = QString(data()->range().c_str()).split("~");
    if (std::string::npos == data()->range().find("~")) {
        lstRange.push_back(QString());
    }

    const QObject *sender = this->sender();
    if (sender == checkLeftInf_) {
        spinMinimum_->setEnabled(enabled);
        spinMinimum_->clear();
        if (enabled) {
            spinMinimum_->setFocus();
            lstRange[0] = spinMinimum_->text();
            data()->setRange(lstRange.join("~").toStdString());
        } else {
            lstRange[0].clear();
            data()->setRange(lstRange.join("~").toStdString());
        }
    } else if (sender == checkRightInf_) {
        spinMaximum_->setEnabled(enabled);
        spinMaximum_->clear();
        if (enabled) {
            spinMaximum_->setFocus();
            lstRange[1] = spinMaximum_->text();
            data()->setRange(lstRange.join("~").toStdString());
        } else {
            lstRange[1].clear();
            data()->setRange(lstRange.join("~").toStdString());
        }
    }

    enableCommit(true);
}

void ItemEdit::onTableItemPressed(QStandardItem *item)
{
    Q_UNUSED(item);
    if (Qt::RightButton == QApplication::mouseButtons()) {
        showMenu();
    }
}

void ItemEdit::onNew()
{
    _Eigenvalue data;
    int index = tableView_->rowCount();
    if (index > 0) {
        data.value = tableView_->itemValue(index - 1, 0).toDouble() + 1.0;
    }
    insertValueOne(index, data);
    tableView_->clearSelection();
    tableView_->setCurrentCell(index, 1);
    enableCommit(true);
}

void ItemEdit::onDelete()
{
    const int result = QMessageBox::question(tableView_,
                                             QStringLiteral("删除确认"),
                                             QStringLiteral("确认删除当前选中数据？"));
    if (result == QMessageBox::No) {
        return;
    }

    std::map<int, int> items;
    QList<JTableViewSelectionRange> lst = tableView_->selectRanges();
    QListIterator<JTableViewSelectionRange> it = lst;
    while (it.hasNext()) {
        const JTableViewSelectionRange &range = it.next();
        items[range.topRow()] = qMax(items[range.topRow()], range.rowCount());
    }
    std::map<int, int>::reverse_iterator itM = items.rbegin();
    for (; itM != items.rend(); ++itM) {
        tableView_->removeRow(itM->first, itM->second);
    }

    enableCommit(true);
    if (actionNew_) {
        actionNew_->setEnabled(true);
    }
}

void ItemEdit::onClear()
{
    const int result = QMessageBox::question(tableView_,
                                             QStringLiteral("删除确认"),
                                             QStringLiteral("确认清空所有数据？"));
    if (result == QMessageBox::No) {
        return;
    }

    tableView_->clearContents();

    enableCommit(true);
    if (actionNew_) {
        actionNew_->setEnabled(true);
    }
}

int ItemEdit::windowType() const
{
    return wdItem;
}

bool ItemEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    bool result = false;
    if (sender() == tableView_) {
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool ItemEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    bool result = false;
    ICDCommonData *commonData = data();
    const QObject *sender = this->sender();
    if (sender == editUnit_) {
        commonData->setUnit(editUnit_->text().trimmed().toStdString());
        result = true;
    } else if (sender == comboNumericType_) {
        ICDNumericData *numericData = static_cast<ICDNumericData*>(commonData);
        if (numericData) {
            numericData->setNumericType(comboNumericType_->itemData(comboNumericType_->currentIndex()).toInt());
        }
        result = true;
    } else if (sender == spinMinimum_ || sender == spinMaximum_) {
        QString sMinimum;
        if (checkLeftInf_->isChecked()) {
            sMinimum = spinMinimum_->textFromValue(spinMinimum_->value());
            if (spinDefault_->value() < spinMinimum_->value()) {
                spinDefault_->setValue(spinMinimum_->value());
            }
        }
        QString sMaximum;
        if (checkRightInf_->isChecked()) {
            sMaximum = spinMaximum_->textFromValue(spinMaximum_->value());
        }
        commonData->setRange(QString("%1~%2").arg(sMinimum).arg(sMaximum).toStdString());
        result = true;
    } else if (sender == spinOffset_) {
        commonData->setOffset(spinOffset_->value());
        result = true;
    } else if (sender == spinScale_) {
        commonData->setScale(spinScale_->value());
        result = true;
    } else if (sender == spinDefault_) {
        commonData->setDefaultStr(QString::number(spinDefault_->value()).toStdString());
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool ItemEdit::init()
{
    // numeric type
    if (comboNumericType_) {
        comboNumericType_->clear();
        std::vector<stDictionary> names;
        QVariantList args;
        args.append(qVariantFromValue((void*)&names));
        args.append(int(GlobalDefine::dicNumericType));
        jnotify->send("edit.queryDictionaryTable", args);
        const int count = names.size();
        for (int i = 0; i < count; ++i) {
            const stDictionary &dic = names.at(i);
            comboNumericType_->addItem(dic.sDec.c_str(), dic.nCode);
        }
        ICDNumericData *numericData = static_cast<ICDNumericData*>(data());
        if (numericData) {
            const int index = comboNumericType_->findData(numericData->numericType());
            comboNumericType_->setCurrentIndex((index < 0) ? 0 : index);
        }
    }

    // offset
    spinOffset_->setValue(data()->offset());
    // scale
    spinScale_->setValue(data()->scale());
    // unit
    editUnit_->clearFocus();
    editUnit_->setText(QString::fromStdString(data()->unit()));
    // range
    const QStringList lstRange = QString(data()->range().c_str()).split("~");
    if (lstRange.count() >= 2) {
        const QString first = lstRange.first();
        const QString last = lstRange.last();
        // minimum
        spinMinimum_->setProperty("highlight", false);
        spinMinimum_->setValue(first.isEmpty() ? 0.0 : first.toDouble());
        spinMinimum_->setDisabled(first.isEmpty());
        checkLeftInf_->setChecked(!first.isEmpty());
        // maximum
        spinMaximum_->setProperty("highlight", false);
        spinMaximum_->setValue(last.isEmpty() ? 0.0 : last.toDouble());
        spinMaximum_->setDisabled(last.isEmpty());
        checkRightInf_->setChecked(!last.isEmpty());
    }
    // default value
    spinDefault_->setValue(QString(data()->defaultStr().c_str()).toDouble());
    // specs
    tableView_->clearContents();
    std::map<double, std::string> values = data()->values();
    std::map<double, std::string>::iterator it = values.begin();
    for (; it != values.end(); ++it) {
        _Eigenvalue data;
        data.value = it->first;
        data.describe = it->second.c_str();
        insertValueOne(tableView_->rowCount(), data);
    }

    return true;
}

void ItemEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    if (comboNumericType_) {
        disconnect(comboNumericType_, SIGNAL(currentTextChanged(const QString &)),
                   this, SLOT(onTextChanged(const QString &)));
    }
    disconnect(spinMinimum_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(checkLeftInf_, SIGNAL(toggled(bool)),
               this, SLOT(onRangeStateChanged(bool)));
    disconnect(spinMaximum_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(checkRightInf_, SIGNAL(toggled(bool)),
               this, SLOT(onRangeStateChanged(bool)));
    disconnect(spinDefault_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinOffset_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(editUnit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinScale_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(tableView_, SIGNAL(contentChanged()),
               this, SLOT(onEditFinished()));
    if (enabled) {
        if (comboNumericType_) {
            connect(comboNumericType_, SIGNAL(currentTextChanged(const QString &)),
                    this, SLOT(onTextChanged(const QString &)));
        }
        connect(spinMinimum_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(checkLeftInf_, SIGNAL(toggled(bool)),
                this, SLOT(onRangeStateChanged(bool)));
        connect(spinMaximum_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(checkRightInf_, SIGNAL(toggled(bool)),
                this, SLOT(onRangeStateChanged(bool)));
        connect(spinDefault_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinOffset_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(editUnit_, SIGNAL(textChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinScale_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(tableView_, SIGNAL(contentChanged()),
                this, SLOT(onEditFinished()));
    }
}

bool ItemEdit::confirm()
{
    return true;
}

bool ItemEdit::validate()
{
    // 校验上下限以及默认值
    if (checkLeftInf_->isChecked()) {
        if (checkRightInf_->isChecked()) {
            if (spinMinimum_->value() > spinMaximum_->value()) {
                spinMinimum_->setFocus();
                spinMinimum_->setProperty("highlight", true);
                setStatus(QStringLiteral("范围下限不能大于上限！"));
                return false;
            } else {
                spinMinimum_->setProperty("highlight", false);
            }
            // 校验默认值
            if (spinDefault_->value() < spinMinimum_->value()
                    || spinDefault_->value() > spinMaximum_->value()) {
                spinDefault_->setFocus();
                spinDefault_->setProperty("highlight", true);
                setStatus(QStringLiteral("默认值超出范围！"));
                return false;
            } else {
                spinDefault_->setProperty("highlight", false);
            }
        } else {
            spinMinimum_->setProperty("highlight", false);
            // 校验默认值
            if (spinDefault_->value() < spinMinimum_->value()) {
                spinDefault_->setFocus();
                spinDefault_->setProperty("highlight", true);
                setStatus(QStringLiteral("默认值超出范围！"));
                return false;
            } else {
                spinDefault_->setProperty("highlight", false);
            }
        }
    } else {    // 未启用下限
        spinMinimum_->setProperty("highlight", false);
        if (checkRightInf_->isChecked()) {    // 启用上限
            // 校验默认值
            if (spinDefault_->value() > spinMaximum_->value()) {
                spinDefault_->setFocus();
                spinDefault_->setProperty("highlight", true);
                setStatus(QStringLiteral("默认值超出范围！"));
                return false;
            } else {
                spinDefault_->setProperty("highlight", false);
            }
        }
    }
    // 特征值
    QString value;
    QString desc;
    std::map<double, std::string> values;
    const int count = tableView_->rowCount();
    for (int i = 0; i < count; ++i) {
        value = tableView_->itemValue(i, 0).toString();
        desc = tableView_->itemValue(i, 1).toString().trimmed();
        if (value.isEmpty() || desc.isEmpty()) {
            continue;
        }
        if (!data()->dataInRange(value.toDouble())) {
            setStatus(QStringLiteral("特征值超出限定范围！"));
            tableView_->setCurrentItem(tableView_->item(i, 0));
            return false;
        }
        values[value.toDouble()] = desc.toStdString();
    }
    data()->setValues(values);

    return true;
}

ICDCommonData *ItemEdit::data()
{
    return static_cast<ICDCommonData*>(ObjectEdit::data());
}

ICDCommonData *ItemEdit::oldData()
{
    return static_cast<ICDCommonData*>(ObjectEdit::oldData());
}

void ItemEdit::setupUI(int windowType)
{
    if (windowType == wdNumeric) {
        comboNumericType_ = new QComboBox(this);
        addFormRow(QStringLiteral("值类型："), comboNumericType_);
    }

    spinOffset_ = new DoubleSpinBox(this);
    spinOffset_->setRange(DBL_MIN, DBL_MAX);
    spinOffset_->setDecimals(16);
    addFormRow(QStringLiteral("偏移量："), spinOffset_);

    spinScale_ = new DoubleSpinBox(this);
    spinScale_->setRange(DBL_MIN, DBL_MAX);
    spinScale_->setDecimals(16);
    addFormRow(QStringLiteral("比例尺："), spinScale_);

    editUnit_ = new LimitLineEdit(this);
    editUnit_->setMaxLength(20);
    addFormRow(QStringLiteral("单位："), editUnit_);

    spinMinimum_ = new DoubleSpinBox(this);
    spinMinimum_->setRange(DBL_MIN, DBL_MAX);
    spinMinimum_->setDecimals(16);
    checkLeftInf_ = new QCheckBox(QStringLiteral("下限："), this);
    addFormRow(checkLeftInf_, spinMinimum_);

    spinMaximum_ = new DoubleSpinBox(this);
    spinMaximum_->setRange(DBL_MIN, DBL_MAX);
    spinMaximum_->setDecimals(16);
    checkRightInf_ = new QCheckBox(QStringLiteral("上限："), this);
    addFormRow(checkRightInf_, spinMaximum_);

    spinDefault_ = new DoubleSpinBox(this);
    spinDefault_->setRange(DBL_MIN, DBL_MAX);
    addFormRow(QStringLiteral("默认值："), spinDefault_);

    QGroupBox* groupSpecs = new QGroupBox(QStringLiteral("特征点信息"), this);
    splitterBase()->addWidget(groupSpecs);

    QVBoxLayout* layoutSpecs = new QVBoxLayout(groupSpecs);
    layoutSpecs->setContentsMargins(0, 0, 0, 0);
    layoutSpecs->setSpacing(1);

    tableView_ = new JXmlTable(this);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::MultiSelection);
    tableView_->verticalHeader()->setFixedWidth(40);
    tableView_->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableView_->horizontalHeader()->setSectionsMovable(false);
    tableView_->loadConfig(JMain::instance()->configDir().append("/old/xmltable.xml"),
                           "EigenvalueComTable");
    layoutSpecs->addWidget(tableView_);

    tableView_->setContextMenuPolicy(Qt::ActionsContextMenu);
    QList<QAction *> lstAction;
    actionNew_ = new QAction(QStringLiteral("新增"), tableView_);
    tableView_->addAction(actionNew_);
    actionNew_->setObjectName("new");
    lstAction << actionNew_;
    actionClear_ = new QAction(QStringLiteral("清空"), tableView_);
    tableView_->addAction(actionClear_);
    actionClear_->setObjectName("clear");
    lstAction << actionClear_;

    QListIterator<QAction *> it = lstAction;
    while (it.hasNext()) {
        QAction *act = it.next();
        connect(act, &QAction::triggered, [=](){
            dealAction(act);
        });
    }
    connect(tableView_, SIGNAL(itemPressed(QStandardItem *)),
            this, SLOT(onTableItemPressed(QStandardItem *)));

    enableConnect(true);
}

void ItemEdit::showMenu()
{
    QMenu menu(this);
    menu.addAction(QStringLiteral("新增"), this, SLOT(onNew()));
    menu.addAction(QStringLiteral("删除"), this, SLOT(onDelete()));
    menu.addAction(QStringLiteral("清空"), this, SLOT(onClear()));
    menu.exec(QCursor::pos());
}

void ItemEdit::dealAction(QAction *action)
{
    if (!action)
    {
        return;
    }
    const QString name = action->objectName();
    if (name == "new") {
        onNew();
    } else if (name == "clear") {
        onClear();
    }
}

void ItemEdit::insertValueOne(int index, const _Eigenvalue &data)
{
    int column = 0;
    tableView_->insertRow(index);
    tableView_->setItemValue(index, column, data.value);
    tableView_->setItemValue(index, ++column, data.describe);
}
