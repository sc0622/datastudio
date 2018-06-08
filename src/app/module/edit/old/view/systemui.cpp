#include "precomp.h"
#include "systemui.h"
#include "limittextedit.h"
#include "limitlineedit.h"

SystemUI::SystemUI(QWidget *parent)
    : MetaUI(parent)
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

    int row = 0;
    q_gridLayout = new QGridLayout(group);
    q_gridLayout->setContentsMargins(6, 3, 0, 3);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("名称：")), row, 0);
    q_gridLayout->addWidget(q_edtName, row, 1);
    q_gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("标识：")), ++row, 0);
    q_gridLayout->addWidget(q_edtCode, row, 1);
    q_gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("描述：")), ++row, 0);
    q_gridLayout->addWidget(q_edtDescribe, row, 1, 2, 1);
    q_gridLayout->setRowStretch(++row, 1);

    layoutMain()->insertWidget(0, group);

    //
    enableConnection(true);

    // 记录原始颜色
    q_color = q_edtName->palette().color(QPalette::Base);
}

int SystemUI::uiType() const
{
    return wdSystem;
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
        buttonConfirm()->setEnabled(true);
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
        if (edt == editStatus()) { // 如果出现错误提示，则灰化确认按钮
            buttonConfirm()->setEnabled(content.isEmpty());
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
void SystemUI::confirm()
{
    if (!dataValid()) {
        return;
    }
    bool result = false;

    emit confirmed(result);
    if (result) {
//         _UIData data;
// 
//         data.data = &q_data;
//         setUIData(data);
    } else {
        editStatus()->setText(QStringLiteral("保存数据失败！"));
        buttonConfirm()->setEnabled(true);
    }
}

// 取消
void SystemUI::cancel()
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
    editStatus()->clear();
    editStatus()->clearFocus();

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
    disconnect(editStatus(), SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));

    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtDescribe, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(editStatus(), SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// 校验界面数据
bool SystemUI::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;
    QVariantList args;
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    // 名称
    QPalette palette = q_edtName->palette();
    if (q_data.sName.empty()) {
        editStatus()->setText(QStringLiteral("名称不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data.sName.c_str())) {
        editStatus()->setText(QStringLiteral("已存在同名系统！"));
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
    args.clear();
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    palette = q_edtCode->palette();
    if (q_data.sSign.empty()) {
        editStatus()->setText(QStringLiteral("标识不能为空！"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data.sSign.c_str())) {
        editStatus()->setText(QStringLiteral("已存在同名标识！"));
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
