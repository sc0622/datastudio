#include "precomp.h"
#include "DataSourceConfigDlg.h"

// class FileSourceWidget

FileSourceWidget::FileSourceWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QFormLayout *formLayoutTop = new QFormLayout();
    formLayoutTop->setLabelAlignment(Qt::AlignRight);
    vertLayoutMain->addLayout(formLayoutTop);

    QHBoxLayout *horiLayoutFilePath = new QHBoxLayout();
    horiLayoutFilePath->setSpacing(0);
    formLayoutTop->addRow(tr("File path:"), horiLayoutFilePath);

    d_editFilePath = new QLineEdit(this);
    horiLayoutFilePath->addWidget(d_editFilePath);

    QPushButton *buttonView = new QPushButton(QIcon(":/datastudio/image/global/view.png"),
                                              QString(), this);
    buttonView->setFixedSize(35, d_editFilePath->minimumSizeHint().height());
    horiLayoutFilePath->addWidget(buttonView);

    connect(d_editFilePath, &QLineEdit::textChanged, this, &FileSourceWidget::contentChanged);
    connect(buttonView, &QPushButton::clicked, this, [=](){
        const QString filePath = QFileDialog::getOpenFileName(
                    this, tr("Select file"), d_editFilePath->text(),
                    "JSON file (*.json);;XML file (*.xml)").trimmed();
        if (filePath.isEmpty()) {
            return;
        }
        d_editFilePath->setText(filePath);
    });
}

QString FileSourceWidget::filePath() const
{
    return d_editFilePath->text().trimmed();
}

void FileSourceWidget::focusFilePath()
{
    d_editFilePath->setFocus();
    d_editFilePath->selectAll();
}

void FileSourceWidget::setFilePath(const QString &filePath)
{
    d_editFilePath->setText(filePath);
}

// class SqlSourceWidget

SqlSourceWidget::SqlSourceWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QFormLayout *formLayoutTop = new QFormLayout();
    formLayoutTop->setLabelAlignment(Qt::AlignRight);
    vertLayoutMain->addLayout(formLayoutTop);

    d_comboBoxSqlType = new QComboBox(this);
    d_comboBoxSqlType->addItem("SQL Server", "SQL Server");
    d_comboBoxSqlType->addItem("Oracle", "Oracle");
    formLayoutTop->addRow(tr("SQL type:"), d_comboBoxSqlType);

    d_editServerName = new QLineEdit(this);
    formLayoutTop->addRow(tr("Server name:"), d_editServerName);

    d_editDatabaseName = new QLineEdit(this);
    formLayoutTop->addRow(tr("Database name:"), d_editDatabaseName);

    d_editUserName = new QLineEdit(this);
    formLayoutTop->addRow(tr("User name:"), d_editUserName);

    d_editPassword = new QLineEdit(this);
    d_editPassword->setEchoMode(QLineEdit::Password);
    formLayoutTop->addRow(tr("Password:"), d_editPassword);

    connect(d_comboBoxSqlType, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, [=](int index){
        Q_UNUSED(index);
        emit contentChanged();
    });
    connect(d_editServerName, &QLineEdit::textChanged, this, [=](){
        emit contentChanged();
    });
    connect(d_editDatabaseName, &QLineEdit::textChanged, this, [=](){
        emit contentChanged();
    });
    connect(d_editUserName, &QLineEdit::textChanged, this, [=](){
        emit contentChanged();
    });
    connect(d_editPassword, &QLineEdit::textChanged, this, [=](){
        emit contentChanged();
    });
}

QString SqlSourceWidget::sqlType() const
{
    return d_comboBoxSqlType->currentData().toString();
}

QString SqlSourceWidget::serverName() const
{
    return d_editServerName->text().trimmed();
}

QString SqlSourceWidget::databaseName() const
{
    return d_editDatabaseName->text().trimmed();
}

QString SqlSourceWidget::userName() const
{
    return d_editUserName->text().trimmed();
}

QString SqlSourceWidget::password() const
{
    return d_editPassword->text().trimmed();
}

