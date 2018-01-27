#include "sourcewidget.h"

SourceWidget::SourceWidget(QWidget *parent)
    : QWidget(parent)
    , d_lastIndex(0)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QHBoxLayout *horiLayoutPart = new QHBoxLayout();
    horiLayoutPart->setContentsMargins(0, 0, 0, 0);

    QFormLayout *form = new QFormLayout();
    vertLayoutMain->addLayout(form);

    form->addRow(QStringLiteral("����Դ��"), horiLayoutPart);

    d_box = new QComboBox(this);
    horiLayoutPart->addWidget(d_box);

    d_buttonApply = new QPushButton(QStringLiteral("Ӧ��"), this);
    d_buttonApply->setFixedWidth(80);
    horiLayoutPart->addWidget(d_buttonApply);

    vertLayoutMain->addStretch();

    //
    connect(d_box, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(slotTypeChanged(const QString &)));
    connect(d_buttonApply, SIGNAL(clicked()),
            this, SLOT(slotApplyButtonClicked()));
}

// ��ʼ����������
void SourceWidget::initUi()
{
    QHash<QString, QString> type;
    QString current;
    if (!readSourceType(type, current)) {
        return;
    }
    d_box->clear();

    QHashIterator<QString, QString> it = type;
    while (it.hasNext()) {
        it.next();
        d_box->addItem(it.key(), it.value());
    }
    d_box->setCurrentIndex(d_box->findText(current));
    d_lastIndex = d_box->currentIndex();
    d_buttonApply->setEnabled(false);
}

QString SourceWidget::sourceType() const
{
    return d_box->currentText();
}

void SourceWidget::restore()
{
    d_box->setCurrentIndex(d_lastIndex);
    d_buttonApply->setDisabled(true);
}

void SourceWidget::setApplyState(bool enabled)
{
    if (d_buttonApply) {
        d_buttonApply->setEnabled(enabled);
    }
}

void SourceWidget::slotApplyButtonClicked()
{
    bool result = false;

    emit sourceChanged(result);

    if (result) {
        QMessageBox::information(this, QStringLiteral("����"),
                                 QStringLiteral("�������Դ�ɹ���"));
        d_buttonApply->setDisabled(result);
        updateSoure();
        d_lastIndex = d_box->currentIndex();
    } else {
        restore();
    }
}

void SourceWidget::slotTypeChanged(const QString &text)
{
    d_buttonApply->setEnabled(true);

    emit typeChanged(text);
}

// ��ȡ����Դ��������
bool SourceWidget::readSourceType(QHash<QString, QString> &type,
                                  QString &currentType)
{
    TiXmlDocument doc(jframeFacade()->configDirPath()
                      .append("/jframe_global.xml"));
    if (!doc.LoadFile()) {
        return false;
    }
    TiXmlNode *root = doc.FirstChild("jframe");
    if (!root) {
        return false;
    }
    if (!(root = root->FirstChild("app"))) {
        return false;
    }

    TiXmlElement *element = root->FirstChildElement("icddata");
    if (!element) {
        return false;
    }
    currentType = element->Attribute("name");

    QString itemValue;
    QString itemName;
    element = element->FirstChildElement();
    while (element) {
        itemName = element->Value();
        if (itemName == "file") {
            itemName = QStringLiteral("�ļ�");
        }
        itemValue = element->Attribute("name");
        type[itemName] = itemValue;
        if (!itemValue.isEmpty() && currentType == itemValue) {
            currentType = itemName;
        }
        element = element->NextSiblingElement();
    }

    return true;
}

// ��������Դ
bool SourceWidget::updateSoure()
{
    std::string file = jframeFacade()->configDirPath()
            .append("/jframe_global.xml");
    TiXmlDocument doc(file);
    if (!doc.LoadFile()) {
        return false;
    }
    TiXmlNode *root = doc.FirstChild("jframe");
    if (!root) {
        return false;
    }
    if (!(root = root->FirstChild("app"))) {
        return false;
    }

    TiXmlElement *element = root->FirstChildElement("icddata");
    if (!element) {
        return false;
    }
    element->SetAttribute("name",
                          d_box->currentData().toString().toStdString());

    return doc.SaveFile(file);
}
