#include "precomp.h"
#include "subtableui.h"
#include "KernelClass/globalstruct.h"
#include "limittextedit.h"
#include "limitlineedit.h"
#include "hexspinbox.h"
#include "jwt/jspinbox.h"

void SubTableUI::setUIData(const _UIData &data)
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

void* SubTableUI::uiData() const
{
    return (void*)&q_data;
}

SubTableUI::SubTableUI(QWidget* parent)
    : MetaUI(parent)
{
    QGroupBox *group = new QGroupBox(QStringLiteral("������Ϣ"));
    q_edtName = new LimitLineEdit(this);
    q_edtName->setObjectName("edtName");
    q_edtName->setMaxLength(256);
    q_edtName->setToolTip(QStringLiteral("���256���ַ���"));
    q_boxType = new QComboBox(this);
    q_spinCode = new JLargeSpinBox(this);
    q_spinCode->setObjectName("edtCode");
    q_spinCode->setRadix(16);
    q_spinCode->setRange(0, 0xff);
    q_spinCode->setPrefix("0x");
    q_spinCode->setFillChar(QChar('0'));
    q_spinSequence = new QSpinBox(this);
    q_spinSequence->setRange(1, 1e9);
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setObjectName("edtDescribe");
    q_edtDescribe->setMaxLength(256);
    q_edtDescribe->setToolTip(QStringLiteral("���256���ַ���"));

    int row = 0;
    QGridLayout *gridLayout = new QGridLayout(group);
    gridLayout->setContentsMargins(6, 3, 0, 3);
    gridLayout->addWidget(new QLabel(QStringLiteral("��   �ƣ�")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("֡���ͣ�")), ++row, 0);
    gridLayout->addWidget(q_boxType, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("֡   �룺")), ++row, 0);
    gridLayout->addWidget(q_spinCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("ʱ   ��")), ++row, 0);
    gridLayout->addWidget(q_spinSequence, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("��   ����")), ++row, 0);
    gridLayout->addWidget(q_edtDescribe, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

    layoutMain()->insertWidget(0, group);

    // 
    initBoxLenght();

    // signal-slot
    enableConnection(true);

    // ��¼ԭʼ��ɫ
    q_color = q_edtName->palette().color(QPalette::Base);
}

int SubTableUI::uiType() const
{
    return wdSubTable;
}

void SubTableUI::slotTextChanged(const QString &text)
{
    // У������
    QLineEdit *edt = qobject_cast<QLineEdit *>(sender());
    QComboBox *box = qobject_cast<QComboBox *>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox *>(sender());
    if (edt) {
        QString content = text.trimmed();
        if (edt == editStatus()) { // ������ִ�����ʾ����һ�ȷ�ϰ�ť
            buttonConfirm()->setEnabled(content.isEmpty());
        } else if (edt == q_edtName) {
            q_data.sDescribe = content.toStdString();
        }
        enableOptionButton(true);
    } else if (box) {
        if (box == q_boxType) {
            int Len = 2 * q_boxType->currentData().toInt();
            QString strValue = QString().fill('f', Len);
            q_spinCode->setRange(0, strValue.prepend("0x").toUInt(0, 16));
            Len -= q_data.sCode.length();
            if (Len > 0) {
                q_data.sCode.insert(0, Len, '0');
            } else if (Len < 0) {
                q_data.sCode = q_data.sCode.substr(abs(Len));
            }
            q_spinCode->setValue(QString(q_data.sCode.c_str()).toInt(0, 16));
        }
        enableOptionButton(true);
    } else if (spin) {
        if (spin == q_spinSequence) {
            QStringList lst = QString(q_data.sRemark.c_str()).split("##");
            if (lst.empty()) {
                lst << "" << "";
            } else if (lst.size() < 2) {
                lst << "";
            }
            lst[1] = QString::number(spin->value());
            q_data.sRemark = lst.join("##").toStdString();
        }
        enableOptionButton(true);
    }
}

// ֡�����ݱ��
void SubTableUI::slotCodeChanged(const qulonglong &value)
{
    Q_UNUSED(value);
    q_data.sCode = q_spinCode->text().remove(q_spinCode->prefix()).toStdString();

    enableOptionButton(true);
}