void SqlSourceWidget::focusSqlType()
{
    d_comboBoxSqlType->setFocus();
}

void SqlSourceWidget::focusServerName()
{
    d_editServerName->setFocus();
    d_editServerName->selectAll();
}

void SqlSourceWidget::focusDatabaseName()
{
    d_editDatabaseName->setFocus();
    d_editDatabaseName->selectAll();
}

void SqlSourceWidget::focusUserName()
{
    d_editUserName->setFocus();
    d_editUserName->selectAll();
}

void SqlSourceWidget::focusPassword()
{
    d_editPassword->setFocus();
    d_editPassword->selectAll();
}

void SqlSourceWidget::setSqlType(const QString &sqlType)
{
    if (sqlType == "SQL Server") {
        d_comboBoxSqlType->setCurrentText("SQL Server");
    } else if (sqlType == "Oracle") {
        d_comboBoxSqlType->setCurrentText("Oracle");
    }
}

void SqlSourceWidget::setServrerName(const QString &serverName)
{
    d_editServerName->setText(serverName);
}

void SqlSourceWidget::setDatabaseName(const QString &databaseName)
{
    d_editDatabaseName->setText(databaseName);
}

void SqlSourceWidget::setUserName(const QString &userName)
{
    d_editUserName->setText(userName);
}

void SqlSourceWidget::setPassword(const QString &password)
{
    d_editPassword->setText(password);
}

// class DataSourceConfigDlg

DataSourceConfigDlg::DataSourceConfigDlg(const QString &module, QObject *receiver,
                                         QWidget *parent)
    : QDialog(parent)
    , d_module(module)
    , d_receiver(receiver)
{
    setWindowTitle(tr("Data source configuration"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_tabWidget = new QTabWidget(this);
    vertLayoutMain->addWidget(d_tabWidget);

    d_fileSourceWidget = new FileSourceWidget(this);
    d_tabWidget->addTab(d_fileSourceWidget,
                        QIcon(":/datastudio/image/global/file.png"), tr("File source"));

    d_sqlSourceWidget = new SqlSourceWidget(this);
    d_tabWidget->addTab(d_sqlSourceWidget,
                        QIcon(":/datastudio/image/global/sql.png"), tr("SQL source"));

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    d_buttonConnect = new QPushButton(tr("Connect"), this);
    d_buttonConnect->setMinimumWidth(80);
    horiLayoutBottom->addWidget(d_buttonConnect);

    horiLayoutBottom->addSpacing(20);

    d_buttonOk = new QPushButton(tr("Ok"), this);
    d_buttonOk->setMinimumWidth(80);
    d_buttonOk->setDefault(true);
    horiLayoutBottom->addWidget(d_buttonOk);

    QPushButton *buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonCancel->setMinimumWidth(80);
    horiLayoutBottom->addWidget(buttonCancel);

    horiLayoutBottom->addSpacing(10);

    vertLayoutMain->addSpacing(10);

    connect(d_tabWidget, &QTabWidget::currentChanged, this, [=](int index){
        updateTab(index);
        d_buttonOk->setEnabled(true);
    });
    connect(d_fileSourceWidget, &FileSourceWidget::contentChanged, this, [=](){
        d_buttonOk->setEnabled(true);
    });
    connect(d_sqlSourceWidget, &SqlSourceWidget::contentChanged, this, [=](){
        d_buttonOk->setEnabled(true);
    });
    connect(d_buttonOk, &QPushButton::clicked, this, [=](){
        if (!saveConfig()) {
            return;
        }
        accept();
        // notify
        jnotify->post(d_module + ".parser.changed", d_receiver);
    });
    connect(buttonCancel, &QPushButton::clicked, this, [=](){
        reject();
    });

    resize(600, sizeHint().height());
    updateTab(d_tabWidget->currentIndex());
    d_buttonOk->setDisabled(true);

    if (!restoreConfig()) {
        //
    }
}

void DataSourceConfigDlg::updateTab(int index)
{
    switch (index) {
    case DataSourceFile:
    {
        d_buttonConnect->hide();
        break;
    }
    case DataSourceSql:
    {
        d_buttonConnect->show();
        break;
    }
    default:
        break;
    }
}

bool DataSourceConfigDlg::saveConfig()
{
    Json::Value config;

    switch (d_tabWidget->currentIndex()) {
    case DataSourceFile:
    {
        // source type
        config["sourceType"] = "file";
        // file path
        const QString filePath = d_fileSourceWidget->filePath();
        if (filePath.isEmpty()) {
            d_fileSourceWidget->focusFilePath();
            return false;
        }
        if (!QFile::exists(filePath)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("File \"%1\" is not exists!").arg(filePath));
            d_fileSourceWidget->focusFilePath();
            return false;
        }
        config["filePath"] = filePath.toStdString();

        break;
    }
    case DataSourceSql:
    {
        // source type
        config["sourceType"] = "sql";
        // sql type
        const QString sqlType = d_sqlSourceWidget->sqlType();
        if (sqlType.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("SQL type cannot be empty!"));
            d_sqlSourceWidget->focusSqlType();
            return false;
        }
        config["sqlType"] = sqlType.toStdString();
        // server's name
        const QString serverName = d_sqlSourceWidget->serverName();
        if (serverName.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Server's name cannot be empty!"));
            d_sqlSourceWidget->focusServerName();
            return false;
        }
        config["serverName"] = serverName.toStdString();
        // database's name
        const QString databaseName = d_sqlSourceWidget->databaseName();
        if (databaseName.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Database's name cannot be empty!"));
            d_sqlSourceWidget->focusDatabaseName();
            return false;
        }
        config["databaseName"] = databaseName.toStdString();
        // user's name
        const QString userName = d_sqlSourceWidget->userName();
        if (userName.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("User's name cannot be empty!"));
            d_sqlSourceWidget->focusUserName();
            return false;
        }
        config["userName"] = userName.toStdString();
        // user's password
        const QString password = d_sqlSourceWidget->password();
        if (password.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("User's password cannot be empty!"));
            d_sqlSourceWidget->focusPassword();
            return false;
        }
        config["password"] = password.toStdString();
        break;
    }
    default:
        break;
    }

    if (!JMain::instance()->setOption(d_module, "parser", config)) {
        QMessageBox::warning(this, tr("Warning"), tr("Save config failure!"));
        return false;
    }

    return true;
}

