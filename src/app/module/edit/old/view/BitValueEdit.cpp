#include "precomp.h"
#include "BitValueEdit.h"
#include "DoubleSpinBox.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"
#include "jwt/jxmltable.h"
#include "jwt/jsplitter.h"

BitValueEdit::BitValueEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    spinBitStart_ = new QSpinBox(this);
    spinBitStart_->setRange(0, MAX_BIT_COUNT - 1);
    addFormRow(QStringLiteral("<font color=red>*</font>起始位："), spinBitStart_);

    spinBitCount_ = new QSpinBox(this);
    spinBitCount_->setRange(1, MAX_BIT_COUNT);
    addFormRow(QStringLiteral("<font color=red>*</font>长度："), spinBitCount_);

    spinOffset_ = new DoubleSpinBox(this);
    spinOffset_->setRange(DBL_MIN, DBL_MAX);
    addFormRow(QStringLiteral("偏移量："), spinOffset_);

    spinScale_ = new DoubleSpinBox(this);
    spinScale_->setRange(DBL_MIN, DBL_MAX);
    spinScale_->setDecimals(16);
    addFormRow(QStringLiteral("比例："), spinScale_);

    editUnit_ = new LimitLineEdit(this);
    addFormRow(QStringLiteral("单位："), editUnit_);

    spinMinimum_ = new DoubleSpinBox(this);
    spinMinimum_->setRange(DBL_MIN, DBL_MAX);
    checkLeftInf_ = new QCheckBox(QStringLiteral("下限："), this);
    addFormRow(checkLeftInf_, spinMinimum_);

    spinMaximum_ = new DoubleSpinBox(this);
    spinMaximum_->setRange(DBL_MIN, DBL_MAX);
    checkRightInf_ = new QCheckBox(QStringLiteral("上限："), this);
    addFormRow(checkRightInf_, spinMaximum_);

    spinDefault_ = new DoubleSpinBox(this);
    spinDefault_->setRange(DBL_MIN, DBL_MAX);
    spinDefault_->setDecimals(16);
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
    actionClear = new QAction(QStringLiteral("清空"), tableView_);
    tableView_->addAction(actionClear);
    actionClear->setObjectName("clear");
    lstAction << actionClear;

    QListIterator<QAction *> iter = lstAction;
    while (iter.hasNext()) {
        QAction *action = iter.next();
        connect(action, &QAction::triggered, [=](){ dealAction(action); });
    }
    connect(tableView_, SIGNAL(itemPressed(QStandardItem*)),
            this, SLOT(onTableItemPressed(QStandardItem*)));

    enableConnect(true);
}

void BitValueEdit::onRangeStateChanged(bool enabled)
{
    QStringList lstRange = QString(data()->dataRange().c_str()).split("~");
    if (std::string::npos == data()->dataRange().find("~")) {
        lstRange.push_back(QString());
    }

    const QObject *sender = this->sender();
    if (sender == checkLeftInf_) {
        checkLeftInf_->setEnabled(enabled);
        if (enabled) {
            lstRange[0] = spinMinimum_->text();
            data()->setRange(lstRange.join("~").toStdString());
        } else {
            lstRange[0].clear();
            data()->setRange(lstRange.join("~").toStdString());
        }
    } else if (sender == checkRightInf_) {
        spinMaximum_->setEnabled(enabled);
        if (enabled) {
            lstRange[1] = spinMaximum_->text();
            data()->setRange(lstRange.join("~").toStdString());
        } else {
            lstRange[1].clear();
            data()->setRange(lstRange.join("~").toStdString());
        }
    }

    enableCommit(true);
}

void BitValueEdit::onTableItemPressed(QStandardItem *item)
{
    Q_UNUSED(item);
    if (Qt::RightButton == QApplication::mouseButtons()) {
        showMenu();
    }
}

void BitValueEdit::onNew()
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

void BitValueEdit::onDelete()
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

void BitValueEdit::onClear()
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

int BitValueEdit::windowType() const
{
    return wdBitValue;
}

bool BitValueEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    bool result = false;
    const QObject *sender = this->sender();
    if (sender == spinBitStart_) {
        spinBitCount_->setMaximum(MAX_BIT_COUNT - spinBitStart_->value());
        result = true;
    } else if (sender == spinBitCount_) {
        result = true;
    } else if (sender == tableView_) {
        enableCommit(true);
        return true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool BitValueEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    bool result = true;
    ICDBitData *bitData = data();
    const QObject *sender = this->sender();
    if (sender == editUnit_) {
        bitData->setUnit(text.trimmed().toStdString());
    } else if (sender == spinBitStart_) {
        bitData->setStart(spinBitStart_->value());
        bitData->setBitLength(spinBitCount_->value());
    } else if (sender == spinBitCount_) {
        bitData->setBitLength(spinBitCount_->value());
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
        bitData->setRange(QString("%1~%2").arg(sMinimum).arg(sMaximum).toStdString());
    } else if (sender == spinDefault_) {
        bitData->setDefaultStr(QString::number(spinDefault_->value()).toStdString());
    } else if (sender == spinScale_) {
        bitData->setScale(spinScale_->value());
    } else if (sender == spinOffset_) {
        bitData->setOffset(spinOffset_->value());
    } else {
        result = false;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool BitValueEdit::init()
{
    spinBitStart_->setProperty("highlight", false);
    spinBitStart_->setValue(data()->start());

    spinBitCount_->setProperty("highlight", false);
    spinBitCount_->setValue(data()->length());

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

void BitValueEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(spinBitStart_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinBitStart_, SIGNAL(editingFinished()),
               this, SLOT(onEditFinished()));
    disconnect(spinBitCount_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinBitCount_, SIGNAL(editingFinished()),
               this, SLOT(onEditFinished()));
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
        connect(spinBitStart_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinBitStart_, SIGNAL(editingFinished()),
                this, SLOT(onEditFinished()));
        connect(spinBitCount_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinBitCount_, SIGNAL(editingFinished()),
                this, SLOT(onEditFinished()));
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

bool BitValueEdit::validate()
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

ICDBitData *BitValueEdit::data()
{
    return static_cast<ICDBitData*>(ObjectEdit::data());
}

ICDBitData *BitValueEdit::oldData()
{
    return static_cast<ICDBitData*>(ObjectEdit::oldData());
}

void BitValueEdit::showMenu()
{
    QMenu menu(this);
    menu.addAction(QStringLiteral("新增"), this, SLOT(onNew()));
    menu.addAction(QStringLiteral("删除"), this, SLOT(onDelete()));
    menu.addAction(QStringLiteral("清空"), this, SLOT(onClear()));
    menu.exec(QCursor::pos());
}

void BitValueEdit::dealAction(QAction *action)
{
    if (!action) {
        return;
    }

    const QString name = action->objectName();
    if (name == "new") {
        onNew();
    } else if (name == "clear") {
        onClear();
    }
}

void BitValueEdit::insertValueOne(int index, const _Eigenvalue &data)
{
    int column = 0;
    tableView_->insertRow(index);
    tableView_->setItemValue(index, column, data.value);
    tableView_->setItemValue(index, ++column, data.describe);
}
