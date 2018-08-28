#ifndef DATASOURCEDLG_H
#define DATASOURCEDLG_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QTabWidget;
class QPushButton;
class QCheckBox;

enum DataSourceType {
    DataSourceInvalid = -1,
    DataSourceFile,
    DataSourceSql
};

// class FileSourceWidget

class FileSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileSourceWidget(QWidget *parent = nullptr);

    QString filePath() const;

    void focusFilePath();

signals:
    void contentChanged();

public slots:
    void setFilePath(const QString &filePath);

private:
    QLineEdit *editFilePath_;
};

// class SqlSourceWidget

class SqlSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SqlSourceWidget(QWidget *parent = nullptr);

    QString sqlType() const;
    QString serverName() const;
    QString databaseName() const;
    QString userName() const;
    QString password() const;

    void focusSqlType();
    void focusServerName();
    void focusDatabaseName();
    void focusUserName();
    void focusPassword();

signals:
    void contentChanged();

public slots:
    void setSqlType(const QString &sqlType);
    void setServrerName(const QString &serverName);
    void setDatabaseName(const QString &databaseName);
    void setUserName(const QString &userName);
    void setPassword(const QString &password);

private:
    QComboBox *comboBoxSqlType_;
    QLineEdit *editServerName_;
    QLineEdit *editDatabaseName_;
    QLineEdit *editUserName_;
    QLineEdit *editPassword_;
};

// class DataSourceWidget

class DataSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataSourceWidget(const QString &module, QObject *receiver,
                              QWidget *parent = nullptr);

signals:
    void accepted();
    void rejected();

public slots:

private:
    void updateTab(int index);
    bool saveConfig();
    bool restoreConfig();

private:
    QString module_;
    QObject *receiver_;
    QTabWidget *tabWidget_;
    FileSourceWidget *fileSourceWidget_;
    SqlSourceWidget *sqlSourceWidget_;
    QCheckBox *checkApplyToAll_;
    QPushButton *buttonConnect_;
    QPushButton *buttonOk_;
};

// class DataSourceDlg

class DataSourceDlg : public QDialog
{
    Q_OBJECT
public:
    explicit DataSourceDlg(const QString &module, QObject *receiver,
                           QWidget *parent = nullptr);

signals:

public slots:

private:
};

#endif // DATASOURCECONFIGDLG_H
