#include "precomp.h"
#include "checkui.h"
#include "limittextedit.h"
#include "limitlineedit.h"

CheckUI::CheckUI(QWidget *parent)
    : MetaUI(parent)
{
    QGroupBox* group = new QGroupBox(QStringLiteral("基本信息"), this);
    layoutMain()->insertWidget(0, group);

    q_edtName = new LimitLineEdit(this);
    q_edtName->setMaxLength(256);
    q_edtName->setToolTip(QStringLiteral("最多256个字符！"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,255}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(256);
    q_edtCode->setToolTip(QStringLiteral("最多256个字符！"));
    q_boxCheckType = new QComboBox(this);
    q_spinStart = new QSpinBox(this);
    q_spinStart->setRange(0, 1e4);
    q_spinEnd = new QSpinBox(this);
    q_spinEnd->setRange(0, 1e4);
    q_edtRemak = new LimitTextEdit(this);
    q_edtRemak->setMaxLength(256);
    q_edtRemak->setToolTip(QStringLiteral("最多256个字符！"));

    int row = 0;
    QGridLayout* gridLayout = new QGridLayout(group);
    gridLayout->setContentsMargins(6, 3, 0, 3);
    gridLayout->addWidget(new QLabel(QStringLiteral("名      称：")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("标      识：")), ++row, 0);
    gridLayout->addWidget(q_edtCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("校验类型：")), ++row, 0);
    gridLayout->addWidget(q_boxCheckType, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("起 始 位 ：")), ++row, 0);
    gridLayout->addWidget(q_spinStart, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("终 止 位 ：")), ++row, 0);
    gridLayout->addWidget(q_spinEnd, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("描      述：")), ++row, 0);
    gridLayout->addWidget(q_edtRemak, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

    // signal-slot
    enableConnection(true);

    // 记录原始颜色
    q_color = q_edtName->palette().color(QPalette::Base);
}

int CheckUI::uiType() const
{
    return wdCheck;
}

void CheckUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    ICDMetaData::smtMeta &meta =
            *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
    q_old = std::dynamic_pointer_cast<ICDCheckData>(meta);
    q_data = std::dynamic_pointer_cast<ICDCheckData>(q_old->clone());
    setProperty("option", data.type);

    enableConnection(false);
    initBoxType();
    enableConnection(true);

    init();
    // 如果是新增，则默认保存可用
    if (GlobalDefine::optNew == data.type) {
        buttonConfirm()->setEnabled(true);
    }

    show();
}

void*CheckUI::uiData() const
{
    return (void*)&q_data;
}

// 界面操作按钮可用
void CheckUI::changeDataType(int type)
{
    q_data->setType(type);
    enableOptionButton(true);
}

// 原始数据类型
int CheckUI::originalType() const
{
    if (!q_old) {
        return GlobalDefine::dtInvalid;
    }

    return q_old->type();
}

void CheckUI::slotEditFinished()
{
    LimitTextEdit *tedt = qobject_cast<LimitTextEdit*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (tedt) {
        QString content = tedt->toPlainText().trimmed();
        if (tedt == q_edtRemak) {
            q_data->setRemark(content.toStdString());
        }
        enableOptionButton(true);
    } else if (spin) {
        if (spin == q_spinStart) { // 设置结束校验位下限
            q_spinEnd->setMinimum(q_spinStart->value());
        } else if (spin == q_spinEnd) {
            q_spinStart->setMaximum(q_spinEnd->value());
        }
    }
}

void CheckUI::slotTextChanged(const QString& text)
{
    QString content = text.trimmed();
    // 校验数据
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (edt) {
        if (edt == editStatus()) { // 如果出现错误提示，则灰化确认按钮
            buttonConfirm()->setEnabled(content.isEmpty());
        } else {
            if (edt == q_edtName) {
                q_data->setName(content.toStdString());
            } else if (edt == q_edtCode) {
                q_data->setProCode(content.toStdString());
            }
            enableOptionButton(true);
        }
    } else if (box) {
        if (box == q_boxCheckType) {
            q_data->setCheckType(q_boxCheckType->itemData(q_boxCheckType->currentIndex())
                                 .toInt());
            q_spinStart->setDisabled(GlobalDefine::ctInvalid == q_data->checkType());
            q_spinEnd->setDisabled(GlobalDefine::ctInvalid == q_data->checkType());
            int bound = q_spinEnd->property("bound").toInt();
            if (bound > 0) {
                q_spinEnd->setMaximum(bound - 1 - q_data->length());
            }
        }
        enableOptionButton(true);
    } else if (spin) {
        if (spin == q_spinStart) {  // 校验起始位
            q_data->setStart(content.toInt());
        } else if (spin = q_spinEnd) {  //  校验终止位
            q_data->setEnd(content.toInt());
        }
        enableOptionButton(true);
    }
}

