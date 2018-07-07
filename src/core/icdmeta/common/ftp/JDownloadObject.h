#ifndef JDownloadObject_H
#define JDownloadObject_H

#include <QThread>
#include <QUrl>
#include "qtftp/qurlinfo.h"
#include "../../icdmeta_global.h"

class QFtp;
class QEventLoop;
class QFile;

namespace icdmeta {

// class JFolderInfo

class ICDMETA_EXPORT JFolderInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool isDownloadFile READ isDownloadFile WRITE setIsDownloadFile NOTIFY isDownloadFileChanged)
public:
    explicit JFolderInfo(QObject *parent = nullptr);
    ~JFolderInfo();

    Q_INVOKABLE QStringList filePaths();
    const QStringList &filePaths() const;
    int count() const;
    qint64 size() const;
    bool isDownloadFile() const;

    bool isNull() const;
    void clear();

    void appendFilePath(const QString &filePath);

signals:
    void sizeChanged(qint64);
    void countChanged(int);
    void isDownloadFileChanged(bool);

public slots:
    void setSize(qint64 value);
    void setIsDownloadFile(bool value);

private:
    QStringList d_filePaths;
    qint64 d_size;
    bool d_isDownloadFile;
};

struct JDirLink;
typedef std::shared_ptr<JDirLink> JDirLinkPtr;
typedef std::vector<JDirLinkPtr> JDirLinkPtrArray;

struct JDirLink
{
    QString name;
    JDirLinkPtr parent;
    JDirLinkPtrArray children;

    JDirLink() {}
    explicit JDirLink(const QString &name) : name(name) {}

    void appendChild(const JDirLinkPtr &dir)
    { children.push_back(dir); }

    bool hasChild() const { return (children.size() > 0); }
    JDirLinkPtr firstChild() const { return *children.cbegin(); }
};

// class JDownloadObject

class ICDMETA_EXPORT JDownloadObject : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    enum Status {
        InvalidStatus = -1,
        QueryFolderStatus,
        DownloadingSatus
    };
    Q_ENUM(Status)

    explicit JDownloadObject(QObject *parent = nullptr);
    ~JDownloadObject();

    static void registerQmlType();

    QFtp *ftp() const;
    void setFtp(QFtp *ftp);

    QString currentFile() const;
    int totalCount() const;
    int currentIndex() const;
    bool isCanceled() const;

    qint64 totalSize() const;
    qint64 downloadedSize() const;

    void reset();
    bool result() const;
    bool startDownload(const QString &downloadPath, bool isDownloadFile,
                       const QString &saveDir, bool blocking);

signals:
    void finished(bool result);
    void dataTransferProgress(qint64 readBytes, qint64 totalBytes);
    void currentFileChanged(const QString &filePath);
    void totalCountChanged(int);
    void currentIndexChanged(int);
    void statusChanged(int status);

public slots:
    void cancelDownload();

private slots:
    void onCommandFinished(int commandId, bool error);
    void onListInfo(const QUrlInfo &info);
    void onDataTransferProgress(qint64 readBytes, qint64 totalBytes);

    void setCurrentIndex(int index);

protected:
    void timerEvent(QTimerEvent *event) override;
    void run() override;

private:
    void enableFtpConnect(bool enabled);
    bool downloadNextFile(int index);
    bool queryJFolderInfo(const QString &downloadPath = QString());
    bool downloadFile();
    bool downloadFolder();

private:
    QFtp *d_ftp;
    JFolderInfo *d_folderInfo;
    JDirLinkPtr d_rootDir;
    JDirLinkPtr d_currentDir;
    QEventLoop d_eventLoop;
    QString d_downloadPath;
    QString d_saveDir;
    QString d_currentFile;
    int d_filePathIndex;
    QFile *d_file;
    qint64 d_readBytes;
    qint64 d_downloadedSize;
    qint64 d_totalSize;
    int d_timerUpdate;
    bool d_isCanceled;
};

}

#endif // JDownloadObject_H
