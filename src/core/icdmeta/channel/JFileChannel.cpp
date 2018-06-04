#include "precomp.h"
#include "JFileChannel.h"
#include "icdcomm/icdcomm_filechannel.h"
#include "icdworker/icdworker.h"
#include "icdcore/3rdpart/jsoncpp/json/json.h"

namespace icdmeta {

// class JFileChannelPrivate

class JFileChannelPrivate
{
public:
    JFileChannelPrivate(JFileChannel *q)
        : J_QPTR(q)
    {

    }

    void init();
    Icd::FileChannelPtr channel() const;

private:
    J_DECLARE_PUBLIC(JFileChannel)
};

void JFileChannelPrivate::init()
{
    Q_Q(JFileChannel);
    q->setChannel(Icd::FileChannelPtr(new Icd::FileChannel()));
}

Icd::FileChannelPtr JFileChannelPrivate::channel() const
{
    Q_Q(const JFileChannel);
    return JHandlePtrCast<Icd::FileChannel, Icd::Channel>(q->nativeChannel());
}

}

using namespace icdmeta;

// class JFileChannel

JFileChannel::JFileChannel(QObject *parent)
    : JSuperChannel(parent)
    , J_DPTR(new JFileChannelPrivate(this))
{
    Q_D(JFileChannel);
    d->init();
}

JFileChannel::JFileChannel(const QString &filePath, QObject *parent)
    : JSuperChannel(parent)
    , J_DPTR(new JFileChannelPrivate(this))
{
    Q_D(JFileChannel);
    d->init();
    d->channel()->setFilePath(filePath.toStdString());
}

JFileChannel::~JFileChannel()
{
    Q_D(JFileChannel);
    delete d;
}

void JFileChannel::registerQmlType()
{
    //
    IcdMetaRegisterUncreatableType2(JFileChannel);

    //
}

JSuperChannel::ChannelType JFileChannel::channelType() const
{
    return JSuperChannel::ChannelFile;
}

QString JFileChannel::filePath() const
{
    Q_D(const JFileChannel);
    return QString::fromStdString(d->channel()->filePath());
}

int JFileChannel::openMode() const
{
    Q_D(const JFileChannel);
    return int(d->channel()->openMode());
}

int JFileChannel::saveFormat() const
{
    Q_D(const JFileChannel);
    return int(d->channel()->saveFormat());
}

uint JFileChannel::fileHeader() const
{
    Q_D(const JFileChannel);
    return uint(d->channel()->fileHeader());
}

void JFileChannel::setFilePath(const QString &filePath)
{
    Q_D(JFileChannel);
    Icd::FileChannelPtr channel = d->channel();
    const std::string _filePath = filePath.toStdString();
    if (_filePath != channel->filePath()) {
        channel->setFilePath(_filePath);
        emit filePathChanged(filePath);
    }
}

void JFileChannel::setOpenMode(int openMode)
{
    Q_D(JFileChannel);
    Icd::FileChannelPtr channel = d->channel();
    if (openMode != (int)channel->openMode()) {
        channel->setOpenMode(Icd::FileChannel::OpenMode(openMode));
        emit openModeChanged(openMode);
    }
}

void JFileChannel::setSaveFormat(int saveFormat)
{
    Q_D(JFileChannel);
    Icd::FileChannelPtr channel = d->channel();
    if (saveFormat != (int)channel->saveFormat()) {
        channel->setSaveFormat(Icd::FileChannel::SaveFormat(saveFormat));
        emit saveFormatChanged(saveFormat);
    }
}

void JFileChannel::setFileHeader(uint fileHeader)
{
    Q_D(JFileChannel);
    Icd::FileChannelPtr channel = d->channel();
    if (fileHeader != (uint)channel->fileHeader()) {
        channel->setFileHeader((fileHeader));
        emit fileHeaderChanged(fileHeader);
    }
}

