#include "precomp.h"
#include "JFtpModel.h"
#include "JFtpTableModel.h"
#include "JDownloadObject.h"

namespace icdmeta {

// class JFtpModelPrivate

class JFtpModelPrivate
{
public:
    JFtpModelPrivate(JFtpModel *q)
        : J_QPTR(q)
        , host("127.0.0.1")
        , port(21)
        , userName("admin")
        , password("admin")
        , ftpTableModel(nullptr)
        , sortModel(nullptr)
        , ftp(nullptr)
        , wait(false)
        , downloadingRow(-1)
        , downloading(false)
        , downloadingCount(0)
        , downloadingIndex(-1)
        , downloadObject(nullptr)
        , timerIdNoop(0)
    {
        saveAsDir = QCoreApplication::applicationDirPath()
                .append("/data");
    }

    void init();
    void callDownloadCallback(bool success);
    static QString generateSizeText(qint64 size);

private:
    J_DECLARE_PUBLIC(JFtpModel)
    QString host;
    int port;
    QString userName;
    QString password;
    JFtpTableModel *ftpTableModel;
    JFtpTableSortModel *sortModel;
    QFtp *ftp;
    QJSValue downloadCallback;
    QString currentPath;
    bool wait;
    QString saveAsDir;
    int downloadingRow;
    bool downloading;
    QString downloadingFile;
    int downloadingCount;
    int downloadingIndex;
    JDownloadObject *downloadObject;
    int timerIdNoop;
    const int timerInternal = 10000;
};

void JFtpModelPrivate::init()
{
    Q_Q(JFtpModel);
    ftpTableModel = new JFtpTableModel(q);
    downloadObject = new JDownloadObject(q);

    //
    QObject::connect(q, &JFtpModel::currentPathChanged,
                     q, &JFtpModel::rootDirChanged);
}

void JFtpModelPrivate::callDownloadCallback(bool success)
{
    if (downloadCallback.isCallable()) {
        //
        QJSValue _downloadCallback = downloadCallback;
        downloadCallback = QJSValue();
        //
        QJSValueList args;
        // success
        args.append(success);
        _downloadCallback.call(args);
    }
}

QString JFtpModelPrivate::generateSizeText(qint64 size)
{
    qreal newSize = size;
    QString unit;
    int prec = 0;

    if (size < 1024L) {
        unit = QStringLiteral(" B");
    } else if (size < 1024L * 1024L) {
        newSize /= 1024L;
        unit = QStringLiteral(" KB");
        prec = 2;
    } else if (size < 1024L * 1024L * 1024L) {
        newSize /= 1024L * 1024L;
        unit = QStringLiteral(" MB");
        prec = 2;
    } else {
        newSize /= 1024L * 1024L * 1024L;
        unit = QStringLiteral(" GB");
        prec = 2;
    }

    return QLocale(QLocale::English, QLocale::UnitedStates)
            .toString(newSize, 'f', prec) + " " + unit;
}

// class JFtpModel

JFtpModel::JFtpModel(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JFtpModelPrivate(this))
{
    Q_D(JFtpModel);
    d->init();

    //
    //d->timerIdNoop = startTimer(d->timerInternal);    // 定时发送noop指令，防止FTP自动断开连接
}

JFtpModel::~JFtpModel()
{
    Q_D(JFtpModel);
    delete d;
}

void JFtpModel::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JFtpModel);

    //
    IcdMetaRegisterUncreatableType2(QFtp);
    JFtpTableModel::registerQmlType();
    JDownloadObject::registerQmlType();
}

QString JFtpModel::host() const
{
    Q_D(const JFtpModel);
    return d->host;
}

int JFtpModel::port() const
{
    Q_D(const JFtpModel);
    return d->port;
}

QString JFtpModel::userName() const
{
    Q_D(const JFtpModel);
    return d->userName;
}

QString JFtpModel::password() const
{
    Q_D(const JFtpModel);
    return d->password;
}

icdmeta::JFtpTableModel *JFtpModel::ftpTableModel() const
{
    Q_D(const JFtpModel);
    return d->ftpTableModel;
}

QFtp *JFtpModel::ftp() const
{
    Q_D(const JFtpModel);
    return d->ftp;
}

