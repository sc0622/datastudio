#include "precomp.h"
#include "icdcomm_filechannel.h"
#include <sstream>
#include <QDir>

namespace Icd {

// class FileChannelData

class FileChannelData
{
    friend class FileChannel;
public:
    FileChannelData()
        : filePath("channel/" + QUuid::createUuid().toString()
                   .remove(QRegExp("[{}-]")).toStdString() + ".icd")
        , openMode(FileChannel::ReadWrite)
        , saveFormat(FileChannel::SaveFormatNormal)
        , fileHeader(0x00444349U)       // "ICD\0"
    {
        file.setFileName(QString::fromStdString(filePath));
    }

    int fileSize(std::fstream &file);
    std::string openmode() const;
    std::string openmode(const std::string &prefix) const;
    std::string saveformat() const;
    std::string saveformat(const std::string &prefix) const;

private:
    std::string filePath;               // 文件路径
    FileChannel::OpenMode openMode;     // 操作模式
    FileChannel::SaveFormat saveFormat; // 操作模式
    unsigned int fileHeader;            // 文件头信息
    std::string domain;                 //
    QFile file;                         // 文件句柄
};

int FileChannelData::fileSize(std::fstream &file)
{
    if (file.is_open()) {
        long size = 0;
        size = file.tellp();
        file.seekp(0, std::ios::end);
        size = (long)file.tellp() - size;
        return (int)size;
    } else {
        return 0;
    }
}

std::string FileChannelData::openmode() const
{
    return openmode("--openMode=");
}

std::string FileChannelData::openmode(const std::string &prefix) const
{
    std::ostringstream os;
    os << prefix;
    if (openMode & FileChannel::ReadOnly) {
        os << "r";
    }
    if (openMode & FileChannel::WriteOnly) {
        os << "w";
    }
    if (openMode & FileChannel::Append) {
        os << "a";
    }
    if (openMode & FileChannel::Truncate) {
        os << "u";
    }
    if (openMode & FileChannel::Text) {
        os << "t";
    }
    if (openMode & FileChannel::Unbuffered) {
        os << "b";
    }
    return os.str();
}

std::string FileChannelData::saveformat() const
{
    return saveformat("--saveFormat=");
}

std::string FileChannelData::saveformat(const std::string &prefix) const
{
    std::ostringstream os;
    os << prefix;
    if (saveFormat & FileChannel::SaveFormatDomain) {
        os << "d";
    }
    if (saveFormat & FileChannel::SaveFormatTimestamp) {
        os << "t";
    }
    return os.str();
}

// class FileChannel

FileChannel::FileChannel()
    : Channel(ChannelFile)
    , d(new FileChannelData())
{

}

FileChannel::FileChannel(const std::string &filePath, OpenMode openMode)
    : Channel(ChannelFile)
    , d(new FileChannelData())
{
    d->filePath = filePath;
    d->openMode = openMode;
}

FileChannel::~FileChannel()
{

}

bool FileChannel::open()
{
    //
    QFileInfo fileInfo(QString::fromStdString(d->filePath));
    QDir dir(fileInfo.path());
    if (!dir.exists()) {
        dir.mkpath(fileInfo.path());
    }

    // in
    if (!d->file.isOpen()) {
        if (!d->file.open((QIODevice::OpenMode)d->openMode)) {
            return false;
        }
    }

    //
    if (d->openMode & ReadOnly) {
        //
    }
    // out
    if (d->openMode & WriteOnly) {
        bool needHeader = true;
        if (d->openMode & FileChannel::Append) {
            if (d->file.size() > 0) {
                needHeader = false;
            }
        } else {
            d->file.resize(0);
        }
        if (needHeader) {
            // marker
            d->file.write((const char*)&d->fileHeader, 4);
            // saveformat
            if (d->saveFormat & FileChannel::SaveFormatDomain) {
                d->file.putChar('d');
            } else {
                d->file.putChar('\0');
            }
            if (d->saveFormat & FileChannel::SaveFormatTimestamp) {
                d->file.putChar('t');
            } else {
                d->file.putChar('\0');
            }
            if (d->saveFormat & FileChannel::SaveFormatDomain) {
                d->file.write(d->domain.c_str(), d->domain.size());
            }
            d->file.putChar('\n');
            d->file.flush();
        }
    }

    return true;
}

void FileChannel::close()
{
    d->file.close();
}

int FileChannel::read(char *buffer, int size)
{
    return (int)d->file.read(buffer, size);
}

int FileChannel::write(const char *buffer, int size)
{
    int _size = 0;
    if (d->file.isOpen()) {
        if (d->saveFormat & FileChannel::SaveFormatTimestamp) {
            const qint64 msecs = QDateTime::currentMSecsSinceEpoch();
            _size = d->file.write((const char*)&msecs, sizeof(msecs));
        }
        return _size + (int)d->file.write(buffer, size);
    } else {
        return -1;
    }
}

int FileChannel::sizeOfIn() const
{
    // in
    if (d->openMode & ReadOnly) {
        if (d->file.isOpen()) {
            return d->file.size();
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

int FileChannel::sizeOfOut() const
{
    return 0;
}

void FileChannel::flush()
{
    // out
    if (d->openMode & WriteOnly) {
        d->file.flush();
    }
}

bool FileChannel::isOpen() const
{
    return d->file.isOpen();
}

// "file: -f [filename] -m [r][w]"
std::string FileChannel::config() const
{
    std::ostringstream os;
    os << "file: --filePath=" << d->filePath << " ";
    // openmode
    os << d->openmode();
    // saveformat
    os << " " << d->saveformat();
    //
    os << " " << Channel::config();

    return os.str();
}

// "file: -f [filename] -m [r][w]"
bool FileChannel::setConfig(const std::string &config)
{
    //
    close();

    // parse
    const std::map<std::string, std::string> items = parseConfig(config);
    if (items.empty()) {
        return false;
    }

    // id
    std::map<std::string, std::string>::const_iterator citer = items.find("id");
    if (citer == items.cend()) {
        return false;
    }
    setIdentity(citer->second);

    // name
    citer = items.find("name");
    if (citer == items.cend()) {
        return false;
    }
    setName(citer->second);

    // filePath
    citer = items.find("filePath");
    if (citer == items.cend()) {
        return false;
    }
    setFilePath(citer->second);

    // openMode
    citer = items.find("openMode");
    if (citer != items.cend()) {
        const std::string openMode = citer->second;
        unsigned int eOpenMode = FileChannel::NotOpen;
        if (openMode.find('r') != std::string::npos) {
            eOpenMode |= FileChannel::ReadOnly;
        }
        if (openMode.find('w') != std::string::npos) {
            eOpenMode |= FileChannel::WriteOnly;
        }
        if (openMode.find('a') != std::string::npos) {
            eOpenMode |= FileChannel::Append;
        }
        if (openMode.find('u') != std::string::npos) {
            eOpenMode |= FileChannel::Truncate;
        }
        if (openMode.find('t') != std::string::npos) {
            eOpenMode |= FileChannel::Text;
        }
        if (openMode.find('b') != std::string::npos) {
            eOpenMode |= FileChannel::Unbuffered;
        }
        setOpenMode((OpenMode)eOpenMode);
    }

    // saveFormat
    citer = items.find("saveFormat");
    if (citer != items.cend()) {
        unsigned int eSaveFormat = 0;
        const std::string saveFormat = citer->second;
        if (saveFormat.find('d') != std::string::npos) {
            eSaveFormat |= FileChannel::SaveFormatDomain;
        }
        if (saveFormat.find('t') != std::string::npos) {
            eSaveFormat |= FileChannel::SaveFormatTimestamp;
        }
        setSaveFormat((SaveFormat)eSaveFormat);
    }

    return true;
}

std::string FileChannel::desc() const
{
    std::ostringstream os;
    os << "" << QFileInfo(QString::fromStdString(d->filePath))
          .absolutePath().toStdString() << ", ";
    // openmode
    os << d->openmode("-m=");
    // saveformat
    os << ", " << d->saveformat("-s=");
    return os.str();
}

std::string FileChannel::filePath() const
{
    return d->filePath;
}

void FileChannel::setFilePath(const std::string &filePath)
{
    if (filePath != d->filePath) {
        //
        bool isOpen = this->isOpen();
        if (isOpen) {
            close();
        }
        // clear old file
        if (d->file.exists()) {
#if 0
            d->file.reset();
            d->file.resize(0);
            d->file.rename(QString::fromStdString(filePath));
#else
            d->file.setFileName(QString::fromStdString(filePath));
#endif
        } else {
            d->file.setFileName(QString::fromStdString(filePath));
        }
        //
        d->filePath = filePath;
        if (isOpen) {
            open();
        }
    }
}

Icd::FileChannel::OpenMode Icd::FileChannel::openMode() const
{
    return d->openMode;
}

void Icd::FileChannel::setOpenMode(Icd::FileChannel::OpenMode openMode)
{
    d->openMode = openMode;
}

FileChannel::SaveFormat FileChannel::saveFormat() const
{
    return d->saveFormat;
}

void FileChannel::setSaveFormat(FileChannel::SaveFormat saveFormat)
{
    d->saveFormat = saveFormat;
}

unsigned int FileChannel::fileHeader() const
{
    return d->fileHeader;
}

void FileChannel::setFileHeader(unsigned int header)
{
    d->fileHeader = header;
}

std::string FileChannel::domain() const
{
    return d->domain;
}

void FileChannel::setDomain(const std::string &domain)
{
    d->domain = domain;
}

int FileChannel::lastErrorCode() const
{
    return 0;
}

std::string FileChannel::lastError() const
{
    return "";
}

} // end of namespace Icd