// 确认
void CheckUI::confirm()
{
    // 更新序号和字节号]
    QVariantList args;
    args.append(qVariantFromValue((void*)&q_data));
    jnotify->send("edit.fillBitSerial", args);

    // 校验
    if (!dataValid()) {
        return;
    }
    bool result = false;

    emit confirmed(result);
    if (result) {
        MetaUI::confirm();
    } else {
        editStatus()->setText(QStringLiteral("保存数据失败！"));
    }
}

// 取消
void CheckUI::cancel()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    MetaUI::cancel();

    emit canceled();
}

// 初始化界面数据
void CheckUI::init()
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
    if (q_boxCheckType) {   // 校验类型
        int index = q_boxCheckType->findData(q_data->checkType());
        q_boxCheckType->setCurrentIndex((index < 0) ? 0 : index);
    }
    int bound = 0;
    QVariantList args;
    args.append(qVariantFromValue((void*)&bound));
    QString command("total");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryTableInformation", args);
    if (q_spinStart) {  // 校验起始位
        if (bound > 0) {
            q_spinStart->setMaximum(bound - 1);
        }
        q_spinStart->setValue(q_data->start());
        q_data->setStart(q_spinStart->value());
    }
    if (q_spinEnd) {    // 校验终止位
        if (bound > 0) {
            // 起始字号从0开始，除去自身长度
            q_spinEnd->setMaximum(bound - 1 - q_data->length());
            q_spinEnd->setProperty("bound", bound);
        }
        q_spinEnd->setValue(q_data->end());
        q_data->setEnd(q_spinEnd->value());
    }
    if (q_edtRemak) {   // 描述
        q_edtRemak->setText(q_data->remark().c_str());
        q_edtRemak->clearFocus();
    }
    // 状态提示
    editStatus()->clear();
    editStatus()->clearFocus();

    enableConnection(true);
}

// 初始化校验类型下拉框
void CheckUI::initBoxType()
{
    if (!q_boxCheckType) {
        return;
    }
    // 清空原始数据
    q_boxCheckType->clear();
    // 查询数据
    std::vector<stDictionary> names;

    QVariantList args;
    args.append(qVariantFromValue((void*)&names));
    args.append(int(GlobalDefine::dicCheckType));
    jnotify->send("edit.queryDictionaryTable", args);

    const int count = names.size();
    for (int i = 0; i < count; ++i) {
        const stDictionary &dic = names.at(i);
        q_boxCheckType->addItem(dic.sDec.c_str(), dic.nCode);
    }
}

// 启/停用信号槽
void CheckUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_boxCheckType, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinStart, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinStart, SIGNAL(editingFinished()),
               this, SLOT(slotEditFinished()));
    disconnect(q_spinEnd, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinEnd, SIGNAL(editingFinished()),
               this, SLOT(slotEditFinished()));
    disconnect(q_edtRemak, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    disconnect(editStatus(), SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));

    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_boxCheckType, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinStart, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinStart, SIGNAL(editingFinished()),
                this, SLOT(slotEditFinished()));
        connect(q_spinEnd, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinEnd, SIGNAL(editingFinished()),
                this, SLOT(slotEditFinished()));
        connect(q_edtRemak, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(editStatus(), SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// 校验界面数据
bool CheckUI::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;
    QVariantList args;
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);
    // 名称
    QPalette palette = q_edtName->palette();
    if (q_data->name().empty()) {
        editStatus()->setText(QStringLiteral("名称不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();
        return false;
    } else if (existed.contains(q_data->name().c_str())) {
        editStatus()->setText(QStringLiteral("已存在同名项！"));
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
    args.clear();
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    palette = q_edtCode->palette();
    if (q_data->proCode().empty()) {
        editStatus()->setText(QStringLiteral("标识不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();
        return false;
    } else if (existed.contains(q_data->proCode().c_str())) {
        editStatus()->setText(QStringLiteral("已存在同名标识！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();
        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
    }
    int lengthCheck = 0;

    args.clear();
    args.append(qVariantFromValue((void*)&lengthCheck));
    QString command("lengthCheck");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryTableInformation", args);
    if (0 == lengthCheck) {
        return true;    // 不进行长度校验
    }
    // 长度
    int remains = 0;
    int offset = 0;

    args.clear();
    args.append(qVariantFromValue((void*)&remains));
    command = "remains";
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryTableInformation", args);

    // 长度偏移量
    args.clear();
    args.append(qVariantFromValue((void*)&q_data));
    args.append(qVariantFromValue((void*)&offset));
    jnotify->send("edit.queryLengthOffset", args);
    if (remains < offset || remains < 0) {
        editStatus()->setText(QStringLiteral("规划数据超过预定总长度！"));

        return false;
    }

    return true;
}