bool JFtpModel::isConnected() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return false;
    }

    switch (d->ftp->state()) {
    case QFtp::Unconnected:
    case QFtp::HostLookup:
    case QFtp::Connecting:
        return false;
    default:
        return true;
    }
}

bool JFtpModel::isRootDir() const
{
    Q_D(const JFtpModel);
    if (d->currentPath.isEmpty() || d->currentPath == "/") {
        return true;
    } else {
        return false;
    }
}

QString JFtpModel::currentPath() const
{
    Q_D(const JFtpModel);
    return d->currentPath;
}

bool JFtpModel::isWait() const
{
    Q_D(const JFtpModel);
    return d->wait;
}

QString JFtpModel::saveAsDir() const
{
    Q_D(const JFtpModel);
    return d->saveAsDir;
}

int JFtpModel::downloadingRow() const
{
    Q_D(const JFtpModel);
    return d->downloadingRow;
}

bool JFtpModel::isDownloading() const
{
    Q_D(const JFtpModel);
    return d->downloading;
}

QString JFtpModel::downloadingFile() const
{
    Q_D(const JFtpModel);
    return d->downloadingFile;
}

int JFtpModel::downloadingCount() const
{
    Q_D(const JFtpModel);
    return d->downloadingCount;
}

int JFtpModel::downloadingIndex() const
{
    Q_D(const JFtpModel);
    return d->downloadingIndex;
}

int JFtpModel::setProxy(const QString &host, quint16 port)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->setProxy(host, port);
}

int JFtpModel::connectToHost(const QString &host, quint16 port)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->connectToHost(host, port);
}

int JFtpModel::login(const QString &user, const QString &password)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->login(user, password);
}

int JFtpModel::close()
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }

    return d->ftp->close();
}

int JFtpModel::setTransferMode(int mode)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->setTransferMode((QFtp::TransferMode)mode);
}

int JFtpModel::list(const QString &dir)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->list(dir);
}

int JFtpModel::cd(const QString &dir)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->cd(dir);
}

int JFtpModel::get(const QString &file, QIODevice *dev, int type)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->get(file, dev, (QFtp::TransferType)type);
}

int JFtpModel::put(const QByteArray &data, const QString &file, int type)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->put(data, file, (QFtp::TransferType)type);
}

int JFtpModel::put(QIODevice *dev, const QString &file, int type)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->put(dev, file, (QFtp::TransferType)type);
}

int JFtpModel::remove(const QString &file)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->remove(file);
}

int JFtpModel::mkdir(const QString &dir)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->mkdir(dir);
}

int JFtpModel::rmdir(const QString &dir)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->rmdir(dir);
}

int JFtpModel::rename(const QString &oldname, const QString &newname)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->rename(oldname, newname);
}

int JFtpModel::rawCommand(const QString &command)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->rawCommand(command);
}

qint64 JFtpModel::bytesAvailable() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return 0;
    }
    return d->ftp->bytesAvailable();
}

qint64 JFtpModel::read(char *data, qint64 maxlen)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return 0;
    }
    return d->ftp->read(data, maxlen);
}

QByteArray JFtpModel::readAll()
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return QByteArray();
    }
    return d->ftp->readAll();
}

int JFtpModel::currentId() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->currentId();
}

int JFtpModel::currentCommand() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return QFtp::None;
    }
    return d->ftp->currentCommand();
}

bool JFtpModel::hasPendingCommands() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return false;
    }
    return d->ftp->hasPendingCommands();
}

void JFtpModel::clearPendingCommands()
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }
    d->ftp->clearPendingCommands();
}

int JFtpModel::state() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return QFtp::Unconnected;
    }
    return d->ftp->state();
}

int JFtpModel::error() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return QFtp::NotConnected;
    }
    return d->ftp->error();
}

QString JFtpModel::errorString() const
{
    Q_D(const JFtpModel);
    if (!d->ftp) {
        return QStringLiteral("NotConnected");
    }
    return d->ftp->errorString();
}

bool JFtpModel::connectToHost(QJSValue callback)
{
    Q_D(JFtpModel);
    return connectToHost(d->host, d->port, d->userName, d->password, callback);
}

