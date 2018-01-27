#include "commonui.h"
#include "limittextedit.h"
#include "limitlineedit.h"
#include "customdoublespinbox.h"
#include "jxmltable.h"
#include "jsplitter.h"

// 由于数据存储规则字段限制，所以界面增加特征值个数限制
#ifndef TABLE_MAX_COUNT
#define TABLE_MAX_COUNT 40
#endif

CommonUI::CommonUI(IJAttempter &attempter, QWidget *parent)
    :q_attempter(attempter)
    , MetaUI(parent)
{
    QGroupBox *basicGroup = new QGroupBox(QStringLiteral("基本信息"), this);
    // 基本属性
    q_edtName = new LimitLineEdit(this);
    q_edtName->setMaxLength(40);
    q_edtName->setToolTip(QStringLiteral("最多40个字符！"));
    QLabel *labelNameFlag = new QLabel("<font color=red>*</font>");
    labelNameFlag->setFixedWidth(20);
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,19}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(20);
    q_edtCode->setToolTip(QStringLiteral("最多20个字符！"));
    q_spinMin = new CustomDoubleSpinbox(this);
    q_spinMin->setRange(-pow(10.0, 10), pow(10.0, 10));
    q_spinMax = new CustomDoubleSpinbox(this);
    q_spinMax->setRange(-pow(10.0, 10), pow(10.0, 10));
    q_checkLower = new QCheckBox(this);
    q_checkUpper = new QCheckBox(this);
    q_spinOffset = new CustomDoubleSpinbox(this);
    q_spinOffset->setRange(-pow(10.0, 10), pow(10.0, 10));
    q_edtUnit = new LimitLineEdit(this);
    q_edtUnit->setMaxLength(20);
    q_spinLSB = new CustomDoubleSpinbox(this);
    q_spinLSB->setRange(1e-16, 1e16);
    q_spinLSB->setDecimals(16);
    q_spinDefault = new CustomDoubleSpinbox(this);
    q_spinDefault->setRange(-pow(10.0, 10), pow(10.0, 10));
    q_spinDefault->setDecimals(16);
    q_edtRemak = new LimitTextEdit(this);
    q_edtRemak->setMaxLength(200);
    q_edtRemak->setToolTip(QStringLiteral("最多200个字符！"));

    int row = 0;
    QGridLayout* gridLayout = new QGridLayout(basicGroup);
    gridLayout->addWidget(new QLabel(QStringLiteral("名   称：")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(labelNameFlag, row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("标   识：")), ++row, 0);
    gridLayout->addWidget(q_edtCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("下   限：")), ++row, 0);
    gridLayout->addWidget(q_spinMin, row, 1);
    gridLayout->addWidget(q_checkLower, row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("上   限：")), ++row, 0);
    gridLayout->addWidget(q_spinMax, row, 1);
    gridLayout->addWidget(q_checkUpper, row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("偏   置：")), ++row, 0);
    gridLayout->addWidget(q_spinOffset, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("单   位：")), ++row, 0);
    gridLayout->addWidget(q_edtUnit, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("比例尺：")), ++row, 0);
    gridLayout->addWidget(q_spinLSB, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("默认值：")), ++row, 0);
    gridLayout->addWidget(q_spinDefault, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("描   述：")), ++row, 0);
    gridLayout->addWidget(q_edtRemak, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

    // 
    q_table = new JXmlTable(this);
    q_table->loadConfig(QString::fromStdString(jframeFacade()
        ->thisDirPath()).append("/config/editor/icd_infrastructure.xml"),
        "EigenvalueComTable");
    q_table->setContextMenuPolicy(Qt::ActionsContextMenu);
    QList<QAction *> lstAction;
    q_actionNew = new QAction(QStringLiteral("新增"), q_table);
    q_table->addAction(q_actionNew);
    q_actionNew->setObjectName("new");
    lstAction << q_actionNew;
    q_actionClear = new QAction(QStringLiteral("清空"), q_table);
    q_table->addAction(q_actionClear);
    q_actionClear->setObjectName("clear");
    lstAction << q_actionClear;
    q_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    q_table->setSelectionMode(QAbstractItemView::MultiSelection);
    q_table->verticalHeader()->setFixedWidth(40);
    q_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    q_table->horizontalHeader()->setSectionsMovable(false);
    QGroupBox *tableGroup = new QGroupBox(QStringLiteral("特征点信息"), this);
    QVBoxLayout *veriLayoutTable = new QVBoxLayout(tableGroup);
    veriLayoutTable->setContentsMargins(2, 2, 2, 2);
    veriLayoutTable->addWidget(q_table);

    JSplitter* splitter = new JSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setScales(QList<double>() << 2.5 << 1.5);
    splitter->setHandleColor(QColor(32, 32, 32, 10));
    splitter->setHandleWidth(4);
    splitter->addWidget(basicGroup);
    splitter->addWidget(tableGroup);

    q_edtStatus = new QLineEdit(this);
    q_edtStatus->setMaximumHeight(30);
    q_edtStatus->setEnabled(false);
    q_edtStatus->setFrame(false);
    QPalette palette = q_edtStatus->palette();
    palette.setColor(QPalette::Text, Qt::red);
    palette.setColor(QPalette::Base, Qt::transparent);
    q_edtStatus->setPalette(palette);

    q_btnConfirm = new QPushButton(QIcon(":/icd/editor/images/apply-32.png"),
                                   QStringLiteral("确认"), this);
    q_btnConfirm->setObjectName("btnConfirm");
    q_btnConfirm->setMaximumSize(65, 35);
    q_btnConfirm->setIconSize(QSize(24, 24));
    q_btnConfirm->setFlat(true);
    q_btnCancel = new QPushButton(QIcon(":/icd/editor/images/cancel-32.png"),
                                  QStringLiteral("取消"), this);
    q_btnCancel->setObjectName("btnCancel");
    q_btnCancel->setMaximumSize(65, 35);
    q_btnCancel->setIconSize(QSize(24, 24));
    q_btnCancel->setFlat(true);
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(q_btnConfirm);
    btnLayout->addWidget(q_btnCancel);

    QVBoxLayout* veriLayoutMain = new QVBoxLayout(this);
    veriLayoutMain->setContentsMargins(0, 0, 0, 0);
    veriLayoutMain->addWidget(splitter);
    veriLayoutMain->addWidget(q_edtStatus);
    veriLayoutMain->addLayout(btnLayout);

    // signal-slot
    QListIterator<QAction *> it = lstAction;
    while (it.hasNext()) {
        QAction *act = it.next();
        connect(act, &QAction::triggered, [=](){
            dealAction(act);
        });
    }
    connect(q_table, SIGNAL(itemPressed(QStandardItem *)),
            this, SLOT(slotTableItemPressed(QStandardItem *)));
    connect(q_btnConfirm, SIGNAL(clicked()), this, SLOT(slotConfirm()));
    connect(q_btnCancel, SIGNAL(clicked()), this, SLOT(slotCanceled()));
    enableConnection(true);

    // 记录原始颜色
    q_color = q_edtName->palette().color(QPalette::Base);
    //setExtraAttributeVisable(false);
}

void CommonUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    ICDMetaData::smtMeta &meta =
        *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
    q_old = std::dynamic_pointer_cast<ICDCommonData>(meta);
    q_data = std::dynamic_pointer_cast<ICDCommonData>(q_old->clone());
    setProperty("option", data.type);

    init();
    // 如果是新增，则默认保存可用
    if (GlobalDefine::optNew == data.type) {
        if (q_btnConfirm) {
            q_btnConfirm->setEnabled(true);
        }
    }

    show();
}

void* CommonUI::uiData() const
{
    return (void*)&q_data;
}

// 界面操作按钮可用
void CommonUI::changeDataType(int type)
{
    q_data->setType(type);
    enableOptionButton(true);
}

// 原始数据类型
int CommonUI::originalType() const
{
    if (!q_old) {
        return GlobalDefine::dtInvalid;
    }

    return q_old->type();
}

// 数据录入完成
void CommonUI::slotEditFinished()
{
    // 校验数据
    LimitTextEdit *tedt = qobject_cast<LimitTextEdit*>(sender());
    if (tedt) {
        QString content = tedt->toPlainText().trimmed();
        if (tedt == q_edtRemak) {   // 备注
            q_data->setRemark(content.toStdString());
        }
    }
    enableOptionButton(true);
}

// 编辑文本变更
void CommonUI::slotTextChanged(const QString& text)
{
    QString content = text.trimmed();
    // 校验数据
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    CustomDoubleSpinbox *dbSpin
        = qobject_cast<CustomDoubleSpinbox*>(sender());
    if (edt) {
        if (edt == q_edtStatus) { // 如果出现错误提示，则灰化确认按钮
            if (q_btnConfirm) {
                q_btnConfirm->setEnabled(content.isEmpty());
            }
        } else {
            if (edt == q_edtName) { // 名称
                q_data->setName(content.toStdString());
            } else if (edt == q_edtCode) {  // 编码
                q_data->setProCode(content.toStdString());
            } else if (edt == q_edtUnit) {  // 单位
                q_data->setUnit(content.toStdString());
            }
            enableOptionButton(true);
        }
    } else if (dbSpin) {
        if (dbSpin == q_spinMin || dbSpin == q_spinMax) {   // 范围
            QString min;
            if (q_checkLower->isChecked()) {
                min = q_spinMin->textFromValue(q_spinMin->value());
                if (q_spinDefault->value() < q_spinMin->value()) {
                    q_spinDefault->setValue(q_spinMin->value());
                }
            }
            QString max;
            if (q_checkUpper->isChecked()) {
                max = q_spinMax->textFromValue(q_spinMax->value());
            }
            q_data->setRange(QString("%1~%2")
                             .arg(min).arg(max).toStdString());
        } else if (dbSpin == q_spinDefault) {   // 默认值
            q_data->setDefaultStr(QString::number(dbSpin->value())
                                  .toStdString());
        } else if (dbSpin == q_spinLSB) {   // 比例尺
            q_data->setScale(dbSpin->value());
        } else if (dbSpin == q_spinOffset) {    // 偏置
            q_data->setOffset(dbSpin->value());
        }
        enableOptionButton(true);
    }
}

