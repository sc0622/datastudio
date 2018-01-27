#include "tableui.h"
#include "globalstruct.h"
#include "limittextedit.h"
#include "limitlineedit.h"

void TableUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    q_data = *reinterpret_cast<stICDBase*>(data.data);
    q_old = q_data;
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

void* TableUI::uiData() const
{
    return (void*)&q_data;
}

TableUI::TableUI(IJAttempter &attempter, QWidget* parent)
    : q_attempter(attempter)
    , MetaUI(parent)
{
    QGroupBox *group = new QGroupBox(QStringLiteral("基本信息"));

    q_edtName = new LimitLineEdit(this);
    q_edtName->setObjectName("edtName");
    q_edtName->setMaxLength(120);
    q_edtName->setToolTip(QStringLiteral("最多120个字符！"));
    q_edtCode = new QLineEdit(this);
    q_edtCode->setObjectName("edtCode");
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,29}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(30);
    q_edtCode->setToolTip(QStringLiteral("最多30个字符！"));
    q_spinLength = new QSpinBox(this);
    q_spinLength->setObjectName("edtLength");
    q_spinLength->setRange(0, 1e6);
    q_spinLength->setSuffix(QStringLiteral("   字节"));
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setObjectName("edtDescribe");
    q_edtDescribe->setMaxLength(200);
    q_edtDescribe->setToolTip(QStringLiteral("最多200个字符！"));
    QGridLayout *gridLayout = new QGridLayout(group);
    gridLayout->addWidget(new QLabel(QStringLiteral("名称：")), 0, 0);
    gridLayout->addWidget(q_edtName, 0, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), 0, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("标识：")), 1, 0);
    gridLayout->addWidget(q_edtCode, 1, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), 1, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("长度：")), 2, 0);
    gridLayout->addWidget(q_spinLength, 2, 1);
    gridLayout->addWidget(q_check = new QCheckBox(this), 2, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("描述：")), 3, 0);
    gridLayout->addWidget(q_edtDescribe, 3, 1, 2, 1);
    gridLayout->setRowStretch(4, 1);

    q_edtStatus = new QLineEdit(this);
    q_edtStatus->setObjectName("edtStatus");
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

    QVBoxLayout *veriLayoutMain = new QVBoxLayout(this);
    veriLayoutMain->setContentsMargins(0, 0, 0, 0);
    veriLayoutMain->addWidget(group);
    veriLayoutMain->addStretch();
    veriLayoutMain->addWidget(q_edtStatus);
    veriLayoutMain->addLayout(btnLayout);

    // signal-slot
    connect(q_btnConfirm, SIGNAL(clicked()), this, SLOT(slotConfirm()));
    connect(q_btnCancel, SIGNAL(clicked()), this, SLOT(slotCanceled()));
    enableConnection(true);

    // 记录原始颜色
    q_color = q_edtName->palette().color(QPalette::Base);
}

void TableUI::slotTextChanged(const QString &text)
{
    QString content = text.trimmed();
    // 校验数据
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (edt) {
        if (edt == q_edtStatus) { // 如果出现错误提示，则灰化确认按钮
            if (q_btnConfirm) {
                q_btnConfirm->setEnabled(content.isEmpty());
            }
        } else if (edt == q_edtName) {
            q_data.sDescribe = content.toStdString();
        } else if (edt == q_edtCode) {
            q_data.sCode = content.toStdString();
        }
        enableOptionButton(true);
    } else if (spin) {
        if (spin == q_spinLength) {
            q_data.nLength = spin->value();
        }
        enableOptionButton(true);
    }
}

// 数据录入完成
void TableUI::slotEditFinished()
{
    LimitTextEdit *edt = qobject_cast<LimitTextEdit*>(sender());
    if (edt) {
        QString content = edt->toPlainText().trimmed();
        if (edt == q_edtDescribe) {
            q_data.sRemark = content.toStdString();
        }
        enableOptionButton(true);
    }
}

void TableUI::slotCheckStateChanged(bool checked)
{
    if (q_spinLength) {
        q_spinLength->setEnabled(checked);
    }
    q_data.check = checked;

    enableOptionButton(true);
}

// 确认
void TableUI::slotConfirm()
{
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
void TableUI::slotCanceled()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// 初始化界面数据
void TableUI::init()
{
    enableConnection(false);

    enableOptionButton(false);
    QPalette palette;
    if (q_edtName) {
        palette = q_edtName->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
        q_edtName->setText(q_data.sDescribe.c_str());
        q_edtName->clearFocus();
    }
    if (q_edtCode) {
        palette = q_edtCode->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
        q_edtCode->setText(q_data.sCode.c_str());
        q_edtCode->clearFocus();
    }
    if (q_check) {
        q_check->setChecked(q_data.check);
    }
    if (q_spinLength) {
        q_spinLength->setValue(q_data.nLength);
        q_spinLength->setEnabled(q_data.check);
        q_spinLength->clearFocus();
    }
    if (q_edtDescribe) {
        q_edtDescribe->setText(QString(q_data.sRemark.c_str())
                               .split("##").first());
        q_edtDescribe->clearFocus();
    }
    if (q_edtStatus) {
        q_edtStatus->clear();
        q_edtStatus->clearFocus();
    }

    enableConnection(true);
}

void TableUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinLength, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_check, SIGNAL(toggled(bool)),
               this, SLOT(slotCheckStateChanged(bool)));
    disconnect(q_edtDescribe, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    disconnect(q_edtStatus, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));

    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinLength, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_check, SIGNAL(toggled(bool)),
                this, SLOT(slotCheckStateChanged(bool)));
        connect(q_edtDescribe, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(q_edtStatus, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// 启/停用界面控制按钮
void TableUI::enableOptionButton(bool enable)
{
    if (q_btnConfirm) {
        q_btnConfirm->setEnabled(enable);
    }
    if (q_btnCancel) {
        q_btnCancel->setEnabled(enable);
    }
}

// 校验界面数据
bool TableUI::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;
    q_attempter.notifier().sendMessage("queryExistedData@ICDMainView",
                                       (JWPARAM)&existed,
                                       (JLPARAM)&section);
    // 名称
    QPalette palette = q_edtName->palette();
    if (q_data.sDescribe.empty()) {
        q_edtStatus->setText(QStringLiteral("名称不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data.sDescribe.c_str())) {
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
    if (q_data.sCode.empty()) {
        q_edtStatus->setText(QStringLiteral("标识不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data.sCode.c_str())) {
        q_edtStatus->setText(QStringLiteral("已存在同名标识！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
    }

    // 查询已规划数据长度，避免出现修改数据长度比原来已规划数据长度小的情况
    if (q_data.check) {
        int total = 0;
        q_attempter.notifier().sendMessage("queryTableInformation@ICDMainView",
                                           (JWPARAM)&total,
                                           (JLPARAM)&QString("total"));
        if (total > q_data.nLength) {
            q_edtStatus->setText(QStringLiteral("已规划数据超过当前预设长度！"));

            return false;
        }
    }

    return true;
}
