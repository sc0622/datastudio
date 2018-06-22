#include "precomp.h"
#include "TableEdit.h"
#include "KernelClass/globalstruct.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"

TableEdit::TableEdit(QWidget* parent)
    : ObjectEdit(parent)
{
    QGroupBox *group = new QGroupBox(QStringLiteral("������Ϣ"));
    q_edtName = new LimitLineEdit(this);
    q_edtName->setObjectName("edtName");
    q_edtName->setMaxLength(256);
    q_edtName->setToolTip(QStringLiteral("���256���ַ���"));
    q_edtCode = new QLineEdit(this);
    q_edtCode->setObjectName("edtCode");
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,255}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(256);
    q_edtCode->setToolTip(QStringLiteral("���256���ַ���"));
    q_spinLength = new QSpinBox(this);
    q_spinLength->setObjectName("edtLength");
    q_spinLength->setRange(0, 1e6);
    q_spinLength->setSuffix(QStringLiteral("   �ֽ�"));
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setObjectName("edtDescribe");
    q_edtDescribe->setMaxLength(256);
    q_edtDescribe->setToolTip(QStringLiteral("���256���ַ���"));

    QGridLayout *gridLayout = new QGridLayout(group);
    gridLayout->setContentsMargins(6, 3, 0, 3);
    gridLayout->addWidget(new QLabel(QStringLiteral("���ƣ�")), 0, 0);
    gridLayout->addWidget(q_edtName, 0, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), 0, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��ʶ��")), 1, 0);
    gridLayout->addWidget(q_edtCode, 1, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), 1, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("���ȣ�")), 2, 0);
    gridLayout->addWidget(q_spinLength, 2, 1);
    gridLayout->addWidget(q_check = new QCheckBox(this), 2, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("������")), 3, 0);
    gridLayout->addWidget(q_edtDescribe, 3, 1, 2, 1);
    gridLayout->setRowStretch(4, 1);

    layoutMain()->insertWidget(0, group);

    // signal-slot
    enableConnection(true);

    // ��¼ԭʼ��ɫ
    q_color = q_edtName->palette().color(QPalette::Base);
}

int TableEdit::windowType() const
{
    return wdTable;
}

void TableEdit::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    q_data = *reinterpret_cast<stICDBase*>(data.data);
    q_old = q_data;
    setProperty("option", data.type);

    init();
    // �������������Ĭ�ϱ������
    if (GlobalDefine::optNew == data.type) {
        buttonConfirm()->setEnabled(true);
    }

    show();
}

void* TableEdit::uiData() const
{
    return (void*)&q_data;
}

void TableEdit::slotTextChanged(const QString &text)
{
    QString content = text.trimmed();
    // У������
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (edt) {
        if (edt == editStatus()) { // ������ִ�����ʾ����һ�ȷ�ϰ�ť
            buttonConfirm()->setEnabled(content.isEmpty());
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

// ����¼�����
void TableEdit::slotEditFinished()
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

void TableEdit::slotCheckStateChanged(bool checked)
{
    if (q_spinLength) {
        q_spinLength->setEnabled(checked);
    }
    q_data.check = checked;

    enableOptionButton(true);
}

// ȷ��
void TableEdit::confirm()
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
void TableEdit::cancel()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    ObjectEdit::cancel();

    emit canceled();
}

// ��ʼ����������
void TableEdit::init()
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
        editStatus()->clear();
        editStatus()->clearFocus();

    enableConnection(true);
}

void TableEdit::enableConnection(bool enable)
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
    disconnect(editStatus(), SIGNAL(textChanged(const QString &)),
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
        connect(editStatus(), SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// У���������
bool TableEdit::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;
    QVariantList args;
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    // ����
    QPalette palette = q_edtName->palette();
    if (q_data.sDescribe.empty()) {
        editStatus()->setText(QStringLiteral("���Ʋ���Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data.sDescribe.c_str())) {
        editStatus()->setText(QStringLiteral("�Ѵ���ͬ���"));
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
    if (q_data.sCode.empty()) {
        editStatus()->setText(QStringLiteral("��ʶ����Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data.sCode.c_str())) {
        editStatus()->setText(QStringLiteral("�Ѵ���ͬ����ʶ��"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
    }

    // ��ѯ�ѹ滮���ݳ��ȣ���������޸����ݳ��ȱ�ԭ���ѹ滮���ݳ���С�����
    if (q_data.check) {
        int total = 0;

        args.clear();
        args.append(qVariantFromValue((void*)&total));
        QString command("total");
        args.append(qVariantFromValue((void*)&command));
        jnotify->send("edit.queryTableInformation", args);
        if (total > q_data.nLength) {
            editStatus()->setText(QStringLiteral("�ѹ滮���ݳ�����ǰԤ�賤�ȣ�"));
            return false;
        }
    }

    return true;
}
