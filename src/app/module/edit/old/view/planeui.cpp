#include "precomp.h"
#include "planeui.h"
#include "limittextedit.h"
#include "limitlineedit.h"

PlaneUI::PlaneUI(QWidget *parent)
    : MetaUI(parent)
{
    QGroupBox *group = new QGroupBox(QStringLiteral("������Ϣ"));

    q_edtName = new LimitLineEdit(this);
    q_edtName->setObjectName("edtName");
    q_edtName->setMaxLength(30);
    q_edtName->setToolTip(QStringLiteral("���30���ַ���"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,29}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setObjectName("edtCode");
    q_edtCode->setMaxLength(30);
    q_edtCode->setToolTip(QStringLiteral("���30���ַ���"));
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setObjectName("edtDescribe");
    q_edtDescribe->setMaxLength(120);
    q_edtDescribe->setToolTip(QStringLiteral("���120���ַ���"));

    QGridLayout *gridLayout = new QGridLayout(group);
    int row = 0;
    gridLayout->addWidget(new QLabel(QStringLiteral("���ƣ�")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��ʶ��")), ++row, 0);
    gridLayout->addWidget(q_edtCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("������")), ++row, 0);
    gridLayout->addWidget(q_edtDescribe, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

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
                                   QStringLiteral("ȷ��"), this);
    q_btnConfirm->setObjectName("btnConfirm");
    q_btnConfirm->setMaximumSize(65, 35);
    q_btnConfirm->setIconSize(QSize(24, 24));
    q_btnConfirm->setFlat(true);
    q_btnCancel = new QPushButton(QIcon(":/icd/editor/images/cancel-32.png"),
                                  QStringLiteral("ȡ��"), this);
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

    // ��¼ԭʼ��ɫ
    q_color = q_edtName->palette().color(QPalette::Base);
}

PlaneUI::~PlaneUI()
{
}

void PlaneUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    q_data = *reinterpret_cast<stPlane*>(data.data);
    q_old = q_data;
    setProperty("option", data.type);

    init();
    // �������������Ĭ�ϱ������
    if (GlobalDefine::optNew == data.type) {
        if (q_btnConfirm) {
            q_btnConfirm->setEnabled(true);
        }
    }

    show();
}

void *PlaneUI::uiData() const
{
    return (void*)&q_data;
}

// �༭���ı����
void PlaneUI::slotTextChanged(const QString &text)
{
    QString content = text.trimmed();
    // У������
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    if (!edt) {
        return;
    }
    if (edt == q_edtStatus) { // ������ִ�����ʾ����һ�ȷ�ϰ�ť
        if (q_btnConfirm) {
            q_btnConfirm->setEnabled(text.isEmpty());
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

// ����¼�����
void PlaneUI::slotEditFinished()
{
    LimitTextEdit *edt = qobject_cast<LimitTextEdit*>(sender());
    if (edt) {
        QString content = edt->toPlainText().trimmed();
        if (edt == q_edtDescribe) {
            q_data.sDescribe = content.toStdString();
        }
        enableOptionButton(true);
    }
}

// ȷ��
void PlaneUI::slotConfirm()
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
        q_edtStatus->setText(QStringLiteral("��������ʧ�ܣ�"));
        q_btnConfirm->setEnabled(true);
    }
}

// ȡ��
void PlaneUI::slotCanceled()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// ��ʼ����������
void PlaneUI::init()
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

// ��/ͣ���źŲ�
void PlaneUI::enableConnection(bool enable)
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

// ��/ͣ�ý�����ư�ť
void PlaneUI::enableOptionButton(bool enable)
{
    if (q_btnConfirm) {
        q_btnConfirm->setEnabled(enable);
    }
    if (q_btnCancel) {
        q_btnCancel->setEnabled(enable);
    }
}

// У���������
bool PlaneUI::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;

    QVariantList args;
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    // ����
    QPalette palette = q_edtName->palette();
    if (q_data.sName.empty()) {
        q_edtStatus->setText(QStringLiteral("���Ʋ���Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data.sName.c_str())) {
        q_edtStatus->setText(QStringLiteral("�Ѵ���ͬ�����ͣ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
    }
    // ����
    section = "code";
    args.clear();
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    palette = q_edtCode->palette();
    if (q_data.sSign.empty()) {
        q_edtStatus->setText(QStringLiteral("��ʶ����Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data.sSign.c_str())) {
        q_edtStatus->setText(QStringLiteral("�Ѵ���ͬ����ʶ��"));
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