bool JFtpModel::connectToHost(const QString &host, int port, const QString &userName,
                             const QString &password, QJSValue callback)
{
    Q_D(JFtpModel);
    setWait(true);
    if (d->ftp) {
        d->downloadObject->setFtp(nullptr);
        d->ftp->abort();
        d->ftp->close();
        d->ftp->disconnect(this);
        d->ftp->deleteLater();
        d->ftp = Q_NULLPTR;
        //
        emit stateChanged(QFtp::Unconnected);
    }

    d->ftp = new QFtp(this);
    d->downloadObject->setFtp(d->ftp);
    setDownloadingRow(-1);

    //
    connect(d->ftp, &QFtp::stateChanged, this, &JFtpModel::stateChanged);
    connect(d->ftp, &QFtp::readyRead, this, &JFtpModel::readyRead);
    connect(d->ftp, &QFtp::rawCommandReply, this, &JFtpModel::rawCommandReply);
    connect(d->ftp, &QFtp::done, this, &JFtpModel::done);
    connect(this, &JFtpModel::stateChanged, this, &JFtpModel::onStateChanged);

    enabledFtpConnect(true);

    if (d->currentPath.isEmpty()) {
        d->currentPath = '/';
    }

    QUrl url(host);
    url.setPath(d->currentPath);
    url.setPort(port);
    url.setUserName(userName);
    url.setPassword(password);
    if (!url.isValid() || url.scheme().toLower() != QStringLiteral("ftp")) {
        d->ftp->connectToHost(host, url.port(21));
        d->ftp->login();
    } else {
        d->ftp->connectToHost(url.host(), url.port(21));
        if (url.userName().isEmpty()) {
            d->ftp->login();
        } else {
            d->ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()),
                          url.password());
        }
    }

    //
    d->currentPath = url.path();
    emit currentPathChanged();

    //
    if (callback.isCallable()) {
        QJSValueList args;
        // success
        args.append(true);
        callback.call(args);
    }

    return true;
}

void JFtpModel::disconnectFromHost(QJSValue callback)
{
    Q_D(JFtpModel);
    if (d->ftp) {
        setWait(false);
        //
        d->downloadObject->setFtp(nullptr);
        //
        d->ftpTableModel->clearContents();
        //
        d->ftp->abort();
        d->ftp->close();
        d->ftp->disconnect(this);
        d->ftp->deleteLater();
        d->ftp = Q_NULLPTR;
        //
        emit stateChanged(QFtp::Unconnected);
    }

    setDownloadingRow(-1);
    //d->currentPath.clear();
    //emit currentPathChanged();

    //
    if (callback.isCallable()) {
        QJSValueList args;
        // success
        args.append(true);
        callback.call(args);
    }
}

void JFtpModel::processRowClicked(int row)
{
    Q_UNUSED(row);
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }
}

void JFtpModel::processRowDoubleClicked(int row)
{
    Q_D(JFtpModel);
    if (row == 0 && !isRootDir()) {
        cdToParent();
    } else {
        if (!d->ftp) {
            return;
        }
        const QString name = d->ftpTableModel
                ->data(row, JFtpTableModel::NameRole).toString();
        const bool isDir = d->ftpTableModel
                ->data(row, JFtpTableModel::IsDirRole).toBool();
        const bool isFile = d->ftpTableModel
                ->data(row, JFtpTableModel::IsFileRole).toBool();
        if (isDir) {
            if (!d->currentPath.endsWith("/")) {
                d->currentPath.append("/");
            }
            setDownloadingRow(-1);
            d->currentPath += name;
            emit currentPathChanged();
            d->ftpTableModel->clearContents();
            addToParentRow();
            d->ftp->cd(QString::fromLatin1(name.toLocal8Bit()));
            setWait(true);
        } else if (isFile) {
            //
        }
    }
}

void JFtpModel::cdToParent()
{
    if (isRootDir()) {
        return;
    }
    setWait(true);
    Q_D(JFtpModel);
    setDownloadingRow(-1);
    d->currentPath = d->currentPath.section("/", 0, -2);
    if (d->currentPath.isEmpty()) {
        d->currentPath = "/";
    }
    d->ftp->cd(QString::fromLatin1(d->currentPath.toLocal8Bit()));
    //
    emit currentPathChanged();
}

