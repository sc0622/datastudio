#ifndef JFILECHANNEL_H
#define JFILECHANNEL_H

#include "JSuperChannel.h"

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JSerialChannel)

class JFileChannelPrivate;

class ICDMETA_EXPORT JFileChannel : public JSuperChannel
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(int openMode READ openMode WRITE setOpenMode NOTIFY openModeChanged)
    Q_PROPERTY(int saveFormat READ saveFormat WRITE setSaveFormat NOTIFY saveFormatChanged)
    Q_PROPERTY(uint fileHeader READ fileHeader WRITE setFileHeader NOTIFY fileHeaderChanged)
public:
    enum OpenMode {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 0x0004,
        Truncate = 0x0008,
        Text = 0x0010,
        Unbuffered = 0x0020
    };
    Q_ENUM(OpenMode)

    enum SaveFormat {
        SaveFormatDomain = 0x01,
        SaveFormatTimestamp = 0x02,
        SaveFormatNormal = SaveFormatDomain | SaveFormatTimestamp
    };
    Q_ENUM(SaveFormat)

    explicit JFileChannel(const QString &identity, QObject *parent = nullptr);
    JFileChannel(const QString &identity, const QString &filePath, QObject *parent = nullptr);
    ~JFileChannel();

    static void registerQmlType();

    virtual ChannelType channelType() const;

    QString filePath() const;
    int openMode() const;
    int saveFormat() const;
    uint fileHeader() const;

signals:
    void filePathChanged(const QString &filePath);
    void openModeChanged(int openMode);
    void saveFormatChanged(int saveFormat);
    void fileHeaderChanged(uint fileHeader);

public slots:
    void setFilePath(const QString &filePath);
    void setOpenMode(int openMode);
    void setSaveFormat(int saveFormat);
    void setFileHeader(uint fileHeader);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    Q_DISABLE_COPY(JFileChannel)
    J_DECLARE_PRIVATE(JFileChannel)
};

}

#endif // JFILECHANNEL_H