// 范围限制状态变更
void CommonUI::slotRangeStateChanged(bool enable)
{
    QCheckBox *check = qobject_cast<QCheckBox *>(sender());
    QStringList lstRange = QString(q_data->range().c_str()).split("~");
    if (std::string::npos == q_data->range().find("~")) {
        lstRange.push_back(QString());
    }
    if (check == q_checkLower) {
        q_spinMin->setEnabled(enable);
        if (enable) {
            lstRange[0] = q_spinMin->text();
            q_data->setRange(lstRange.join("~").toStdString());
        } else {
            lstRange[0].clear();
            q_data->setRange(lstRange.join("~").toStdString());
        }
    } else if (check == q_checkUpper) {
        q_spinMax->setEnabled(enable);
        if (enable) {
            lstRange[1] = q_spinMin->text();
            q_data->setRange(lstRange.join("~").toStdString());
        } else {
            lstRange[1].clear();
            q_data->setRange(lstRange.join("~").toStdString());
        }
    }
    enableOptionButton(true);
}

// 表点击
void CommonUI::slotTableItemPressed(QStandardItem *item)
{
    Q_UNUSED(item);
    if (Qt::RightButton == QApplication::mouseButtons()) {
        showMenu();
    }
}

// 确认
void CommonUI::slotConfirm()
{
    // 更新序号和字节号
    q_attempter.notifier().sendMessage("fillBitSerial@ICDMainView",
                                       (JWPARAM)&q_data);
    // 校验
    if (!dataValid()) {
        return;
    }
    bool result = false;

    emit confirm(result);
    if (result) {
//         _UIData data;
// 
//         data.data = &q_data;
//         setUIData(data);
    } else {
        q_edtStatus->setText(QStringLiteral("保存数据失败！"));
        q_btnConfirm->setEnabled(true);
    }
}

