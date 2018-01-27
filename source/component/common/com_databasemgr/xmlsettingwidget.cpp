#include "precomp.h"
#include "xmlsettingwidget.h"

XmlSettingWidget::XmlSettingWidget(IJAttempter &attempter, QWidget *parent)
    : d_attempter(attempter)
    , QWidget(parent)
    , d_valid(true)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("XML����"), this);
    groupBox->setAlignment(Qt::AlignCenter);
    vertLayoutMain->addWidget(groupBox);

    QVBoxLayout *vertLayoutGroupBox = new QVBoxLayout(groupBox);
    QHBoxLayout *horiLayoutPart = new QHBoxLayout();
    horiLayoutPart->addWidget(new QLabel(QStringLiteral("�ļ���"), this));
    horiLayoutPart->addWidget(d_file = new QLineEdit(this));
    QPushButton *btnScan = new QPushButton("...", this);
    horiLayoutPart->addWidget(btnScan);
    vertLayoutGroupBox->addLayout(horiLayoutPart);
    vertLayoutGroupBox->addStretch();

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutGroupBox->addLayout(horiLayoutButtons);

    horiLayoutButtons->addStretch();

    d_buttonApply = new QPushButton(QStringLiteral("ȷ��"), this);
    d_buttonApply->setFixedSize(120, 30);
    d_buttonApply->setToolTip(QStringLiteral("ȷ���޸ĺ�����<Ӧ��>�Ը�������"));
    horiLayoutButtons->addWidget(d_buttonApply);

    horiLayoutButtons->addSpacing(10);

    d_buttonCancel = new QPushButton(QStringLiteral("ȡ��"), this);
    d_buttonCancel->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(d_buttonCancel);

    horiLayoutButtons->addSpacing(15);

    connect(d_file, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_buttonApply, SIGNAL(clicked()),
            this, SLOT(slotApplyButtonClicked()));
    connect(d_buttonCancel, &QPushButton::clicked, this, [=](){
        initUi();
        enableButtons(false);
    });
    connect(btnScan, SIGNAL(clicked()),
            this, SLOT(slotScanButtonClicked()));

    //
    d_buttonApply->setVisible(false);
    d_buttonCancel->setVisible(false);
}

// ��ʼ����������
bool XmlSettingWidget::initUi()
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

    TiXmlNode *node = root->FirstChild("icddata");
    if (!node) {
        return false;
    }
    if (!(node = node->FirstChild("file"))) {
        return false;
    }

    TiXmlElement *element = node->FirstChildElement("path");
    if (!element) {
        return false;
    }

    //
    QString path = QString(element->GetText()).trimmed();
    path = QString::fromStdString(jframeFacade()->parsePath(path.toStdString()));
    d_file->setText(path);

    enableButtons(false);

    return true;
}

bool XmlSettingWidget::dataValid()
{
    slotApplyButtonClicked();

    return d_valid;
}

QString XmlSettingWidget::xmlFile() const
{
    return d_file->text().trimmed();
}

void XmlSettingWidget::slotApplyButtonClicked()
{
    QFile file(d_file->text().trimmed());
    if (!file.exists()) {
        d_valid = false;
        QMessageBox::warning(this, QStringLiteral("����"),
                             QStringLiteral("�ļ������ڣ�"));
    } else {
        updateXml();
        enableButtons(false);
        d_valid = true;

        emit configChanged();
    }
}

void XmlSettingWidget::slotScanButtonClicked()
{
    QString file = QFileDialog::getOpenFileName(
                this, QStringLiteral("ѡ���ļ�"), d_file->text().trimmed(),
                "XML files (*.xml);;"
                "JSON files (*.json)");
    if (!file.isEmpty()) {
        d_file->setText(file);
    }
}

// ����xml��Ϣ
bool XmlSettingWidget::updateXml()
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

    TiXmlNode *node = root->FirstChild("icddata");
    if (!node) {
        return false;
    }
    if (!(node = node->FirstChild("file"))) {
        return false;
    }

    TiXmlElement *element = node->FirstChildElement("path");
    if (!element) {
        return false;
    }
    TiXmlText textNode("");
    QString value = d_file->text().trimmed();
    value.replace(QString::fromStdString(jframeFacade()->appDirPath()), QStringLiteral("@AppDir@"));

    TiXmlNode *attributeNode = element->FirstChild();
    if (!attributeNode) {
        attributeNode = element->InsertEndChild(textNode);
    }
    attributeNode->SetValue(value.toStdString());

    return doc.SaveFile(file);
}

void XmlSettingWidget::enableButtons(bool enabled)
{
    d_buttonApply->setEnabled(enabled);
    d_buttonCancel->setEnabled(enabled);
    if (enabled) {
        emit configChanged();
    }
}
