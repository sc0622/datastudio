#include "precomp.h"
#include "complexui.h"
#include "limittextedit.h"
#include "limitlineedit.h"

ComplexUI::ComplexUI(QWidget *parent)
    : MetaUI(parent)
{
    QGroupBox* basicGroup = new QGroupBox(QStringLiteral("������Ϣ"), this);
    layoutMain()->insertWidget(0, basicGroup);

    q_edtName = new LimitLineEdit(this);
    q_edtName->setMaxLength(40);
    q_edtName->setToolTip(QStringLiteral("���40���ַ���"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,19}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(20);
    q_edtCode->setToolTip(QStringLiteral("���20���ַ���"));
    q_spinLength = new QSpinBox(this);
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setMaxLength(200);
    q_edtDescribe->setToolTip(QStringLiteral("���200���ַ���"));
    q_spinLength->setRange(0, 1e6);
    q_spinLength->setSuffix(QStringLiteral("   �ֽ�"));
    q_spinLength->setEnabled(false);

    int row = 0;
    QGridLayout* gridLayout = new QGridLayout(basicGroup);
    gridLayout->setContentsMargins(6, 3, 0, 3);
    gridLayout->addWidget(new QLabel(QStringLiteral("����:")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��ʶ:")), ++row, 0);
    gridLayout->addWidget(q_edtCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("����:")), ++row, 0);
    gridLayout->addWidget(q_spinLength, row, 1);
    //gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("����:")), ++row, 0);
    gridLayout->addWidget(q_edtDescribe, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

    // signal-slot
    enableConnection(true);

    // ��¼ԭʼ��ɫ
    q_color = q_edtName->palette().color(QPalette::Base);
}

ComplexUI::~ComplexUI()
{

}

void ComplexUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    ICDMetaData::smtMeta &meta =
        *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
    q_old = std::dynamic_pointer_cast<ICDComplexData>(meta);
    q_data = std::dynamic_pointer_cast<ICDComplexData>(q_old->clone());
    setProperty("option", data.type);

    init();
    // �������������Ĭ�ϱ������
    if (GlobalDefine::optNew == data.type) {
        buttonConfirm()->setEnabled(true);
    }

    show();
}

void* ComplexUI::uiData() const
{
    return (void*)&q_data;
}

// ���������ť����
void ComplexUI::changeDataType(int type)
{
    q_data->setType(type);
    enableOptionButton(true);
}

// ԭʼ��������
int ComplexUI::originalType() const
{
    if (!q_old) {
        return GlobalDefine::dtInvalid;
    }

    return q_old->type();
}

// ����¼�����
void ComplexUI::slotEditFinished()
{
    LimitTextEdit *tedt = qobject_cast<LimitTextEdit*>(sender());
    if (tedt) {
        QString content = tedt->toPlainText().trimmed();
        if (tedt == q_edtDescribe) {   // ��ע
            q_data->setRemark(content.toStdString());
        }
        enableOptionButton(true);
    }
}

void ComplexUI::slotTextChanged(const QString& text)
{
    QString content = text.trimmed();
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (edt) {
        if (edt == editStatus()) { // ������ִ�����ʾ����һ�ȷ�ϰ�ť
            buttonConfirm()->setEnabled(content.isEmpty());
        } else {
            if (edt == q_edtName) { // ����
                q_data->setName(content.toStdString());
            } else if (edt == q_edtCode) {  // ����
                q_data->setProCode(content.toStdString());
            }
            enableOptionButton(true);
        }
    } else if (spin) {
        if (spin == q_spinLength) { // ����
            //q_data->setLength(spin->value());
        }
        enableOptionButton(true);
    }
}

// ȷ��
void ComplexUI::confirm()
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
//         _UIData data;
// 
//         data.data = &q_data;
//         setUIData(data);
    } else {
        editStatus()->setText(QStringLiteral("��������ʧ�ܣ�"));
        buttonConfirm()->setEnabled(true);
    }
}

// ȡ��
void ComplexUI::cancel()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// ��ʼ����������
void ComplexUI::init()
{
    enableConnection(false);

    enableOptionButton(false);  // ������ť
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
    if (q_spinLength) { // ����
        q_spinLength->setValue(q_data->length());
    }
    if (q_edtDescribe) {    // ����
        q_edtDescribe->setText(q_data->remark().c_str());
    }
    // ״̬��ʾ
    editStatus()->clear();
    editStatus()->clearFocus();

    enableConnection(true);
}

// ��/ͣ���źŲ�
void ComplexUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinLength, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtDescribe, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinLength, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtDescribe, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
    }
}

// У���������
bool ComplexUI::dataValid()
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
