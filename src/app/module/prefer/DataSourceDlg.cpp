#include "precomp.h"
#include "DataSourceDlg.h"

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

// class DataSourceWidget

DataSourceWidget::DataSourceWidget(const QString &module, QObject *receiver,
                                   QWidget *parent)
    : QWidget(parent)
    , module_(module)
    , receiver_(receiver)
    , checkApplyToAll_(nullptr)
{
    setWindowTitle(tr("Data source configuration"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    tabWidget_ = new QTabWidget(this);
    vertLayoutMain->addWidget(tabWidget_);

    fileSourceWidget_ = new FileSourceWidget(this);
    tabWidget_->addTab(fileSourceWidget_,
                       QIcon(":/datastudio/image/global/file.png"), tr("File source"));

    sqlSourceWidget_ = new SqlSourceWidget(this);
    tabWidget_->addTab(sqlSourceWidget_,
                       QIcon(":/datastudio/image/global/sql.png"), tr("SQL source"));

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    if (receiver) {
        checkApplyToAll_ = new QCheckBox(tr("Apply to all"), this);
        horiLayoutBottom->addWidget(checkApplyToAll_);
    }

    horiLayoutBottom->addSpacing(20);

    buttonConnect_ = new QPushButton(tr("Connect"), this);
    buttonConnect_->setMinimumWidth(80);
    horiLayoutBottom->addWidget(buttonConnect_);

    horiLayoutBottom->addSpacing(20);

    buttonOk_ = new QPushButton(receiver ? tr("Ok") : tr("Apply"), this);
    buttonOk_->setMinimumWidth(receiver ? 80 : 150);
    buttonOk_->setDefault(true);
    horiLayoutBottom->addWidget(buttonOk_);

    QPushButton *buttonCancel = nullptr;
    if (receiver) {
        buttonCancel = new QPushButton(tr("Cancel"), this);
        buttonCancel->setMinimumWidth(80);
        horiLayoutBottom->addWidget(buttonCancel);
    }

    if (receiver) {
        horiLayoutBottom->addSpacing(10);
    } else {
        horiLayoutBottom->addStretch();
    }

    vertLayoutMain->addSpacing(10);

    connect(tabWidget_, &QTabWidget::currentChanged, this, [=](int index){
        updateTab(index);
        buttonOk_->setEnabled(true);
    });
    connect(fileSourceWidget_, &FileSourceWidget::contentChanged, this, [=](){
        buttonOk_->setEnabled(true);
    });
    connect(sqlSourceWidget_, &SqlSourceWidget::contentChanged, this, [=](){
        buttonOk_->setEnabled(true);
    });
    if (receiver) {
        connect(checkApplyToAll_, &QCheckBox::stateChanged, this, [=](){
            buttonOk_->setEnabled(true);
        });
    }
    connect(buttonOk_, &QPushButton::clicked, this, [=](){
        if (!saveConfig()) {
            return;
        }
        emit accepted();
        // notify
        if (!checkApplyToAll_ || checkApplyToAll_->isChecked()) {
            foreach (auto &module, JMain::modules()) {
                jnotify->post(module + ".parser.changed");
            }
        } else {
            jnotify->post(module_ + ".parser.changed", receiver_);
        }
        buttonOk_->setDisabled(true);
    });
    if (buttonCancel) {
        connect(buttonCancel, &QPushButton::clicked, this, [=](){
            emit rejected();
        });
    }

    updateTab(tabWidget_->currentIndex());
    buttonOk_->setDisabled(true);

    if (!restoreConfig()) {
        //
    }
}

void DataSourceWidget::updateTab(int index)
{
    switch (index) {
    case DataSourceFile:
    {
        buttonConnect_->hide();
        break;
    }
    case DataSourceSql:
    {
        buttonConnect_->show();
        break;
    }
    default:
        break;
    }
}

bool DataSourceWidget::saveConfig()
{
    Json::Value config;

    switch (tabWidget_->currentIndex()) {
    case DataSourceFile:
    {
        // source type
        config["sourceType"] = "file";
        // file path
        const QString filePath = fileSourceWidget_->filePath();
        if (filePath.isEmpty()) {
            fileSourceWidget_->focusFilePath();
            return false;
        }
        if (!QFile::exists(filePath)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("File \"%1\" is not exists!").arg(filePath));
            fileSourceWidget_->focusFilePath();
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
        const QString sqlType = sqlSourceWidget_->sqlType();
        if (sqlType.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("SQL type cannot be empty!"));
            sqlSourceWidget_->focusSqlType();
            return false;
        }
        config["sqlType"] = sqlType.toStdString();
        // server's name
        const QString serverName = sqlSourceWidget_->serverName();
        if (serverName.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Server's name cannot be empty!"));
            sqlSourceWidget_->focusServerName();
            return false;
        }
        config["serverName"] = serverName.toStdString();
        // database's name
        const QString databaseName = sqlSourceWidget_->databaseName();
        if (databaseName.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Database's name cannot be empty!"));
            sqlSourceWidget_->focusDatabaseName();
            return false;
        }
        config["databaseName"] = databaseName.toStdString();
        // user's name
        const QString userName = sqlSourceWidget_->userName();
        if (userName.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("User's name cannot be empty!"));
            sqlSourceWidget_->focusUserName();
            return false;
        }
        config["userName"] = userName.toStdString();
        // user's password
        const QString password = sqlSourceWidget_->password();
        if (password.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("User's password cannot be empty!"));
            sqlSourceWidget_->focusPassword();
            return false;
        }
        config["password"] = password.toStdString();
        break;
    }
    default:
        break;
    }

    if (!checkApplyToAll_ || checkApplyToAll_->isChecked()) {
        foreach (auto &module, JMain::modules()) {
            if (!JMain::instance()->setOption(module, "parser", config)) {
                QMessageBox::warning(this, tr("Warning"), tr("Save config failure!"));
                return false;
            }
        }
    } else {
        if (!JMain::instance()->setOption(module_, "parser", config)) {
            QMessageBox::warning(this, tr("Warning"), tr("Save config failure!"));
            return false;
        }
    }

    return true;
}