// ����¼�����
void SubTableUI::slotEditFinished()
{
    LimitTextEdit *edt = qobject_cast<LimitTextEdit*>(sender());
    if (edt) {
        QString content = edt->toPlainText().trimmed();
        if (edt == q_edtDescribe) {
            QStringList lst = QString(q_data.sRemark.c_str()).split("##");
            if (lst.empty()) {
                lst << "" << "";
            } else if (lst.size() < 2) {
                lst << "";
            }
            lst[0] = content;
            q_data.sRemark = lst.join("##").toStdString();
        }
        enableOptionButton(true);
    }
}

// ȷ��
void SubTableUI::confirm()
{
    // У��
    if (!dataValid()) {
        return;
    }
    bool result = false;

    emit confirmed(result);
    if (result) {
        MetaUI::confirm();
    } else {
        editStatus()->setText(QStringLiteral("��������ʧ�ܣ�"));
    }
}

// ȡ��
void SubTableUI::cancel()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    MetaUI::cancel();

    emit canceled();
}

// ��ʼ����������
void SubTableUI::init()
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
    if (q_boxType) {  // ֡�볤��
        q_boxType->setCurrentIndex(q_boxType->findData(q_data.sCode.length() / 2));
    }
    if (q_spinCode) {
        palette = q_spinCode->palette();
        palette.setColor(QPalette::Base, q_color);

        QString strValue = QString().fill('f', q_data.sCode.length());
        q_spinCode->setRange(0, strValue.prepend("0x").toUInt(0, 16));
        q_spinCode->setPalette(palette);
        q_spinCode->setValue(QString(q_data.sCode.c_str()).toInt(0, 16));
        q_spinCode->clearFocus();
        q_data.sCode = q_spinCode->text().remove(q_spinCode->prefix())
            .toStdString();
    }
    QStringList lst = QString(q_data.sRemark.c_str()).split("##");
    if (q_spinSequence) {
        q_spinSequence->setValue(lst.last().toInt());
    }
    if (q_edtDescribe) {
        q_edtDescribe->setText(lst.first());
        q_edtDescribe->clearFocus();
    }
    editStatus()->clear();
    editStatus()->clearFocus();

    enableConnection(true);
}

// ��ʼ��֡�볤��������
void SubTableUI::initBoxLenght()
{
    // �̶���ֻ��1��2��4�ֽڿ�ѡ
    if (!q_boxType) {
        return;
    }
    // ���ԭʼ����
    q_boxType->clear();

    q_boxType->addItem(QStringLiteral("1�ֽ�"), 1);
    q_boxType->addItem(QStringLiteral("2�ֽ�"), 2);
    q_boxType->addItem(QStringLiteral("4�ֽ�"), 4);
}

void SubTableUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_boxType, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinCode, SIGNAL(valueChanged(const qulonglong &)),
               this, SLOT(slotCodeChanged(const qulonglong &)));
    disconnect(q_spinSequence, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtDescribe, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    disconnect(editStatus(), SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));

    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_boxType, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinCode, SIGNAL(valueChanged(const qulonglong &)),
                this, SLOT(slotCodeChanged(const qulonglong &)));
        connect(q_spinSequence, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtDescribe, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(editStatus(), SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// У���������
bool SubTableUI::dataValid()
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
    // ��ʶ
    section = "code";
    args.clear();
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    palette = q_spinCode->palette();
    if (q_data.sCode.empty()) {
        editStatus()->setText(QStringLiteral("֡�벻��Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_spinCode->setPalette(palette);
        q_spinCode->setFocus();

        return false;
    } else if (existed.contains(q_data.sCode.c_str())) {
        editStatus()->setText(QStringLiteral("�Ѵ���ͬ����ʶ��"));
        palette.setColor(QPalette::Base, Qt::red);
        q_spinCode->setPalette(palette);
        q_spinCode->setFocus();

        return false;
    } else {
        if (q_data.sCode.size() < 2) {
            q_data.sCode.insert(0, "0");
            q_spinCode->setValue(QString(q_data.sCode.c_str()).toInt(0, 16));
        }
        palette.setColor(QPalette::Base, q_color);
        q_spinCode->setPalette(palette);
    }

    return true;
}
