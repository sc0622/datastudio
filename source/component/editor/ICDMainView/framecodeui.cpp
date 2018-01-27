#include "framecodeui.h"
#include "limittextedit.h"
#include "limitlineedit.h"

FrameCodeUI::FrameCodeUI(IJAttempter &attempter, QWidget *parent)
    : q_attempter(attempter)
    , MetaUI(parent)
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
    q_boxLength = new QComboBox(this);
    q_boxData = new QComboBox(this);
    q_edtRemak = new LimitTextEdit(this);
    q_edtRemak->setMaxLength(200);
    q_edtRemak->setToolTip(QStringLiteral("最多200个字符！"));

    int row = 0;
    QGridLayout* gridLayout = new QGridLayout(group);
    gridLayout->addWidget(new QLabel(QStringLiteral("名   称：")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("标   识：")), ++row, 0);
    gridLayout->addWidget(q_edtCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("长   度：")), ++row, 0);
    gridLayout->addWidget(q_boxLength, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("帧数据：")), ++row, 0);
    gridLayout->addWidget(q_boxData, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("描   述：")), ++row, 0);
    gridLayout->addWidget(q_edtRemak, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

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
    veriLayoutMain->addWidget(group);
    veriLayoutMain->addStretch();
    veriLayoutMain->addWidget(q_edtStatus);
    veriLayoutMain->addLayout(btnLayout);

    // 
    initBoxLenght();

    // signal-slot
    connect(q_btnConfirm, SIGNAL(clicked()), this, SLOT(slotConfirm()));
    connect(q_btnCancel, SIGNAL(clicked()), this, SLOT(slotCanceled()));
    enableConnection(true);
    // 记录原始颜色
    q_color = q_edtName->palette().color(QPalette::Base);
}

void FrameCodeUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    ICDMetaData::smtMeta &meta =
        *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
    q_old = std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
    q_data = std::dynamic_pointer_cast<ICDFrameCodeData>(q_old->clone());
    setProperty("option", data.type);

    enableConnection(false);
    initBoxData();
    enableConnection(true);

    init();
    // 如果是新增，则默认保存可用
    if (GlobalDefine::optNew == data.type) {
        if (q_btnConfirm) {
            q_btnConfirm->setEnabled(true);
        }
    }

    show();
}

void*FrameCodeUI::uiData() const
{
    return (void*)&q_data;
}

// 界面操作按钮可用
void FrameCodeUI::changeDataType(int type)
{
    q_data->setType(type);
    enableOptionButton(true);
}

// 原始数据类型
int FrameCodeUI::originalType() const
{
    if (!q_old) {
        return GlobalDefine::dtInvalid;
    }

    return q_old->type();
}

void FrameCodeUI::slotEditFinished()
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

void FrameCodeUI::slotTextChanged(const QString& text)
{
    QString content = text.trimmed();
    // 校验数据
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    if (edt) {
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
    } else if (box) {
        if (box == q_boxLength) {
            q_data->setFrameType(q_boxLength->itemData(q_boxLength->currentIndex()).toInt());
        } else if (box == q_boxData) {
            q_data->setDefaultStr(box->itemData(box->currentIndex())
                                  .toString().toStdString());
        }
        enableOptionButton(true);
    }
}

// 确认
void FrameCodeUI::slotConfirm()
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
void FrameCodeUI::slotCanceled()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// 初始化界面数据
void FrameCodeUI::init()
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
    if (q_boxLength) {  // 帧码长度
        int index = q_boxLength->findData(q_data->frameType());
        if (-1 == index) {
            index = 0;
        }
        q_boxLength->setCurrentIndex(index);
    }
    if (q_boxData) {    // 帧数据
        QString code = QString::fromStdString(q_data->defaultStr());
        if (code.isEmpty()) {
            q_boxData->setCurrentIndex(0);
        } else {
            int index = q_boxData->findData(code);
            q_boxData->setCurrentIndex((index < 0) ? 0 : index);
        }
    }
    int bound = 0;
    q_attempter.notifier().sendMessage("queryTableInformation@ICDMainView",
                                       (JWPARAM)&bound, 
                                       (JLPARAM)&QString("total"));
    if (q_edtRemak) {   // 描述
        q_edtRemak->setText(q_data->remark().c_str());
        q_edtRemak->clearFocus();
    }
    if (q_edtStatus) {  // 状态提示
        q_edtStatus->clear();
        q_edtStatus->clearFocus();
    }

    enableConnection(true);
}

// 初始化帧码长度下拉框
void FrameCodeUI::initBoxLenght()
{
    // 固定，只有1、2、4字节可选
    if (!q_boxLength) {
        return;
    }
    // 清空原始数据
    q_boxLength->clear();

    q_boxLength->addItem(QStringLiteral("1字节"), GlobalDefine::frame8);
    q_boxLength->addItem(QStringLiteral("2字节"), GlobalDefine::frame16);
    q_boxLength->addItem(QStringLiteral("4字节"), GlobalDefine::frame32);
}

// 初始化帧数据下拉框
void FrameCodeUI::initBoxData()
{
    if (!q_boxData) {
        return;
    }
    // 清空原始数据
    q_boxData->clear();
    // 查询数据
    QMap<int, QString> frame;
    q_attempter.notifier().sendMessage("queryFrameData@ICDMainView",
                                       (JWPARAM)&frame,
                                       (JLPARAM)&QString("idle"));
    // 将已绑定数据加入备选
    ICDComplexData::smtComplex complex
        = std::dynamic_pointer_cast<ICDComplexData>(q_data->data());
    if (complex) {
        frame[complex->serial()] = QString::fromStdString(complex->name());
    }
    QMapIterator<int, QString> it = frame;
    while (it.hasNext()) {
        it.next();
        q_boxData->addItem(QStringLiteral("绑定<%1>")
                           .arg(it.value()), it.key());
    }
    q_boxData->insertItem(0, QStringLiteral("不绑定"), -1);
}

// 启/停用信号槽
void FrameCodeUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_boxLength, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_boxData, SIGNAL(currentTextChanged(const QString &)),
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
        connect(q_boxLength, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_boxData, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtRemak, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(q_edtStatus, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// 启/停用界面控制按钮
void FrameCodeUI::enableOptionButton(bool enable)
{
    if (q_btnConfirm) {
        q_btnConfirm->setEnabled(enable);
    }
    if (q_btnCancel) {
        q_btnCancel->setEnabled(enable);
    }
}

// 校验界面数据
bool FrameCodeUI::dataValid()
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
    int lengthCheck = 0;
    q_attempter.notifier().sendMessage("queryTableInformation@ICDMainView",
                                       (JWPARAM)&lengthCheck,
                                       (JLPARAM)&QString("lengthCheck"));
    if (0 == lengthCheck) {
        return true;    // 不进行长度校验
    }
    // 长度
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

    return true;
}
