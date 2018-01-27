#include "databasesettingwidget.h"

DatabaseSettingWidget::DatabaseSettingWidget(IJAttempter &attempter,
                                             QWidget *parent)
    : d_attempter(attempter)
    , QWidget(parent)
    , d_valid(true)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("���ݿ�����"), this);
    groupBox->setAlignment(Qt::AlignCenter);
    vertLayoutMain->addWidget(groupBox);

    QVBoxLayout *vertLayoutGroupBox = new QVBoxLayout(groupBox);
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    vertLayoutGroupBox->addLayout(formLayout);

    d_driver = new QComboBox(this);
    d_driver->addItems(QStringList() << "SQL SERVER" << "ORACLE");
    formLayout->addRow(QStringLiteral("���ݿ⣺"), d_driver);

    d_server = new QLineEdit(this);
    d_server->setMaxLength(260);
    formLayout->addRow(QStringLiteral("��������"), d_server);

    d_dbLabel = new QLabel(QStringLiteral("���ݿ�����"), this);
    d_database = new QLineEdit(this);
    d_database->setMaxLength(260);
    formLayout->addRow(d_dbLabel, d_database);

    d_user = new QLineEdit(this);
    d_user->setMaxLength(64);
    formLayout->addRow(QStringLiteral("�û�����"), d_user);

    d_password = new QLineEdit(this);
    d_password->setMaxLength(64);
    formLayout->addRow(QStringLiteral("��   �룺"), d_password);

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

    //
    connect(d_driver, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(slotDriverChanged(const QString &)));
    connect(d_server, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_database, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_user, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_password, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_buttonApply, SIGNAL(clicked()),
            this, SLOT(slotApplyButtonClicked()));
    connect(d_buttonCancel, &QPushButton::clicked, this, [=](){
        initUi();
        enableButtons(false);
    });

    //
    d_buttonApply->setVisible(false);
    d_buttonCancel->setVisible(false);
}

// ��ʼ����������
bool DatabaseSettingWidget::initUi()
{
    TiXmlDocument doc(jframeFacade()->configDirPath()
                      .append("/jframe_global.xml"));
    if (!doc.LoadFile()) {
        return false;
    }
    TiXmlNode* root = doc.FirstChild("jframe");
    if (!root) {
        return false;
    }
    if (!(root = root->FirstChild("app"))) {
        return false;
    }

    TiXmlNode *dbNode = root->FirstChild("icddata");
    if (!dbNode) {
        return false;
    }
    if (!(dbNode = dbNode->FirstChild("database"))) {
        return false;
    }
    TiXmlElement*element = dbNode->FirstChildElement("driver");
    if (!element) {
        return false;
    }
    d_driver->setCurrentText(QString(element->GetText()).trimmed());
    if (!(element = element->NextSiblingElement("serverName"))) {
        return false;
    }
    d_server->setText(QString(element->GetText()).trimmed());
    if (!(element = element->NextSiblingElement("database"))) {
        return false;
    }
    d_database->setText(QString(element->GetText()).trimmed());
    if (!(element = element->NextSiblingElement("userName"))) {
        return false;
    }
    d_user->setText(QString(element->GetText()).trimmed());
    if (!(element = element->NextSiblingElement("password"))) {
        return false;
    }
    d_password->setText(QString(element->GetText()).trimmed());

    enableButtons(false);

    return true;
}

bool DatabaseSettingWidget::dataValid()
{
    slotApplyButtonClicked();

    return d_valid;
}

// �������ݿ���Ϣ
bool DatabaseSettingWidget::updateDatabase()
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

    TiXmlNode *dbNode = root->FirstChild("icddata");
    if (!dbNode) {
        return false;
    }
    if (!(dbNode = dbNode->FirstChild("database"))) {
        return false;
    }
    TiXmlElement *element = dbNode->FirstChildElement("driver");
    if (!element) {
        return false;
    }
    TiXmlText textNode("");
    std::string value = d_driver->currentText().trimmed().toStdString();
    TiXmlNode *node = element->FirstChild();
    if (!node) {
        node = element->InsertEndChild(textNode);
    }
    node->SetValue(value);
    if (!(element = element->NextSiblingElement("serverName"))) {
        return false;
    }
    value = d_server->text().trimmed().toStdString();
    if (!(node = element->FirstChild())) {
        node = element->InsertEndChild(textNode);
    }
    node->SetValue(value);
    if (d_database->isVisible()) {
        // ��Ҫ�õ����ݿ����Բű���
        if (!(element = element->NextSiblingElement("database"))) {
            return false;
        }
        value = d_database->text().trimmed().toStdString();
        if (!(node = element->FirstChild())) {
            node = element->InsertEndChild(textNode);
        }
        node->SetValue(value);
    }
    if (!(element = element->NextSiblingElement("userName"))) {
        return false;
    }
    value = d_user->text().trimmed().toStdString();
    if (!(node = element->FirstChild())) {
        node = element->InsertEndChild(textNode);
    }
    node->SetValue(value);
    if (!(element = element->NextSiblingElement("password"))) {
        return false;
    }
    value = d_password->text().trimmed().toStdString();
    if (!(node = element->FirstChild())) {
        node = element->InsertEndChild(textNode);
    }
    node->SetValue(value);

    return doc.SaveFile(file);
}

void DatabaseSettingWidget::enableButtons(bool enabled)
{
    d_buttonApply->setEnabled(enabled);
    d_buttonCancel->setEnabled(enabled);
    if (enabled) {
        emit configChanged();
    }
}

void DatabaseSettingWidget::slotApplyButtonClicked()
{
    QString error;
    if (d_server->text().trimmed().isEmpty()) {
        error = QStringLiteral("����������Ϊ�գ�");
    }
    if (d_database->isVisible()
        && error.isEmpty()
        && d_database->text().trimmed().isEmpty()) {
        error = QStringLiteral("���ݿ�������Ϊ�գ�");
    }
    if (d_user->text().trimmed().isEmpty() && error.isEmpty()) {
        error = QStringLiteral("�û�������Ϊ�գ�");
    }
    if (d_password->text().trimmed().isEmpty() && error.isEmpty()) {
        error = QStringLiteral("���벻��Ϊ�գ�");
    }
    if (!error.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("���ݴ���"), error);
        d_valid = false;
    } else {
        updateDatabase();
        enableButtons(false);
        d_valid = true;

        emit configChanged();
    }
}

void DatabaseSettingWidget::slotDriverChanged(const QString &text)
{
    bool visible = text.contains("SQL SERVER");
    d_dbLabel->setVisible(visible);
    d_database->setVisible(visible);

    enableButtons(true);
}
