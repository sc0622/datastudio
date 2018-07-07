#include "precomp.h"
#include "JDownloadObject.h"
#include "qtftp/qftp.h"

namespace icdmeta {

// class JFolderInfo

JFolderInfo::JFolderInfo(QObject *parent)
    : QObject(parent)
    , d_size(0)
    , d_isDownloadFile(false)
{

}

JFolderInfo::~JFolderInfo()
{

}

QStringList JFolderInfo::filePaths()
{
    return d_filePaths;
}

const QStringList &JFolderInfo::filePaths() const
{
    return d_filePaths;
}

int JFolderInfo::count() const
{
    return d_filePaths.count();
}

qint64 JFolderInfo::size() const
{
    return d_size;
}

bool JFolderInfo::isDownloadFile() const
{
    return d_isDownloadFile;
}

bool JFolderInfo::isNull() const
{
    return (d_filePaths.isEmpty() || d_size == 0);
}

void JFolderInfo::clear()
{
    d_filePaths.clear();
    d_size = 0;
}

void JFolderInfo::appendFilePath(const QString &filePath)
{
    d_filePaths.append(filePath);
}

void JFolderInfo::setSize(qint64 value)
{
    if (value != d_size) {
        d_size = value;
        emit sizeChanged(value);
    }
}

void JFolderInfo::setIsDownloadFile(bool value)
{
    if (value != d_isDownloadFile) {
        d_isDownloadFile = value;
        emit isDownloadFileChanged(value);
    }
}

// class JDownloadObject

JDownloadObject::JDownloadObject(QObject *parent)
    : QThread(parent)
    , d_ftp(nullptr)
    , d_folderInfo(new JFolderInfo(this))
    , d_filePathIndex(-1)
    , d_file(nullptr)
    , d_readBytes(0)
    , d_downloadedSize(0)
    , d_totalSize(0)
    , d_timerUpdate(0)
    , d_isCanceled(false)
{
    connect(this, &JDownloadObject::finished, this, [=](){
        enableFtpConnect(false);
        d_eventLoop.quit();
    });
}

JDownloadObject::~JDownloadObject()
{

}

void JDownloadObject::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JDownloadObject);

    //
}

QFtp *JDownloadObject::ftp() const
{
    return d_ftp;
}

void JDownloadObject::setFtp(QFtp *ftp)
{
    if (isRunning()) {
        quit();
        wait(10000);
    }

    d_ftp = ftp;
}

QString JDownloadObject::currentFile() const
{
    const QStringList &filePaths = d_folderInfo->filePaths();
    if (d_filePathIndex < 0 || d_filePathIndex >= filePaths.count()) {
        return QString();
    } else {
        return filePaths[d_filePathIndex];
    }
}

int JDownloadObject::totalCount() const
{
    return d_folderInfo->count();
}

int JDownloadObject::currentIndex() const
{
    return d_filePathIndex;
}

bool JDownloadObject::isCanceled() const
{
    return d_isCanceled;
}

qint64 JDownloadObject::totalSize() const
{
    if (d_folderInfo->size() > 0) {
        return d_folderInfo->size();
    } else {
        return d_totalSize;
    }
}

qint64 JDownloadObject::downloadedSize() const
{
    return d_downloadedSize;
}

void JDownloadObject::reset()
{
    if (d_timerUpdate > 0) {
        killTimer(d_timerUpdate);
        d_timerUpdate = 0;
    }
    //
    enableFtpConnect(false);
    //
    d_downloadPath.clear();
    d_saveDir.clear();
    d_currentFile.clear();
    d_folderInfo->clear();
    d_rootDir.reset();
    d_currentDir.reset();
    d_eventLoop.quit();
    setCurrentIndex(-1);
    if (d_file) {
        d_file->remove();
        d_file->deleteLater();
        d_file = nullptr;
    }
    d_readBytes = 0;
    d_downloadedSize = 0;
    d_totalSize = 0;
}

bool JDownloadObject::result() const
{
    if (d_folderInfo->isNull()) {
        return false;
    }

    return true;
}

bool JDownloadObject::startDownload(const QString &downloadPath, bool isDownloadFile,
                                   const QString &saveDir, bool blocking)
{
    if (!d_ftp) {
        return false;
    }

    if (!QDir(saveDir).exists()) {
        return false;
    }

    reset();

    d_isCanceled = false;
    d_downloadPath = downloadPath;
    d_saveDir = saveDir;
    d_folderInfo->setIsDownloadFile(isDownloadFile);

    enableFtpConnect(true);

    d_timerUpdate = startTimer(100);

    start();

    if (blocking) {
        d_eventLoop.exec();
        //
        if (d_timerUpdate > 0) {
            emit dataTransferProgress(d_downloadedSize, d_totalSize);
            killTimer(d_timerUpdate);
            d_timerUpdate = 0;
        }

        enableFtpConnect(false);

        const bool result = this->result();
        if (!result) {
            reset();
            return false;
        }
    }

    return true;
}

void JDownloadObject::cancelDownload()
{
    d_isCanceled = true;
    quit();
    wait(10000);
    reset();
}

