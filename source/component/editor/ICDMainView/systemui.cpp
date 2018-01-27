#include "systemui.h"
#include "limittextedit.h"
#include "limitlineedit.h"

SystemUI::SystemUI(IJAttempter &attempter, QWidget *parent)
    : q_attempter(attempter)
    , MetaUI(parent)
{
    QGroupBox *group = new QGroupBox(QStringLiteral("基本信息"));

    q_edtName = new LimitLineEdit(this);
    q_edtName->setObjectName("edtName");
    q_edtName->setMaxLength(30);
    q_edtName->setToolTip(QStringLiteral("最多30个字符！"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,29}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setObjectName("edtCode");
    q_edtCode->setMaxLength(30);
    q_edtCode->setToolTip(QStringLiteral("最多30个字符！"));
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setObjectName("edtDescribe");
    q_edtDescribe->setMaxLength(120);
    q_edtDescribe->setToolTip(QStringLiteral("最多120个字符！"));

    q_gridLayout = new QGridLayout(group);
    int row = 0;
    q_gridLayout->addWidget(new QLabel(QStringLiteral("名称：")), row, 0);
    q_gridLayout->addWidget(q_edtName, row, 1);
    q_gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("标识：")), ++row, 0);
    q_gridLayout->addWidget(q_edtCode, row, 1);
    q_gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("描述：")), ++row, 0);
    q_gridLayout->addWidget(q_edtDescribe, row, 1, 2, 1);
    q_gridLayout->setRowStretch(++row, 1);

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

    QVBoxLayout* veriLayoutMain = new QVBoxLayout(this);
    veriLayoutMain->setContentsMargins(0, 0, 0, 0);
    veriLayoutMain->addWidget(group);
    veriLayoutMain->addStretch();
    veriLayoutMain->addWidget(q_edtStatus);
    veriLayoutMain->addLayout(btnLayout);

    //
    connect(q_btnConfirm, SIGNAL(clicked()), this, SLOT(slotConfirm()));
    connect(q_btnCancel, SIGNAL(clicked()), this, SLOT(slotCanceled()));
    enableConnection(true);

    // 记录原始颜色
    q_color = q_edtName->palette().color(QPalette::Base);
}

SystemUI::~SystemUI()
{
}

void SystemUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    q_data = *reinterpret_cast<stSystem*>(data.data);
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

void *SystemUI::uiData() const
{
    return (void*)&q_data;
}

// 编辑框数据录入完成
void SystemUI::slotEditFinished()
{
    LimitTextEdit *tedt = qobject_cast<LimitTextEdit*>(sender());
    if (tedt) {
        QString content = tedt->toPlainText().trimmed();
        if (tedt == q_edtDescribe) {
            q_data.sDescribe = content.toStdString();
        }
        enableOptionButton(true);
    }
}

// 编辑框文本变更
void SystemUI::slotTextChanged(const QString &text)
{
    QString content = text.trimmed();
    // 校验数据
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    if (edt) {
        if (edt == q_edtStatus) { // 如果出现错误提示，则灰化确认按钮
            if (q_btnConfirm) {
                q_btnConfirm->setEnabled(content.isEmpty());
            }
        } else {
            if (edt == q_edtName) {
                q_data.sName = content.toStdString();
            } else if (edt == q_edtCode) {
                q_data.sSign = content.toStdString();
            }
            enableOptionButton(true);
        }
    }
}

// 确认
void SystemUI::slotConfirm()
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
void SystemUI::slotCanceled()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// 初始化界面数据
void SystemUI::init()
{
    enableConnection(false);

    enableOptionButton(false);
    QPalette palette;
    if (q_edtName) {
        palette = q_edtName->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
        q_edtName->setText(q_data.sName.c_str());
        q_edtName->clearFocus();
    }
    if (q_edtCode) {
        palette = q_edtCode->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
        q_edtCode->setText(q_data.sSign.c_str());
        q_edtCode->clearFocus();
    }
    if (q_edtDescribe) {
        q_edtDescribe->setText(q_data.sDescribe.c_str());
        q_edtDescribe->clearFocus();
    }
    if (q_edtStatus) {
        q_edtStatus->clear();
        q_edtStatus->clearFocus();
    }

    enableConnection(true);
}

void SystemUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtDescribe, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    disconnect(q_edtStatus, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));

    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtDescribe, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(q_edtStatus, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// 启/停用界面控制按钮
void SystemUI::enableOptionButton(bool enable)
{
    if (q_btnConfirm) {
        q_btnConfirm->setEnabled(enable);
    }
    if (q_btnCancel) {
        q_btnCancel->setEnabled(enable);
    }
}

// 校验界面数据
bool SystemUI::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;
    q_attempter.notifier().sendMessage("queryExistedData@ICDMainView",
                                       (JWPARAM)&existed,
                                       (JLPARAM)&section);
    // 名称
    QPalette palette = q_edtName->palette();
    if (q_data.sName.empty()) {
        q_edtStatus->setText(QStringLiteral("名称不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data.sName.c_str())) {
        q_edtStatus->setText(QStringLiteral("已存在同名系统！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
    }
    // 标识
    section = "code";
    q_attempter.notifier().sendMessage("queryExistedData@ICDMainView",
                                       (JWPARAM)&existed,
                                       (JLPARAM)&section);
    palette = q_edtCode->palette();
    if (q_data.sSign.empty()) {
        q_edtStatus->setText(QStringLiteral("标识不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data.sSign.c_str())) {
        q_edtStatus->setText(QStringLiteral("已存在同名标识！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
    }

    return true;
}