bool JFtpModel::downloadFile(int row, QJSValue callback)
{
    Q_D(JFtpModel);
    return downloadFile(row, d->saveAsDir, callback);
}

bool JFtpModel::downloadFile(int row, const QString &dir, QJSValue callback)
{
    return downloadPath(row, true, dir, callback);
}

bool JFtpModel::downloadFolder(int row, QJSValue callback)
{
    Q_D(JFtpModel);
    return downloadFolder(row, d->saveAsDir, callback);
}

bool JFtpModel::downloadFolder(int row, const QString &dir, QJSValue callback)
{
    return downloadPath(row, false, dir, callback);
}

bool JFtpModel::downloadPath(int row, bool isDownloadFile, const QString &dir, QJSValue callback)
{
    Q_D(JFtpModel);
    d->downloadCallback = callback;

    const QString name = d->ftpTableModel
            ->data(row, JFtpTableModel::NameRole).toString();
    if (name.isEmpty()) {
        d->callDownloadCallback(false);
        return false;
    }

    QString path = dir;
    if (path.isEmpty()) {
        path = QCoreApplication::applicationDirPath().append("/data");
    }

    QDir _dir(path);
    if (!_dir.exists()) {
        _dir.mkpath(path);
    }
    if (!_dir.exists()) {
        d->callDownloadCallback(false);
        return false;
    }

    setDownloadingRow(row);
    setDownloading(true);
    setDownloadingCount(0);
    setDownloadingIndex(-1);
    enabledFtpConnect(false);
    d->downloadObject->disconnect();
    this->disconnect(d->downloadObject);

    connect(d->downloadObject, &JDownloadObject::dataTransferProgress,
            this, &JFtpModel::dataTransferProgress, Qt::UniqueConnection);
    connect(d->downloadObject, &JDownloadObject::currentFileChanged,
            this, [=](const QString &filePath){
        setDownloadingFile(filePath);
    }, Qt::UniqueConnection);
    connect(d->downloadObject, &JDownloadObject::totalCountChanged, this, [=](int count){
        setDownloadingCount(count);
    }, Qt::UniqueConnection);
    connect(d->downloadObject, &JDownloadObject::currentIndexChanged, this, [=](int index){
        setDownloadingIndex(index);
    }, Qt::UniqueConnection);
    connect(this, &JFtpModel::canceledDownload, this, [=](){
        d->downloadObject->cancelDownload();
    });

    d->downloadObject->startDownload(name, isDownloadFile, path, true);  // blocking
    d->downloadObject->disconnect();
    this->disconnect(d->downloadObject);

    enabledFtpConnect(true);
    setDownloading(false);
    //setDownloadingRow(-1);
    setDownloadingCount(0);
    setDownloadingIndex(-1);
    setDownloadingFile(QString());

    const bool result = d->downloadObject->result();

    d->callDownloadCallback(result);

    if (d->downloadObject->isCanceled()) {
        //
        disconnectFromHost();
        //
        connectToHost();
    }

    return result;
}

bool JFtpModel::downloadRow(int row, QJSValue callback)
{
    Q_D(JFtpModel);
    return downloadRow(row, d->saveAsDir, callback);
}

bool JFtpModel::downloadRow(int row, const QString &dir, QJSValue callback)
{
    Q_D(JFtpModel);
    const bool isDir = d->ftpTableModel
            ->data(row, JFtpTableModel::IsDirRole).toBool();
    return downloadPath(row, !isDir, dir, callback);
}

void JFtpModel::cancelDownload()
{
    emit canceledDownload();
}

QString JFtpModel::getSaveDir() const
{
    Q_D(const JFtpModel);
    return QFileDialog::getExistingDirectory(Q_NULLPTR, tr("Save As"), d->saveAsDir);
}

void JFtpModel::setHost(const QString &host)
{
    Q_D(JFtpModel);
    if (host != d->host) {
        d->host = host;
        emit hostChanged(host);
    }
}

void JFtpModel::setPort(int port)
{
    Q_D(JFtpModel);
    if (port != d->port) {
        d->port = port;
        emit portChanged(port);
    }
}

void JFtpModel::setUserName(const QString &userName)
{
    Q_D(JFtpModel);
    if (userName != d->userName) {
        d->userName = userName;
        emit userNameChanged(userName);
    }
}