void JDownloadObject::onCommandFinished(int commandId, bool error)
{
    if (!d_ftp) {
        return;
    }

    //qDebug() << d_ftp->currentCommand();

    Q_UNUSED(commandId);
    switch (d_ftp->currentCommand()) {
    case QFtp::List:
    {
        if (error) {
            qDebug() << d_ftp->errorString();
            Q_ASSERT(false);    //TODO
            break;
        }

        while (d_currentDir != d_rootDir && !d_currentDir->hasChild()) {
            JDirLinkPtr &parentDir = d_currentDir->parent;
            if (!parentDir) {
                Q_ASSERT(false);
                break;
            }
            JDirLinkPtrArray &children = parentDir->children;
            children.erase(std::remove(children.begin(), children.end(), d_currentDir));
            d_currentDir = d_currentDir->parent;
        }

        if (d_currentDir->hasChild()) {
            d_currentDir = d_currentDir->firstChild();
            d_ftp->list(QString::fromLatin1(d_currentDir->name.toLocal8Bit()));
        } else {
            //
            const bool result = this->result();
            if (result) {
                //
                d_totalSize = d_folderInfo->size();
                //
                emit totalCountChanged(d_folderInfo->count());
                // download
                d_readBytes = 0;
                if (!downloadNextFile(0)) {
                    d_folderInfo->clear();
                    quit();
                    break;
                }
            } else {
                quit();
                break;
            }
        }
        break;
    }
    case QFtp::Get:
    {
        if (!d_file) {
            break;
        }
        if (error) {
            qCritical() << "get error:" << d_ftp->errorString();
            d_file->close();
            d_file->remove();
            d_folderInfo->clear();
            quit();
            break;
        } else {
            d_file->close();
            d_file->deleteLater();
            d_file = nullptr;
            //
            int filePathIndex = d_filePathIndex + 1;
            if (filePathIndex >= d_folderInfo->count()) {
                quit();
                break;
            }
            //
            d_readBytes = 0;
            if (!downloadNextFile(d_filePathIndex + 1)) {
                d_folderInfo->clear();
                quit();
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

void JDownloadObject::onListInfo(const QUrlInfo &info)
{
    const QString currentDir = d_currentDir->name + '/';
    if (info.isDir()) {
        JDirLinkPtr chid(new JDirLink(currentDir + info.name()));
        chid->parent = d_currentDir;
        d_currentDir->appendChild(chid);
    } else if (info.isFile()) {
        d_folderInfo->appendFilePath(currentDir+ info.name());
        d_folderInfo->setSize(d_folderInfo->size() + info.size());
    }
}

void JDownloadObject::onDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    if (d_folderInfo->isDownloadFile()) {
        d_downloadedSize = readBytes;
        d_totalSize = totalBytes;
        //emit dataTransferProgress(readBytes, totalBytes);
    } else {
        d_downloadedSize += (readBytes - d_readBytes);
        d_readBytes = readBytes;
        //emit dataTransferProgress(d_downloadedSize, d_JFolderInfo->size());
    }
}

void JDownloadObject::setCurrentIndex(int index)
{
    if (index != d_filePathIndex) {
        d_filePathIndex = index;
        emit currentIndexChanged(index);
    }
}

void JDownloadObject::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d_timerUpdate) {
        emit dataTransferProgress(d_downloadedSize, d_totalSize);
        emit currentFileChanged(d_currentFile);
        emit currentIndexChanged(d_filePathIndex);
    }
}

void JDownloadObject::run()
{
    if (d_folderInfo->isDownloadFile()) {
        d_folderInfo->appendFilePath(d_downloadPath);

        emit totalCountChanged(d_folderInfo->count());

        // download
        if (!downloadNextFile(0)) {
            d_eventLoop.quit();
            emit finished(false);
            return;
        }
    } else {
        if (!queryJFolderInfo(d_downloadPath)) {
            d_eventLoop.quit();
            emit finished(false);
            return;
        }
    }

    exec();

    d_eventLoop.quit();

    const bool result = this->result();
    if (!result) {
        //
    }
    emit finished(result);
}

void JDownloadObject::enableFtpConnect(bool enabled)
{
    if (!d_ftp) {
        return;
    }

    //
    disconnect(d_ftp, &QFtp::listInfo, this, &JDownloadObject::onListInfo);
    disconnect(d_ftp, &QFtp::commandFinished, this, &JDownloadObject::onCommandFinished);
    disconnect(d_ftp, &QFtp::dataTransferProgress, this, &JDownloadObject::onDataTransferProgress);
    //
    if (enabled) {
        connect(d_ftp, &QFtp::listInfo, this, &JDownloadObject::onListInfo);
        connect(d_ftp, &QFtp::commandFinished, this, &JDownloadObject::onCommandFinished);
        connect(d_ftp, &QFtp::dataTransferProgress, this, &JDownloadObject::onDataTransferProgress);
    }
}

bool JDownloadObject::downloadNextFile(int index)
{
    if (!d_ftp) {
        return false;
    }

    //setCurrentIndex(index);
    d_filePathIndex = index;

    const QStringList &filePaths = d_folderInfo->filePaths();
    if (index < 0 || index >= filePaths.count()) {
        return false;
    }

    const QString &filePath = filePaths[index];
    QFileInfo fileInfo(d_saveDir + '/' + filePath);
    QDir _dir(fileInfo.absoluteDir());
    if (!_dir.exists()) {
        if (!_dir.mkpath(fileInfo.absolutePath())) {
            return false;
        }
    }

    d_file = new QFile(fileInfo.absoluteFilePath());
    if (!d_file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        reset();
        return false;
    }

    d_currentFile = filePath;
    //emit currentFileChanged(filePath);

    d_ftp->get(QString::fromLatin1(filePath.toLocal8Bit()), d_file);

    return true;
}

bool JDownloadObject::queryJFolderInfo(const QString &downloadPath)
{
    if (!d_ftp || d_ftp->state() != QFtp::LoggedIn) {
        quit();
        return false;
    }

    d_rootDir = JDirLinkPtr(new JDirLink(downloadPath));
    d_currentDir = d_rootDir;

    d_ftp->list(QString::fromLatin1(downloadPath.toLocal8Bit()));

    return true;
}

}
