#include "precomp.h"
#include "SystemEdit.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"

SystemEdit::SystemEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    QGroupBox *group = new QGroupBox(QStringLiteral("������Ϣ"));
    q_edtName = new LimitLineEdit(this);
    q_edtName->setObjectName("edtName");
    q_edtName->setMaxLength(256);
    q_edtName->setToolTip(QStringLiteral("���256���ַ���"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,255}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setObjectName("edtCode");
    q_edtCode->setMaxLength(256);
    q_edtCode->setToolTip(QStringLiteral("���256���ַ���"));
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setObjectName("edtDescribe");
    q_edtDescribe->setMaxLength(256);
    q_edtDescribe->setToolTip(QStringLiteral("���256���ַ���"));

    int row = 0;
    q_gridLayout = new QGridLayout(group);
    q_gridLayout->setContentsMargins(6, 3, 0, 3);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("���ƣ�")), row, 0);
    q_gridLayout->addWidget(q_edtName, row, 1);
    q_gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("��ʶ��")), ++row, 0);
    q_gridLayout->addWidget(q_edtCode, row, 1);
    q_gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    q_gridLayout->addWidget(new QLabel(QStringLiteral("������")), ++row, 0);
    q_gridLayout->addWidget(q_edtDescribe, row, 1, 2, 1);
    q_gridLayout->setRowStretch(++row, 1);

    layoutMain()->insertWidget(0, group);

    //
    enableConnection(true);

    // ��¼ԭʼ��ɫ
    q_color = q_edtName->palette().color(QPalette::Base);
}

int SystemEdit::windowType() const
{
    return wdSystem;
}

void SystemEdit::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    q_data = *reinterpret_cast<stSystem*>(data.data);
    q_old = q_data;
    setProperty("option", data.type);

    init();

    // �������������Ĭ�ϱ������
    if (GlobalDefine::optNew == data.type) {
        buttonConfirm()->setEnabled(true);
    }

    show();
}

void *SystemEdit::uiData() const
{
    return (void*)&q_data;
}

// �༭������¼�����
void SystemEdit::slotEditFinished()
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

// �༭���ı����
void SystemEdit::slotTextChanged(const QString &text)
{
    QString content = text.trimmed();
    // У������
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    if (edt) {
        if (edt == editStatus()) { // ������ִ�����ʾ����һ�ȷ�ϰ�ť
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

// ȷ��
void SystemEdit::confirm()
{
    if (!dataValid()) {
        return;
    }
    bool result = false;

    emit confirmed(result);
    if (result) {
        ObjectEdit::confirm();
    } else {
        editStatus()->setText(QStringLiteral("��������ʧ�ܣ�"));
    }
}

// ȡ��
void SystemEdit::cancel()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    ObjectEdit::cancel();

    emit canceled();
}

// ��ʼ����������
void SystemEdit::init()
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

void SystemEdit::enableConnection(bool enable)
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

// У���������
bool SystemEdit::dataValid()
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
        editStatus()->setText(QStringLiteral("���Ʋ���Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data.sName.c_str())) {
        editStatus()->setText(QStringLiteral("�Ѵ���ͬ��ϵͳ��"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
    }
    // ��ʶ
    section = "code";
    args.clear();
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    palette = q_edtCode->palette();
    if (q_data.sSign.empty()) {
        editStatus()->setText(QStringLiteral("��ʶ����Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data.sSign.c_str())) {
        editStatus()->setText(QStringLiteral("�Ѵ���ͬ����ʶ��"));
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
