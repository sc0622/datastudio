#include "precomp.h"
#include "bitmapui.h"
#include "jwt/jxmltable.h"
#include "jwt/jsplitter.h"
#include "limittextedit.h"
#include "limitlineedit.h"

#define MAX_BIT (8 * 8 - 1)

BitMapUI::BitMapUI(QWidget *parent)
    : MetaUI(parent)
{
    QGroupBox* basicGroup = new QGroupBox(QStringLiteral("������Ϣ"), this);
    QGridLayout* gridLayout = new QGridLayout(basicGroup);
    q_edtName = new LimitLineEdit(this);
    q_edtName->setMaxLength(40);
    q_edtName->setToolTip(QStringLiteral("���40���ַ���"));
    q_edtCode = new QLineEdit(this);
    QRegExp regExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,19}");
    q_edtCode->setValidator(new QRegExpValidator(regExp));
    q_edtCode->setMaxLength(20);
    q_edtCode->setToolTip(QStringLiteral("���20���ַ���"));
    q_spinStart = new QSpinBox(this);
    q_spinLength = new QSpinBox(this);
    q_edtDefault = new QLineEdit(this);
    q_edtDescribe = new LimitTextEdit(this);
    q_edtDescribe->setMaxLength(200);
    q_edtDescribe->setToolTip(QStringLiteral("���200���ַ���"));
    q_spinStart->setRange(0, MAX_BIT);
    q_spinLength->setRange(1, MAX_BIT + 1);
    regExp.setPattern("[0-1]{,64}");
    q_edtDefault->setValidator(new QRegExpValidator(regExp));
    q_edtDefault->setMaxLength(MAX_BIT + 1);
    q_edtDefault->setPlaceholderText(QStringLiteral("������"));
    q_edtDefault->setToolTip(QStringLiteral("�����ƣ�"));

    int row = 0;
    gridLayout->addWidget(new QLabel(QStringLiteral("��   �ƣ�")), row, 0);
    gridLayout->addWidget(q_edtName, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��   ʶ��")), ++row, 0);
    gridLayout->addWidget(q_edtCode, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��ʼλ��")), ++row, 0);
    gridLayout->addWidget(q_spinStart, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("��   ��")), ++row, 0);
    gridLayout->addWidget(q_spinLength, row, 1);
    gridLayout->addWidget(new QLabel("<font color=red>*</font>"), row, 2);
    gridLayout->addWidget(new QLabel(QStringLiteral("Ĭ��ֵ��")), ++row, 0);
    gridLayout->addWidget(q_edtDefault, row, 1);
    gridLayout->addWidget(new QLabel(QStringLiteral("��   ����")), ++row, 0);
    gridLayout->addWidget(q_edtDescribe, row, 1, 2, 1);
    gridLayout->setRowStretch(++row, 1);

    QGroupBox* valueGroup = new QGroupBox(QStringLiteral("��������Ϣ"), this);
    QVBoxLayout* veriLayout = new QVBoxLayout(valueGroup);
    q_table = new JXmlTable(this);
    q_table->verticalHeader()->setFixedWidth(40);
    q_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    q_table->horizontalHeader()->setSectionsMovable(false);
    veriLayout->setContentsMargins(2, 2, 2, 2);
    veriLayout->addWidget(q_table);

    JSplitter* splitter = new JSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setScales(QList<double>() << 2.5 << 1.5);
    splitter->setHandleColor(QColor(32, 32, 32, 10));
    splitter->setHandleWidth(4);
    splitter->addWidget(basicGroup);
    splitter->addWidget(valueGroup);

    q_edtStatus = new QLineEdit(this);
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
    veriLayoutMain->addWidget(splitter);
    veriLayoutMain->addWidget(q_edtStatus);
    veriLayoutMain->addLayout(btnLayout);

    //
     q_table->loadConfig(JMain::instance()->configDir().append("/old/xmltable.xml"),
                         "EigenvalueMapTable");

    // signal-slot
    connect(q_btnConfirm, SIGNAL(clicked()), this, SLOT(slotConfirm()));
    connect(q_btnCancel, SIGNAL(clicked()), this, SLOT(slotCanceled()));
    enableConnection(true);

    // ��¼ԭʼ��ɫ
    q_color = q_edtName->palette().color(QPalette::Base);
    //    QObject *obj = q_table->itemObject(0, 0);
    //    if (obj) {
    //        QObject *objRange = reinterpret_cast<QObject *>(obj->property("range").value<void*>());
    //        if (objRange) {
    //            objRange->setProperty("minimum", 0);
    //            objRange->setProperty("maximum", 0);
    //        }
    //    }

    //QString value;
    //bool result = QMetaObject::invokeMethod(obj, "setValue", Q_ARG(QString, ""),
    //                                        Q_ARG(int, 2), Q_RETURN_ARG(QString, value));
    //if (result) {
    //    //
    //}
}

