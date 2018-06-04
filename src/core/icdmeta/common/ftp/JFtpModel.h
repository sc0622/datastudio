#ifndef JFtpModel_H
#define JFtpModel_H

#include "../../icdmeta_global.h"
#include <QAbstractTableModel>

class QFtp;
class QJSValue;

namespace icdmeta {

class JFtpModelPrivate;
class JFtpTableModel;

class ICDMETA_EXPORT JFtpModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(icdmeta::JFtpTableModel* ftpTableModel READ ftpTableModel NOTIFY ftpTableModelChanged)
    Q_PROPERTY(QFtp* ftp READ ftp NOTIFY ftpChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool rootDir READ isRootDir NOTIFY rootDirChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(bool wait READ isWait NOTIFY waitChanged)
    Q_PROPERTY(QString saveAsDir READ saveAsDir WRITE setSaveAsDir NOTIFY saveAsDirChanged)
    Q_PROPERTY(int downloadingRow READ downloadingRow WRITE setDownloadingRow NOTIFY downloadingRowChanged)
    Q_PROPERTY(bool downloading READ isDownloading WRITE setDownloading NOTIFY downloadingChanged)
    Q_PROPERTY(QString downloadingFile READ downloadingFile WRITE setDownloadingFile NOTIFY downloadingFileChanged)
    Q_PROPERTY(int downloadingCount READ downloadingCount WRITE setDownloadingCount NOTIFY downloadingCountChanged)
    Q_PROPERTY(int downloadingIndex READ downloadingIndex WRITE setDownloadingIndex NOTIFY downloadingIndexChanged)
public:
    explicit JFtpModel(QObject *parent = nullptr);
    ~JFtpModel();

    static void registerQmlType();

    QString host() const;
    int port() const;
    QString userName() const;
    QString password() const;

    icdmeta::JFtpTableModel *ftpTableModel() const;
    QFtp *ftp() const;
    bool isConnected() const;
    bool isRootDir() const;
    QString currentPath() const;
    bool isWait() const;
    QString saveAsDir() const;
    int downloadingRow() const;
    bool isDownloading() const;
    QString downloadingFile() const;
    int downloadingCount() const;
    int downloadingIndex() const;

    Q_INVOKABLE int setProxy(const QString &host, quint16 port);
    Q_INVOKABLE int connectToHost(const QString &host, quint16 port=21);
    Q_INVOKABLE int login(const QString &user = QString(), const QString &password = QString());
    Q_INVOKABLE int close();
    Q_INVOKABLE int setTransferMode(int mode);
    Q_INVOKABLE int list(const QString &dir = QString());
    Q_INVOKABLE int cd(const QString &dir);
    int get(const QString &file, QIODevice *dev=0, int type = QFtp::Binary);
    int put(const QByteArray &data, const QString &file, int type = QFtp::Binary);
    int put(QIODevice *dev, const QString &file, int type = QFtp::Binary);
    Q_INVOKABLE int remove(const QString &file);
    Q_INVOKABLE int mkdir(const QString &dir);
    Q_INVOKABLE int rmdir(const QString &dir);
    Q_INVOKABLE int rename(const QString &oldname, const QString &newname);

    Q_INVOKABLE int rawCommand(const QString &command);

    Q_INVOKABLE qint64 bytesAvailable() const;
    qint64 read(char *data, qint64 maxlen);
    QByteArray readAll();

    Q_INVOKABLE int currentId() const;
    Q_INVOKABLE int currentCommand() const;
    Q_INVOKABLE bool hasPendingCommands() const;
    Q_INVOKABLE void clearPendingCommands();

    Q_INVOKABLE int state() const;

    Q_INVOKABLE int error() const;
    Q_INVOKABLE QString errorString() const;

    Q_INVOKABLE bool connectToHost(QJSValue callback = QJSValue());
    Q_INVOKABLE bool connectToHost(const QString &host, int port,
                                   const QString &userName, const QString &password,
                                   QJSValue callback = QJSValue());
    Q_INVOKABLE void disconnectFromHost(QJSValue callback = QJSValue());
    Q_INVOKABLE void processRowClicked(int row);
    Q_INVOKABLE void processRowDoubleClicked(int row);
    Q_INVOKABLE void cdToParent();
    Q_INVOKABLE bool downloadFile(int row, QJSValue callback);
    Q_INVOKABLE bool downloadFile(int row, const QString &dir, QJSValue callback);
    Q_INVOKABLE bool downloadFolder(int row, QJSValue callback);
    Q_INVOKABLE bool downloadFolder(int row, const QString &dir, QJSValue callback);
    Q_INVOKABLE bool downloadPath(int row, bool isDownloadFile, const QString &dir,
                                  QJSValue callback);
    Q_INVOKABLE bool downloadRow(int row, QJSValue callback);
    Q_INVOKABLE bool downloadRow(int row, const QString &dir, QJSValue callback);
    Q_INVOKABLE void cancelDownload();
    Q_INVOKABLE QString getSaveDir() const;

signals:
    void hostChanged(const QString &);
    void portChanged(int);
    void userNameChanged(const QString &);
    void passwordChanged(const QString &);
    void ftpTableModelChanged();
    void ftpChanged();
    void connectedChanged(bool);
    void rootDirChanged();
    void currentPathChanged();
    void waitChanged(bool value);
    void saveAsDirChanged(const QString &value);
    void downloadingRowChanged(int value);
    void downloadingChanged(bool downloading);
    void downloadingFileChanged(const QString &filePath);
    void downloadingCountChanged(int count);
    void downloadingIndexChanged(int index);
    void canceledDownload();
    //
    void stateChanged(int state);
    void listInfo(const QUrlInfo& item);
    void readyRead();
    void dataTransferProgress(qint64 readBytes, qint64 totalBytes);
    void rawCommandReply(int replyCode, const QString& detail);

    void commandStarted(int commandId);
    void commandFinished(int commandId, bool error);
    void done(bool error);

public slots:
    void setHost(const QString &host);
    void setPort(int port);
    void setUserName(const QString &userName);
    void setPassword(const QString &password);
    void abort();
    void onStateChanged(int state);
    void onCommandStarted(int commandId);
    void onCommandFinished(int commandId, bool error);
    void onListInfo(const QUrlInfo& item);
    void onDataTransferProgress(qint64 readBytes, qint64 totalBytes);
    void setSaveAsDir(const QString &value);

    void reset();

private slots:
    void setWait(bool value);
    void setDownloadingRow(int value);
    void setDownloading(bool value);
    void setDownloadingFile(const QString &value);
    void setDownloadingCount(int count);
    void setDownloadingIndex(int index);

protected:
    void timerEvent(QTimerEvent *event);

private:
    void addToParentRow();
    void enabledFtpConnect(bool enabled);

private:
    Q_DISABLE_COPY(JFtpModel)
    J_DECLARE_PRIVATE(JFtpModel)
};

}

#endif // JFtpModel_H