bool DataSourceWidget::restoreConfig()
{
    Json::Value config = JMain::instance()->option(module_, "parser");
    if (config.isNull()) {
        return false;
    }

    if (!config.isMember("sourceType")) {
        return false;
    }

    const std::string sourceType = config["sourceType"].asString();
    if (sourceType == "file") {
        tabWidget_->setCurrentIndex(DataSourceFile);
        // file path
        if (config.isMember("filePath")) {
            fileSourceWidget_->setFilePath(
                        QString::fromStdString(config["filePath"].asString()));
        }
    } else if (sourceType == "sql") {
        tabWidget_->setCurrentIndex(DataSourceSql);
        // sql type
        if (config.isMember("sqlType")) {
            sqlSourceWidget_->setSqlType(QString::fromStdString(config["sqlType"].asString()));
        }
        // server's name
        if (config.isMember("serverName")) {
            sqlSourceWidget_->setServrerName(QString::fromStdString(config["serverName"].asString()));
        }
        // database's name
        if (config.isMember("databaseName")) {
            sqlSourceWidget_->setDatabaseName(QString::fromStdString(config["databaseName"].asString()));
        }
        // user's name
        if (config.isMember("userName")) {
            sqlSourceWidget_->setUserName(QString::fromStdString(config["userName"].asString()));
        }
        // user's password
        if (config.isMember("password")) {
            sqlSourceWidget_->setPassword(QString::fromStdString(config["password"].asString()));
        }
    }

    buttonOk_->setDisabled(true);

    return true;
}

// DataSourceDlg

DataSourceDlg::DataSourceDlg(const QString &module, QObject *receiver,
                             QWidget *parent)
    : QDialog(parent)
{
    QHBoxLayout *layoutMain = new QHBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    DataSourceWidget *dataSourceWidget = new DataSourceWidget(module, receiver, this);
    layoutMain->addWidget(dataSourceWidget);

    connect(dataSourceWidget, &DataSourceWidget::accepted, this,
            &DataSourceDlg::accept);
    connect(dataSourceWidget, &DataSourceWidget::rejected, this,
            &DataSourceDlg::reject);

    resize(600, sizeHint().height());
}