void JFtpModel::setPassword(const QString &password)
{
    Q_D(JFtpModel);
    if (password != d->password) {
        d->password = password;
        emit passwordChanged(password);
    }
}

void JFtpModel::abort()
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }
    d->ftp->abort();
}

void JFtpModel::onStateChanged(int state)
{
    switch (state) {
    case QFtp::Unconnected:
    case QFtp::HostLookup:
    case QFtp::Connecting:
        emit connectedChanged(false);
        break;
    default:
        emit connectedChanged(true);
        break;
    }
}

void JFtpModel::onCommandStarted(int commandId)
{
    Q_UNUSED(commandId);
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }

    if (d->ftp->currentCommand() != QFtp::RawCommand) {
        if (d->timerIdNoop > 0) {
            killTimer(d->timerIdNoop);
            d->timerIdNoop = 0;
            QCoreApplication::removePostedEvents(this, QEvent::Timer);
        }
    }
}

void JFtpModel::onCommandFinished(int commandId, bool error)
{
    setWait(false);
    Q_UNUSED(commandId);
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }

    // start timer
    if (d->ftp->currentCommand() != QFtp::RawCommand) {
        if (d->timerIdNoop == 0) {
            //d->timerIdNoop = startTimer(d->timerInternal);
        }
    }

    switch (d->ftp->currentCommand()) {
    case QFtp::ConnectToHost:
    {
        if (error) {
            disconnectFromHost();
        }
        break;
    }
    case QFtp::Login:
    {
        if (error) {
            qDebug() << "login error:" << d->ftp->errorString();
            break;
        }
        if (d->currentPath.isEmpty()) {
            d->currentPath = "/";
        }
        d->ftp->cd(QString::fromLatin1(d->currentPath.toLocal8Bit()));
        break;
    }
    case QFtp::List:
    {
        if (error) {
            qDebug() << "list error:" << d->ftp->errorString();
            break;
        }
        break;
    }
    case QFtp::Cd:
    {
        if (error) {
            qDebug() << "cd error:" << d->ftp->errorString();
            break;
        }
        d->ftpTableModel->clearContents();
        addToParentRow();
        d->ftp->list();
        break;
    }
    default:
        break;
    }
}

void JFtpModel::onListInfo(const QUrlInfo &item)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }

    JFtpTableModel *model = d->ftpTableModel;
    //
    int rowIndex = model->rowCount();
    if (!model->insertRow(rowIndex)) {
        return;
    }
    //
    model->setData(rowIndex, item.name(), JFtpTableModel::NameRole);
    model->setData(rowIndex, item.permissions(), JFtpTableModel::PermissionsRole);
    if (item.isFile()) {
        model->setData(rowIndex, d->generateSizeText(item.size()), JFtpTableModel::SizeRole);
    }
    model->setData(rowIndex, item.lastModified().toString("yyyy/MM/dd hh:mm:ss"),
                   JFtpTableModel::LastModifiedRole);
    model->setData(rowIndex, item.lastRead().toString("yyyy/MM/dd hh:mm:ss"),
                   JFtpTableModel::LastReadRole);
    model->setData(rowIndex, item.owner(), JFtpTableModel::OwnerRole);
    model->setData(rowIndex, item.group(), JFtpTableModel::GroupRole);
    model->setData(rowIndex, item.isDir(), JFtpTableModel::IsDirRole);
    model->setData(rowIndex, item.isFile(), JFtpTableModel::IsFileRole);
    model->setData(rowIndex, item.isSymLink(), JFtpTableModel::IsSymLinkRole);
    model->setData(rowIndex, item.isWritable(), JFtpTableModel::IsWritableRole);
    model->setData(rowIndex, item.isReadable(), JFtpTableModel::IsReadableRole);
    model->setData(rowIndex, item.isExecutable(), JFtpTableModel::IsExcutableRole);
    //
    d->ftpTableModel->setSort(isRootDir());
}

void JFtpModel::onDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }
    Q_UNUSED(readBytes);
    Q_UNUSED(totalBytes);
    //qDebug() << readBytes << totalBytes;
}

void JFtpModel::setSaveAsDir(const QString &value)
{
    Q_D(JFtpModel);
    if (value != d->saveAsDir) {
        d->saveAsDir = value;
        emit saveAsDirChanged(value);
    }
}

