#include "precomp.h"
#include "headerui.h"
#include "KernelClass/icdfactory.h"
#include "limittextedit.h"
#include "limitlineedit.h"
#include "hexspinbox.h"

HeaderUI::HeaderUI(QWidget *parent)
    : MetaUI(parent)
{
    QGroupBox* group = new QGroupBox(QStringLiteral("基本信息"), this);

    q_edtName = new LimitLineEdit(this);
    q_edtName->setMaxLength(40);
    q_edtName->setToolTip(QStringLiteral("最多40个字符！"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,19}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(20);
    q_edtCode->setToolTip(QStringLiteral("最多20个字符！"));
    q_spinDefault = new hexSpinBox(this);
    q_spinDefault->setRange(0, 0xFF);
    q_edtRemak = new LimitTextEdit(this);
    q_edtRemak->setMaxLength(200);
    q_edtRemak->setToolTip(QStringLiteral("最多200个字符！"));

    QGridLayout* gridLayout = new QGridLayout(group);
    gridLayout->addWidget(new QLabel(QStringLiteral("名   称：")), 0, 0);
    gridLayout->addWidget(q_edtName, 0, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), 0, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("标   识：")), 1, 0);
    gridLayout->addWidget(q_edtCode, 1, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), 1, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("默认值：")), 2, 0);
    gridLayout->addWidget(q_spinDefault, 2, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("描   述：")), 3, 0);
    gridLayout->addWidget(q_edtRemak, 3, 1, 2, 1);
    gridLayout->setRowStretch(4, 1);

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
    QHBoxLayout* horiLayoutBtn = new QHBoxLayout();
    horiLayoutBtn->addStretch();
    horiLayoutBtn->addWidget(q_btnConfirm);
    horiLayoutBtn->addWidget(q_btnCancel);

    QVBoxLayout* veriLayoutMain = new QVBoxLayout(this);
    veriLayoutMain->setContentsMargins(0, 0, 0, 0);
    veriLayoutMain->addWidget(group);
    veriLayoutMain->addStretch();
    veriLayoutMain->addWidget(q_edtStatus);
    veriLayoutMain->addLayout(horiLayoutBtn);

    // signal-slot
    connect(q_btnConfirm, SIGNAL(clicked()), this, SLOT(slotConfirm()));
    connect(q_btnCancel, SIGNAL(clicked()), this, SLOT(slotCanceled()));
    enableConnection(true);
    // 记录原始颜色
    q_color = q_edtName->palette().color(QPalette::Base);
}

void HeaderUI::setUIData(const _UIData &data)
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

void *HeaderUI::uiData() const
{
    return (void*)&q_data;
}

// 切换数据类型
void HeaderUI::changeDataType(int type)
{
    q_data->setType(type);
    enableOptionButton(true);
}

// 原始数据类型
int HeaderUI::originalType() const
{
    if (!q_old) {
        return GlobalDefine::dtInvalid;
    }

    return q_old->type();
}

void HeaderUI::slotEditFinished()
{
    LimitTextEdit *tedt = qobject_cast<LimitTextEdit*>(sender());
    if (tedt) {
        QString content = tedt->toPlainText().trimmed();
        if (tedt == q_edtRemak) {
            q_data->setRemark(content.toStdString());
        }
        enableOptionButton(true);
    }
}

void HeaderUI::slotTextChanged(const QString& text)
{
    // 校验数据
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox *>(sender());
    if (edt) {
        QString content = text.trimmed();
        if (edt == q_edtStatus) { // 如果出现错误提示，则灰化确认按钮
            if (q_btnConfirm) {
                q_btnConfirm->setEnabled(content.isEmpty());
            }
        } else {
            if (edt == q_edtName) {
                q_data->setName(content.toStdString());
            } else if (edt == q_edtCode) {
                q_data->setProCode(content.toStdString());
            }
            enableOptionButton(true);
        }
    } else if (spin) {
        if (spin == q_spinDefault) {
            q_data->setDefaultStr(QString::number(spin->value(), 16)
                                  .toUpper().toStdString());
        }
        enableOptionButton(true);
    } else {
        enableOptionButton(true);
    }
}

// 确认
void HeaderUI::slotConfirm()
{
    // 更新序号和字节号
    QVariantList args;
    args.append(qVariantFromValue((void*)&q_data));
    jnotify->send("edit.fillBitSerial", args);

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
void HeaderUI::slotCanceled()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// 初始化界面数据
void HeaderUI::init()
{
    enableConnection(false);

    enableOptionButton(false);
    QPalette palette;
    if (q_edtName) {
        palette = q_edtName->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
        q_edtName->setText(q_data->name().c_str());
        q_edtName->clearFocus();
    }
    if (q_edtCode) {
        palette = q_edtCode->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
        q_edtCode->setText(q_data->proCode().c_str());
        q_edtCode->clearFocus();
    }
    if (q_spinDefault) {
        q_spinDefault->setValue(QString(q_data->defaultStr().c_str())
                                .toInt(0, 16));
        q_spinDefault->clearFocus();
    }
    if (q_edtRemak) {
        q_edtRemak->setText(q_data->remark().c_str());
        q_edtRemak->clearFocus();
    }
    if (q_edtStatus) {
        q_edtStatus->clear();
        q_edtStatus->clearFocus();
    }

    enableConnection(true);
}

// 启/停用信号槽
void HeaderUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinDefault, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtRemak, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    disconnect(q_edtStatus, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));

    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinDefault, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtRemak, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(q_edtStatus, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// 启/停用界面控制按钮
void HeaderUI::enableOptionButton(bool enable)
{
    if (q_btnConfirm) {
        q_btnConfirm->setEnabled(enable);
    }
    if (q_btnCancel) {
        q_btnCancel->setEnabled(enable);
    }
}

// 校验界面数据
bool HeaderUI::dataValid()
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
    args.clear();
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

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
        q_edtStatus->setText(QStringLiteral("规划数据超过预定总长度！"));

        return false;
    }

    return true;
}