// 取消
void CommonUI::slotCanceled()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// 新增
void CommonUI::slotNew()
{
    _Eigenvalue data;
    int index = q_table->rowCount();
    if (index > 0) {
        data.value = q_table->itemValue(index - 1, 0).toDouble() + 1.0;
    }
    insertValueOne(index, data);
    q_table->clearSelection();
    q_table->setCurrentCell(index, 1);
    enableOptionButton(true);
    // 超过数据库字段限制
    if (q_table->rowCount() > TABLE_MAX_COUNT) {
        q_actionNew->setEnabled(false);
    }
}

// 删除
void CommonUI::slotDelete()
{
    if (QMessageBox::No == QMessageBox::question(q_table,
        QStringLiteral("删除确认"),
        QStringLiteral("确认删除当前选中数据？"))) {
        return;
    }
    std::map<int, int> items;
    QList<JTableViewSelectionRange> lst = q_table->selectRanges();
    QListIterator<JTableViewSelectionRange> it = lst;
    while (it.hasNext()) {
        const JTableViewSelectionRange &range = it.next();
        items[range.topRow()] = qMax(items[range.topRow()], range.rowCount());
    }
    std::map<int, int>::reverse_iterator itM = items.rbegin();
    for (; itM != items.rend(); ++itM) {
        q_table->removeRow(itM->first, itM->second);
    }
    enableOptionButton(true);
    if (q_actionNew) {
        q_actionNew->setEnabled(true);
    }
}

// 清除
void CommonUI::slotClear()
{
    if (QMessageBox::No == QMessageBox::question(q_table,
        QStringLiteral("删除确认"),
        QStringLiteral("确认清空所有数据？"))) {
        return;
    }
    q_table->clearContents();

    enableOptionButton(true);
    if (q_actionNew) {
        q_actionNew->setEnabled(true);
    }
}

// 初始化界面数据
void CommonUI::init()
{
    enableConnection(false);

    enableOptionButton(false);
    QPalette palette;
    if (q_edtName) {    // 名称
        palette = q_edtName->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
        q_edtName->setText(q_data->name().c_str());
        q_edtName->clearFocus();
    }
    if (q_edtCode) {    // 编码
        palette = q_edtCode->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
        q_edtCode->setText(q_data->proCode().c_str());
        q_edtCode->clearFocus();
    }
    QStringList lstRange = QString(q_data->range().c_str()).split("~");
    if (q_spinMin) {    // 范围下限
        QString value = lstRange.first();
        palette = q_spinMin->palette();
        palette.setColor(QPalette::Base, q_color);
        q_spinMin->setPalette(palette);
        q_spinMin->setValue(value.isEmpty() ? 0.0 : value.toDouble());
        q_spinMin->setDisabled(value.isEmpty());
        q_checkLower->setChecked(!value.isEmpty());
    }
    if (q_spinMax) {    // 范围上限
        QString value = lstRange.last();
        palette = q_spinMax->palette();
        palette.setColor(QPalette::Base, q_color);
        q_spinMax->setPalette(palette);
        q_spinMax->setValue(value.isEmpty() ? 0.0 : value.toDouble());
        q_spinMax->setDisabled(value.isEmpty());
        q_checkUpper->setChecked(!value.isEmpty());
    }
    if (q_edtRemak) {   // 备注
        q_edtRemak->setText(q_data->remark().c_str());
        q_edtRemak->clearFocus();
    }
    if (q_spinDefault) {    // 默认值
        palette = q_spinDefault->palette();
        palette.setColor(QPalette::Base, q_color);
        q_spinDefault->setPalette(palette);
        q_spinDefault->setValue(QString(q_data->defaultStr().c_str()).toDouble());
    }
    if (q_spinOffset) {    // 偏置
        q_spinOffset
            ->setValue(q_data->offset());
    }
    if (q_edtUnit) {    // 单位
        q_edtUnit->setText(q_data->unit().c_str());
        q_edtUnit->clearFocus();
    }
    if (q_spinLSB) {    // 比例尺
        q_spinLSB->setValue(q_data->scale());
    }
    // 特征值表
    if (q_table) {
        q_table->clearContents();
        std::map<double, std::string> values = q_data->values();
        std::map<double, std::string>::iterator it = values.begin();
        for (; it != values.end(); ++it) {
            _Eigenvalue data;

            data.value = it->first;
            data.describe = it->second.c_str();
            insertValueOne(q_table->rowCount(), data);
        }
    }
    if (q_edtStatus) {
        q_edtStatus->clear();
        q_edtStatus->clearFocus();
    }

    enableConnection(true);
}