Json::Value JFileChannel::save() const
{
    Json::Value json = JSuperChannel::save();

    Q_D(const JFileChannel);
    Icd::FileChannelPtr channel = d->channel();
    if (!channel) {
        return json;
    }

    Json::Value propertyJson;
    // filePath
    propertyJson["filePath"] = channel->filePath();
    // openMode
    unsigned int openMode = (unsigned int)channel->openMode();
    std::string sOpenMode;
    if (openMode & JFileChannel::ReadOnly) {
        sOpenMode.push_back('r');
    }
    if (openMode & JFileChannel::WriteOnly) {
        sOpenMode.push_back('w');
    }
    if (openMode & JFileChannel::Append) {
        sOpenMode.push_back('a');
    }
    if (openMode & JFileChannel::Truncate) {
        sOpenMode.push_back('u');
    }
    if (openMode & JFileChannel::Text) {
        sOpenMode.push_back('t');
    }
    if (openMode & JFileChannel::Unbuffered) {
        sOpenMode.push_back('b');
    }
    propertyJson["openMode"] = openMode;
    // saveFormat
    unsigned int saveFormat = (unsigned int)channel->saveFormat();
    std::string sSaveFormat;
    if (saveFormat & JFileChannel::SaveFormatDomain) {
        sSaveFormat.push_back('d');
    }
    if (saveFormat & JFileChannel::SaveFormatTimestamp) {
        sSaveFormat.push_back('t');
    }
    propertyJson["saveFormat"] = saveFormat;
    // fileHeader
    propertyJson["fileHeader"] = uint(channel->fileHeader());

    json["property"] = propertyJson;

    return json;
}

bool JFileChannel::restore(const Json::Value &json, int deep)
{
    if (!JSuperChannel::restore(json, deep)) {
        return false;
    }

    Json::Value propertyJson = json["property"];
    if (propertyJson.isNull()) {
        return false;
    }

    Q_D(JFileChannel);
    Icd::FileChannelPtr channel = d->channel();
    if (!channel) {
        return false;
    }
    // filePath
    channel->setFilePath(propertyJson["filePath"].asString());
    // openMode
    if (propertyJson.isMember("openMode")) {
        const std::string sOpenMode = propertyJson["openMode"].asString();
        unsigned int eOpenMode = JFileChannel::NotOpen;
        if (sOpenMode.find('r') != std::string::npos) {
            eOpenMode |= JFileChannel::ReadOnly;
        }
        if (sOpenMode.find('w') != std::string::npos) {
            eOpenMode |= JFileChannel::WriteOnly;
        }
        if (sOpenMode.find('a') != std::string::npos) {
            eOpenMode |= JFileChannel::Append;
        }
        if (sOpenMode.find('u') != std::string::npos) {
            eOpenMode |= JFileChannel::Truncate;
        }
        if (sOpenMode.find('t') != std::string::npos) {
            eOpenMode |= JFileChannel::Text;
        }
        if (sOpenMode.find('b') != std::string::npos) {
            eOpenMode |= JFileChannel::Unbuffered;
        }
        channel->setOpenMode(Icd::FileChannel::OpenMode(eOpenMode));
    }
    // saveFormat
    if (propertyJson.isMember("saveFormat")) {
        const std::string sSaveFormat = propertyJson["saveFormat"].asString();
        unsigned int eSaveFormat = 0;
        if (sSaveFormat.find('d') != std::string::npos) {
            eSaveFormat |= JFileChannel::SaveFormatDomain;
        }
        if (sSaveFormat.find('t') != std::string::npos) {
            eSaveFormat |= JFileChannel::SaveFormatTimestamp;
        }
        channel->setSaveFormat(Icd::FileChannel::SaveFormat(eSaveFormat));
    }
    // fileHeader
    if (propertyJson.isMember("fileHeader")) {
        channel->setFileHeader(propertyJson["fileHeader"].asUInt());
    }

    return true;
}