void JFtpModel::reset()
{
    cancelDownload();
    disconnectFromHost();
}

void JFtpModel::setWait(bool value)
{
    Q_D(JFtpModel);
    if (d->wait != value) {
#ifndef QT_NO_CURSOR
        if (value) {
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        } else {
            QGuiApplication::restoreOverrideCursor();
        }
#endif
        d->wait = value;
        emit waitChanged(value);
    }
}

void JFtpModel::setDownloadingRow(int value)
{
    Q_D(JFtpModel);
    if (value != d->downloadingRow) {
        d->downloadingRow = value;
        emit downloadingRowChanged(value);
    }
}

void JFtpModel::setDownloading(bool value)
{
    Q_D(JFtpModel);
    if (value != d->downloading) {
        d->downloading = value;
        emit downloadingChanged(value);
    }
}

void JFtpModel::setDownloadingFile(const QString &value)
{
    Q_D(JFtpModel);
    if (value != d->downloadingFile) {
        d->downloadingFile = value;
        emit downloadingFileChanged(value);
    }
}

void JFtpModel::setDownloadingCount(int count)
{
    Q_D(JFtpModel);
    if (count != d->downloadingCount) {
        d->downloadingCount = count;
        emit downloadingCountChanged(count);
    }
}

void JFtpModel::setDownloadingIndex(int index)
{
    Q_D(JFtpModel);
    if (index != d->downloadingIndex) {
        d->downloadingIndex = index;
        emit downloadingIndexChanged(index);
    }
}

void JFtpModel::timerEvent(QTimerEvent *event)
{
    Q_D(JFtpModel);
    if (event->timerId() == d->timerIdNoop) {
        if (d->ftp) {
            if (!d->downloading && !d->ftp->hasPendingCommands()) {
                d->ftp->rawCommand("quote noop");
            }
        }
    } else {
        QObject::timerEvent(event);
    }
}

void JFtpModel::addToParentRow()
{
    Q_D(JFtpModel);
    if (isRootDir()) {
        return;
    }
    if (!d->ftpTableModel->insertRow(0)) {
        return;
    }
    d->ftpTableModel->setData(0, tr("Parent Folder"),
                              JFtpTableModel::NameRole);
    d->ftpTableModel->setData(0, true, JFtpTableModel::IsDirRole);
}

void JFtpModel::enabledFtpConnect(bool enabled)
{
    Q_D(JFtpModel);
    if (!d->ftp) {
        return;
    }
    disconnect(d->ftp, &QFtp::listInfo, this, &JFtpModel::listInfo);
    disconnect(d->ftp, &QFtp::listInfo, this, &JFtpModel::onListInfo);
    disconnect(d->ftp, &QFtp::commandStarted, this, &JFtpModel::commandStarted);
    disconnect(d->ftp, &QFtp::commandFinished, this, &JFtpModel::commandFinished);
    disconnect(d->ftp, &QFtp::commandFinished, this, &JFtpModel::onCommandStarted);
    disconnect(d->ftp, &QFtp::commandFinished, this, &JFtpModel::onCommandFinished);
    disconnect(d->ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
               this, SIGNAL(dataTransferProgress(qint64,qint64)));
    disconnect(d->ftp, &QFtp::dataTransferProgress, this, &JFtpModel::onDataTransferProgress);
    if (enabled) {
        connect(d->ftp, &QFtp::commandStarted, this, &JFtpModel::commandStarted);
        connect(d->ftp, &QFtp::commandFinished, this, &JFtpModel::commandFinished);
        connect(d->ftp, &QFtp::listInfo, this, &JFtpModel::listInfo);
        connect(d->ftp, &QFtp::listInfo, this, &JFtpModel::onListInfo);
        connect(d->ftp, &QFtp::commandFinished, this, &JFtpModel::onCommandStarted);
        connect(d->ftp, &QFtp::commandFinished, this, &JFtpModel::onCommandFinished);
        connect(d->ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
                this, SIGNAL(dataTransferProgress(qint64,qint64)), Qt::UniqueConnection);
        connect(d->ftp, &QFtp::dataTransferProgress,
                this, &JFtpModel::onDataTransferProgress, Qt::UniqueConnection);
    }
}

}