void BitMapUI::setUIData(const _UIData &data)
{
    if (!data.data) {
        return;
    }
    ICDMetaData::smtMeta &meta =
        *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
    q_old = SMT_CONVERT(ICDBitData, meta);
    q_data = SMT_CONVERT(ICDBitData, q_old->clone());
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

void*BitMapUI::uiData() const
{
    return (void*)&q_data;
}

// ���������ť����
void BitMapUI::changeDataType(int type)
{
    q_data->setType(type);
    enableOptionButton(true);
}

// ԭʼ��������
int BitMapUI::originalType() const
{
    if (!q_old) {
        return GlobalDefine::dtInvalid;
    }

    return q_old->type();
}

// ����¼�����
void BitMapUI::slotEditFinished()
{
    LimitTextEdit *tedt = qobject_cast<LimitTextEdit*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (tedt) {
        QString content = tedt->toPlainText().trimmed();
        if (tedt == q_edtDescribe) {   // ��ע
            q_data->setRemark(content.toStdString());
        }
    } else if (spin) {
        if (spin == q_spinStart) {
            // ���³�������
            q_spinLength->setMaximum(MAX_BIT + 1 - spin->value());
        } else if (spin == q_spinLength) {
            // ����Ĭ��ֵ��Χ
            q_edtDefault->setMaxLength(spin->value());
        }
        int bitLen = q_spinLength->value();
        if (bitLen > 0) {
            // �޸����ƣ�ʹbitӳ�����Ϳ��Ե�����ĳһλ��ֵ
            q_edtDefault->setMaxLength(bitLen);
            // ��������ֵ��
            updateTable(bitLen, spin == q_spinLength);
        }
    }
    enableOptionButton(true);
}

// �༭�ı����
void BitMapUI::slotTextChanged(const QString& text)
{
    QString content = text.trimmed();
    QLineEdit *edt = qobject_cast<QLineEdit*>(sender());
    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if (edt) {
        if (edt == q_edtStatus) { // ������ִ�����ʾ����һ�ȷ�ϰ�ť
            if (q_btnConfirm) {
                q_btnConfirm->setEnabled(content.isEmpty());
            }
        } else {
            if (edt == q_edtName) { // ����
                q_data->setName(content.toStdString());
            } else if (edt == q_edtCode) {  // ����
                q_data->setProCode(content.toStdString());
            } else if (edt == q_edtDefault) { // Ĭ��ֵ
                q_data->setDefaultStr(QString::number(content.toLongLong(0, 2)).toStdString());
            }
            enableOptionButton(true);
        }
    } else if (spin) {
        if (spin == q_spinStart) {  // ��ʼλ
            q_data->setStart(content.toInt());
            q_data->setBitLength(q_spinLength->value());
        } else if (spin == q_spinLength) {  // ���س���
            q_data->setBitLength(content.toInt());
        }
        enableOptionButton(true);
    }
}

// ȷ��
void BitMapUI::slotConfirm()
{
    // ������ź��ֽں�
    QVariantList args;
    args.append(qVariantFromValue((void*)&q_data));
    jnotify->send("edit.fillBitSerial", args);

    // У��
    if (!dataValid()) {
        return;
    }
    // ��������ֵ
    q_data->setValues(tableValues());

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
void BitMapUI::slotCanceled()
{
    _UIData data;

    data.data = &q_old;
    setUIData(data);

    emit canceled();
}

// ��ʼ����������
void BitMapUI::init()
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
    if (q_spinStart) {  // ��ʼλ
        palette = q_spinStart->palette();
        palette.setColor(QPalette::Base, q_color);
        q_spinStart->setPalette(palette);
        q_spinStart->setMinimum(0);
        q_spinStart->setMaximum(MAX_BIT);
        q_spinStart->setValue(q_data->start());
    }
    if (q_spinLength) {    // ����
        palette = q_spinLength->palette();
        palette.setColor(QPalette::Base, q_color);
        q_spinLength->setPalette(palette);
        q_spinLength->setMaximum(MAX_BIT + 1);
        q_spinLength->setValue(q_data->length());
    }
    if (q_edtDefault) { // Ĭ��ֵ
        palette = q_edtDefault->palette();
        palette.setColor(QPalette::Base, q_color);
        q_edtDefault->setPalette(palette);
        /*
        if (GlobalDefine::dtBitValue == q_data->type()) {
            q_spinDefault->setMaximum(pow(2.0, q_data->length()) - 1);
        } else {
            q_spinDefault->setMaximum(1);
            }*/
        // �޸����ƣ�ʹbitӳ�����Ϳ��Ե�����ĳһλ��ֵ
        q_edtDefault->setMaxLength(q_data->length());
        q_edtDefault->setText(QString::number(QString(q_data->defaultStr().c_str())
                                              .toULongLong(), 2));
    }
    if (q_edtDescribe) {    // ����
        q_edtDescribe->setText(q_data->remark().c_str());
    }
    if (q_table) {  // ����ֵ
        std::map<double, std::string> items = q_data->values();
        std::map<double, std::string>::iterator it = items.begin();
        q_table->clearContents();
        _Eigenvalue data;
        int bound = q_data->length();
        for (int i = 0; i < bound; ++i) {
            data.value = i;
            updateMapOne(i, data, GlobalDefine::optNew);
        }
        int start = q_data->start();
        for (; it != items.end(); ++it) {
            data.value = it->first;
            data.describe = it->second.c_str();

            updateMapOne(data.value - start, data,
                         GlobalDefine::optEdit);
        }
    }
    if (q_edtStatus) {  // ״̬��ʾ
        q_edtStatus->clear();
        q_edtStatus->clearFocus();
    }

    enableConnection(true);
}

// ��/ͣ���źŲ�
void BitMapUI::enableConnection(bool enable)
{
    disconnect(q_edtName, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtCode, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinStart, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinStart, SIGNAL(editingFinished()),
               this, SLOT(slotEditFinished()));
    disconnect(q_spinLength, SIGNAL(valueChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_spinLength, SIGNAL(editingFinished()),
               this, SLOT(slotEditFinished()));
    disconnect(q_edtDefault, SIGNAL(textChanged(const QString &)),
               this, SLOT(slotTextChanged(const QString &)));
    disconnect(q_edtDescribe, SIGNAL(textChanged()),
               this, SLOT(slotEditFinished()));
    disconnect(q_table, SIGNAL(contentChanged()),
               this, SLOT(slotEditFinished()));
    if (enable) {
        connect(q_edtName, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtCode, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinStart, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinStart, SIGNAL(editingFinished()),
                this, SLOT(slotEditFinished()));
        connect(q_spinLength, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_spinLength, SIGNAL(editingFinished()),
                this, SLOT(slotEditFinished()));
        connect(q_edtDefault, SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(q_edtDescribe, SIGNAL(textChanged()),
                this, SLOT(slotEditFinished()));
        connect(q_table, SIGNAL(contentChanged()),
                this, SLOT(slotEditFinished()));
    }
}

// ��/ͣ�ý�����ư�ť
void BitMapUI::enableOptionButton(bool enable)
{
    if (q_btnConfirm) {
        q_btnConfirm->setEnabled(enable);
    }
    if (q_btnCancel) {
        q_btnCancel->setEnabled(enable);
    }
}

// У���������
bool BitMapUI::dataValid()
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
        q_edtStatus->setText(QStringLiteral("���Ʋ���Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtName->setPalette(palette);
        q_edtName->setFocus();

        return false;
    } else if (existed.contains(q_data->name().c_str())) {
        q_edtStatus->setText(QStringLiteral("�Ѵ���ͬ���"));
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
        q_edtStatus->setText(QStringLiteral("��ʶ����Ϊ�գ�"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else if (existed.contains(q_data->proCode().c_str())) {
        q_edtStatus->setText(QStringLiteral("�Ѵ���ͬ����ʶ��"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtCode->setPalette(palette);
        q_edtCode->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtCode->setPalette(palette);
    }
    // ���Ĭ��ֵ
    palette = q_edtDefault->palette();
    if (q_edtDefault->text().length() > q_spinLength->value()) {
        q_edtStatus->setText(QStringLiteral("Ĭ��ֵ������ֹ��Χ��"));
        palette.setColor(QPalette::Base, Qt::red);
        q_edtDefault->setPalette(palette);
        q_edtDefault->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_edtDefault->setPalette(palette);
    }
    // ��������
    palette = q_table->palette();
    bool error = false;
    int count = q_table->rowCount();
    for (int i = 0; i < count; ++i) {
        if (q_table->itemValue(i, 2).toString().contains(",")
                || q_table->itemValue(i, 3).toString().contains(",")) {
            error = true;
            break;
        }
    }
    if (error) {
        q_edtStatus->setText(QStringLiteral("ֵ�����в����д˷��š�,��"));
        palette.setColor(QPalette::Base, Qt::red);
        q_table->setPalette(palette);
        q_table->setFocus();

        return false;
    } else {
        palette.setColor(QPalette::Base, q_color);
        q_table->setPalette(palette);
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

    bool newItem = (property("option").toInt() == GlobalDefine::optNew);
    // ����
    int remains = 0;
    int offset = 0;
    // ʣ����ó���
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

    if (remains >= offset) {
        if (newItem && remains < 0) {   // �������������Ҫ�ж�ʣ�೤��
            q_edtStatus->setText(QStringLiteral("�滮���ݳ���Ԥ���ܳ��ȣ�"));

            return false;
        }
    } else {
        q_edtStatus->setText(QStringLiteral("�滮���ݳ���Ԥ���ܳ��ȣ�"));

        return false;
    }

    return true;
}

// ��������ֵ������
void BitMapUI::updateTable(int newCount, bool behind)
{
    if (!q_table) {
        return;
    }
    int count = q_table->rowCount();
    // ����ж����ɾ��
    if (newCount < count) {    // ɾ������
        if (behind) {   // ��ֹλ�����ɾ����󲿷�����
            q_table->removeRow(newCount, count - newCount);
        } else {    // ��ʼλ�����ɾ����ǰ��������
            q_table->removeRow(0, count - newCount);
        }
    } else {    // ��ȫ����
        int offset = 0; // ƫ����
        int insertCount = newCount - count; // ����������
        if (behind) {
            offset = count; // ��ֹλ�����ƫ����count
        }
        _Eigenvalue data;
        /*data.describe = QStringLiteral("����")*/;
        for (int i = 0; i < insertCount; ++i) {
            updateMapOne(i + offset, data);
        }
        // ���µ�һ������
        q_table->setItemValue(0, 0, q_data->start());
    }
}

// ��bitMap�����һ������
void BitMapUI::updateMapOne(int index, const _Eigenvalue &data, int option)
{
    int column = 0;

    if (GlobalDefine::optNew == option) {
        q_table->insertRow(index);
    } else if (GlobalDefine::optEdit == option) {
        if (index > q_table->rowCount() - 1) {
            return;
        }
    }
    QStringList lst = data.describe.split(":");
    QStringList desc = lst.size() > 1 ? lst.last().split(";") : QStringList();
    q_table->setItemValue(index, column, q_spinStart->value() + index);
    q_table->setItemValue(index, ++column, lst.first());
    if (!desc.empty()) {
        q_table->setItemValue(index, ++column, desc.first());
        q_table->setItemValue(index, ++column, desc.last());
    }
}

// ��ȡ����ֵ������
std::map<double, std::string> BitMapUI::tableValues() const
{
    std::map<double, std::string> result;
    if (!q_table) {
        return result;
    }
    int column = 0;
    QString des;
    double value;
    const int count = q_table->rowCount();
    for (int i = 0; i < count; ++i) {
        value = q_table->itemValue(i, column).toInt();
        des = q_table->itemValue(i, ++column).toString().trimmed();
        des.append(":").append(q_table->itemValue(i, ++column).toString().trimmed());
        des.append(";").append(q_table->itemValue(i, ++column).toString().trimmed());
        if (des != QString(":;")) {
            result[value] = des.toStdString();
        }
        column = 0;
    }

    return result;
}