// 启/停用信号槽
void CommonUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinMin, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_checkLower, SIGNAL(toggled(bool)),
               this, SLOT(slotRangeStateChanged(bool)));
    disconnect(q_spinMax, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_checkUpper, SIGNAL(toggled(bool)),
               this, SLOT(slotRangeStateChanged(bool)));
    disconnect(q_edtRemak, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    disconnect(q_spinDefault, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinOffset, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtUnit, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinLSB, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtStatus, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_table, SIGNAL(contentChanged()),
               this, SLOT(slotEditFinished()));
    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinMin, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_checkLower, SIGNAL(toggled(bool)),
                this, SLOT(slotRangeStateChanged(bool)));
        connect(q_spinMax, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_checkUpper, SIGNAL(toggled(bool)),
                this, SLOT(slotRangeStateChanged(bool)));
        connect(q_edtRemak, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(q_spinDefault, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinOffset, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtUnit, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinLSB, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtStatus, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_table, SIGNAL(contentChanged()),
                this, SLOT(slotEditFinished()));
    }
}

// 启/停用界面控制按钮
void CommonUI::enableOptionButton(bool enable)
{
    if (q_btnConfirm) {
        q_btnConfirm->setEnabled(enable);
    }
    if (q_btnCancel) {
        q_btnCancel->setEnabled(enable);
    }
}