bool DataSourceConfigDlg::restoreConfig()
{
    Json::Value config = JMain::instance()->option(d_module, "parser");
    if (config.isNull()) {
        return false;
    }

    if (!config.isMember("sourceType")) {
        return false;
    }

    const std::string sourceType = config["sourceType"].asString();
    if (sourceType == "file") {
        d_tabWidget->setCurrentIndex(DataSourceFile);
        // file path
        if (config.isMember("filePath")) {
            d_fileSourceWidget->setFilePath(
                        QString::fromStdString(config["filePath"].asString()));
        }
    } else if (sourceType == "sql") {
        d_tabWidget->setCurrentIndex(DataSourceSql);
        // sql type
        if (config.isMember("sqlType")) {
            d_sqlSourceWidget->setSqlType(QString::fromStdString(config["sqlType"].asString()));
        }
        // server's name
        if (config.isMember("serverName")) {
            d_sqlSourceWidget->setServrerName(QString::fromStdString(config["serverName"].asString()));
        }
        // database's name
        if (config.isMember("databaseName")) {
            d_sqlSourceWidget->setDatabaseName(QString::fromStdString(config["databaseName"].asString()));
        }
        // user's name
        if (config.isMember("userName")) {
            d_sqlSourceWidget->setUserName(QString::fromStdString(config["userName"].asString()));
        }
        // user's password
        if (config.isMember("password")) {
            d_sqlSourceWidget->setPassword(QString::fromStdString(config["password"].asString()));
        }
    }

    d_buttonOk->setDisabled(true);

    return true;
}
