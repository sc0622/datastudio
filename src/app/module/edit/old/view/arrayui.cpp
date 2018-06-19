#include "precomp.h"
#include "arrayui.h"
#include "limittextedit.h"
#include "limitlineedit.h"

ArrayUI::ArrayUI(QWidget *parent)
    : MetaUI(parent)
{
    QGroupBox* group = new QGroupBox(QStringLiteral("������Ϣ"), this);
    q_edtName = new LimitLineEdit(this);
    q_edtName->setMaxLength(40);
    q_edtName->setToolTip(QStringLiteral("���40���ַ���"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,19}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(20);
    q_edtCode->setToolTip(QStringLiteral("���20���ַ���"));
    q_boxArrayType = new QComboBox(this);
    q_spinCount = new QSpinBox(this);
    q_spinCount->setRange(1, INT_MAX);
    q_edtRemak = new LimitTextEdit(this);
    q_edtRemak->setMaxLength(200);
    q_edtRemak->setToolTip(QStringLiteral("���200���ַ���"));

    int row = 0;
    QGridLayout* gridLayout = new QGridLayout(group);
    gridLayout->setContentsMargins(6, 3, 0, 3);
    gridLayout->addWidget(new QLabel(QStringLiteral("��      �ƣ�")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��      ʶ��")), ++row, 0);
    gridLayout->addWidget(q_edtCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("�������ͣ�")), ++row, 0);
    gridLayout->addWidget(q_boxArrayType, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��   ��")), ++row, 0);
    gridLayout->addWidget(q_spinCount, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��      ����")), ++row, 0);
    gridLayout->addWidget(q_edtRemak, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

    layoutMain()->insertWidget(0, group);

    // signal-slot
    enableConnection(true);

    // ��¼ԭʼ��ɫ
    q_color = q_edtName->palette().color(QPalette::Base);
}

int ArrayUI::uiType() const
{
    return wdArray;
}

void ArrayUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    ICDMetaData::smtMeta &meta =
        *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
    q_old = std::dynamic_pointer_cast<ICDArrayData>(meta);
    q_data = std::dynamic_pointer_cast<ICDArrayData>(q_old->clone());
    setProperty("option", data.type);

    enableConnection(false);
    initBoxType();
    enableConnection(true);

    init();
    // �������������Ĭ�ϱ������
    if (GlobalDefine::optNew == data.type) {
        buttonConfirm()->setEnabled(true);
    }

    show();
}

void*ArrayUI::uiData() const
{
    return (void*)&q_data;
}

// ���������ť����
void ArrayUI::changeDataType(int type)
{
    q_data->setType(type);
    enableOptionButton(true);
}

// ԭʼ��������
int ArrayUI::originalType() const
{
    if (!q_old) {
        return GlobalDefine::dtInvalid;
    }

    return q_old->type();
}

void ArrayUI::slotEditFinished()
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

void ArrayUI::slotTextChanged(const QString& text)
{
    QString content = text.trimmed();
    // У������
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (edt) {
        if (edt == editStatus()) { // ������ִ�����ʾ����һ�ȷ�ϰ�ť
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
        if (box == q_boxArrayType) {
            q_data->setArrayType(
                q_boxArrayType->itemData(q_boxArrayType->currentIndex())
                .toInt());
        }
        enableOptionButton(true);
    } else if (spin) {
        if (spin == q_spinCount) {
            q_data->setCount(q_spinCount->value());
        }
        enableOptionButton(true);
    }
}

// ȷ��
void ArrayUI::confirm()
{
    // ������ź��ֽں�
    QVariantList args;
    args.append(qVariantFromValue((void*)&q_data));
    jnotify->send("edit.fillBitSerial", args);

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
void ArrayUI::cancel()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    MetaUI::cancel();

    emit canceled();
}

// ��ʼ����������
void ArrayUI::init()
{
    enableConnection(false);

    enableOptionButton(false);
    QPalette palette;
    if (q_edtName) {    // ����
        palette = q_edtName->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtName->setPalette(palette);
        q_edtName->setText(q_data->name().c_str());
        q_edtName->clearFocus();
    }
    if (q_edtCode) {    // ����
        palette = q_edtCode->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
        q_edtCode->setText(q_data->proCode().c_str());
        q_edtCode->clearFocus();
    }
    if (q_boxArrayType) {   // ��������
        int index = q_boxArrayType->findData(q_data->arrayType());
        q_boxArrayType->setCurrentIndex((index < 0) ? 0 : index);
    }
    if (q_spinCount) {     // ���鳤��
        palette = q_spinCount->palette();
        palette.setColor(QPalette::Base, q_color);
        q_spinCount->setPalette(palette);
        q_spinCount->setRange(1, INT_MAX);
        q_spinCount->setValue(q_data->count());
    }
    int bound = 0;
    QVariantList args;
    args.append(qVariantFromValue((void*)&bound));
    QString command("total");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryTableInformation", args);

    if (q_edtRemak) {   // ����
        q_edtRemak->setText(q_data->remark().c_str());
        q_edtRemak->clearFocus();
    }
    // ״̬��ʾ
    editStatus()->clear();
    editStatus()->clearFocus();

    enableConnection(true);
}

// ��ʼ��У������������
void ArrayUI::initBoxType()
{
    if (!q_boxArrayType) {
        return;
    }
    // ���ԭʼ����
    q_boxArrayType->clear();
    // ��ѯ����
    std::vector<stDictionary> names;
    QVariantList args;
    args.append(qVariantFromValue((void*)&names));
    args.append(int(GlobalDefine::dicArrayType));
    jnotify->send("edit.queryDictionaryTable", args);

    const int count = names.size();
    for (int i = 0; i < count; ++i) {
        const stDictionary &dic = names.at(i);
        q_boxArrayType->addItem(dic.sDec.c_str(), dic.nCode);
    }
}

// ��/ͣ���źŲ�
void ArrayUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_boxArrayType, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinCount, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinCount, SIGNAL(editingFinished()),
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
        connect(q_boxArrayType, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinCount, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinCount, SIGNAL(editingFinished()),
                this, SLOT(slotEditFinished()));
        connect(q_edtRemak, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(editStatus(), SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
    }
}

// У���������
bool ArrayUI::dataValid()
{
    QString section = "name";
    QMap<QString, QString> existed;
    QVariantList args;
    args.append(qVariantFromValue((void*)&existed));
    args.append(qVariantFromValue((void*)&section));
    jnotify->send("edit.queryExistedData", args);

    // ����
    QPalette palette = q_edtName->palette();
    if (q_data->name().empty()) {
        editStatus()->setText(QStringLiteral("���Ʋ���Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data->name().c_str())) {
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
    if (q_data->proCode().empty()) {
        editStatus()->setText(QStringLiteral("��ʶ����Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data->proCode().c_str())) {
        editStatus()->setText(QStringLiteral("�Ѵ���ͬ����ʶ��"));
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
        return true;    // �����г���У��
    }
    // ����
    int remains = 0;
    int offset = 0;
    args.clear();
    args.append(qVariantFromValue((void*)&remains));
    command = "remains";
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryTableInformation", args);

    // ����ƫ����
    args.clear();
    args.append(qVariantFromValue((void*)&q_data));
    args.append(qVariantFromValue((void*)&offset));
    jnotify->send("edit.queryLengthOffset", args);
    if (remains < offset || remains < 0) {
        editStatus()->setText(QStringLiteral("�滮���ݳ���Ԥ���ܳ��ȣ�"));

        return false;
    }

    return true;
}