// 校验界面数据
bool CommonUI::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;
    q_attempter.notifier().sendMessage("queryExistedData@ICDMainView",
                                       (JWPARAM)&existed,
                                       (JLPARAM)&section);
    // 名称
    QPalette palette = q_edtName->palette();
    if (q_data->name().empty()) {
        q_edtStatus->setText(QStringLiteral("名称不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data->name().c_str())) {
        q_edtStatus->setText(QStringLiteral("已存在同名项！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
    }
    // 编码
    section = "code";
    q_attempter.notifier().sendMessage("queryExistedData@ICDMainView",
                                       (JWPARAM)&existed,
                                       (JLPARAM)&section);
    palette = q_edtCode->palette();
    if (q_data->proCode().empty()) {
        q_edtStatus->setText(QStringLiteral("标识不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data->proCode().c_str())) {
        q_edtStatus->setText(QStringLiteral("已存在同名标识！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
    }
    // 校验上下限以及默认值
    palette = q_spinMin->palette();
    if (q_checkLower->isChecked()) {    // 启用下限
        if (q_checkUpper->isChecked()) {    // 启用上限
            if (q_spinMin->value() > q_spinMax->value()) {
                q_edtStatus->setText(QStringLiteral("范围下限不能大于上限！"));
                palette.setColor(QPalette::Base, Qt::red);
                q_spinMin->setPalette(palette);
                q_spinMin->setFocus();

                return false;
            } else {
                palette.setColor(QPalette::Base, q_color);
                q_spinMin->setPalette(palette);
            }
            // 校验默认值
            palette = q_spinDefault->palette();
            if (q_spinDefault->value() < q_spinMin->value()
                || q_spinDefault->value() > q_spinMax->value()) {
                q_edtStatus->setText(QStringLiteral("默认值超出范围！"));
                palette.setColor(QPalette::Base, Qt::red);
                q_spinDefault->setPalette(palette);
                q_spinDefault->setFocus();

                return false;
            } else {
                palette.setColor(QPalette::Base, q_color);
                q_spinDefault->setPalette(palette);
            }
        } else {    // 未启用上限
            palette.setColor(QPalette::Base, q_color);
            q_spinMin->setPalette(palette);
            // 校验默认值
            palette = q_spinDefault->palette();
            if (q_spinDefault->value() < q_spinMin->value()) {
                q_edtStatus->setText(QStringLiteral("默认值超出范围！"));
                palette.setColor(QPalette::Base, Qt::red);
                q_spinDefault->setPalette(palette);
                q_spinDefault->setFocus();

                return false;
            } else {
                palette.setColor(QPalette::Base, q_color);
                q_spinDefault->setPalette(palette);
            }
        }
    } else {    // 未启用下限
        palette.setColor(QPalette::Base, q_color);
        q_spinMin->setPalette(palette);
        if (q_checkUpper->isChecked()) {    // 启用上限
            // 校验默认值
            palette = q_spinDefault->palette();
            if (q_spinDefault->value() > q_spinMax->value()) {
                q_edtStatus->setText(QStringLiteral("默认值超出范围！"));
                palette.setColor(QPalette::Base, Qt::red);
                q_spinDefault->setPalette(palette);
                q_spinDefault->setFocus();

                return false;
            } else {
                palette.setColor(QPalette::Base, q_color);
                q_spinDefault->setPalette(palette);
            }
        }
    }
    int lengthCheck = 0;
    q_attempter.notifier().sendMessage("queryTableInformation@ICDMainView",
                                       (JWPARAM)&lengthCheck,
                                       (JLPARAM)&QString("lengthCheck"));
    if (0 != lengthCheck) {
        // 进行长度校验
        int remains = 0;
        int offset = 0;
        q_attempter.notifier().sendMessage("queryTableInformation@ICDMainView",
                                           (JWPARAM)&remains,
                                           (JLPARAM)&QString("remains"));
        // 长度偏移量
        q_attempter.notifier().sendMessage("queryLengthOffset@ICDMainView",
                                           (JWPARAM)&q_data,
                                           (JLPARAM)&offset);
        if (remains < offset || remains < 0) {
            q_edtStatus->setText(QStringLiteral("规划数据超过预定总长度！"));

            return false;
        }
    }
    // 特征值
    QString value;
    QString desc;
    std::map<double, std::string> values;
    const int count = q_table->rowCount();
    for (int i = 0; i < count; ++i) {
        value = q_table->itemValue(i, 0).toString();
        desc = q_table->itemValue(i, 1).toString().trimmed();
        if (value.isEmpty() || desc.isEmpty()) {
            continue;
        }
        if (!q_data->dataInRange(value.toDouble())) {
            q_edtStatus->setText(QStringLiteral("特征值超出限定范围！"));
            q_table->setCurrentItem(q_table->item(i, 0));

            return false;
        }
        values[value.toDouble()] = desc.toStdString();
    }
    q_data->setValues(values);

    return true;
}

// 显示特征表右键菜单
void CommonUI::showMenu()
{
    QMenu menu(this);

    QAction *act = menu.addAction(QStringLiteral("新增"),
                                  this, SLOT(slotNew()));
    // 超过数据库字段限制
    if (q_table->rowCount() >= TABLE_MAX_COUNT) {
        act->setEnabled(false);
    }
    menu.addAction(QStringLiteral("删除"), this, SLOT(slotDelete()));
    menu.addAction(QStringLiteral("清空"), this, SLOT(slotClear()));
    menu.exec(QCursor::pos());
}

// 特征表菜单处理
void CommonUI::dealAction(QAction *action)
{
    if (!action)
    {
        return;
    }
    const QString name = action->objectName();
    if (name == "new") {
        slotNew();
    } else if (name == "clear") {
        slotClear();
    }
}

// 向特征值表插入一条数据
void CommonUI::insertValueOne(int index, const _Eigenvalue &data)
{
    int column = 0;

    q_table->insertRow(index);
    q_table->setItemValue(index, column, data.value);
    q_table->setItemValue(index, ++column, data.describe);
}
