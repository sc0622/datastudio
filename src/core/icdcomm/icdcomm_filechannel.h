#ifndef ICDCOMM_FILECHANNEL_H
#define ICDCOMM_FILECHANNEL_H

#include "icdcomm_channel.h"
#include <fstream>

namespace Icd {

class FileChannel;
class FileChannelData;
typedef JHandlePtr<FileChannel> FileChannelPtr;

/**
 * @brief 数据文件通道，用于数据记录或回放
 *        数据文件通道不支持同时读和写操作，必须在初始化时指定是读模式还是写模式
 */
class ICDCOMM_EXPORT FileChannel : public Channel
{
public:
    /**
     * @brief The OpenMode enum
     */
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

    /**
     * @brief The SaveFormat enum
     */
    enum SaveFormat {
        SaveFormatDomain = 0x01,    // 包含表域名
        SaveFormatTimestamp = 0x02, // 包含时间戳
        SaveFormatNormal = SaveFormatDomain | SaveFormatTimestamp
    };

    /**
     * @brief FileChannel
     */
    explicit FileChannel();

    /**
     * @brief 数据文件通道构造函数，默认为读模式
     * @param [in] filePath ：文件路径
     * @param [in] openMode : 操作模式
     */
    explicit FileChannel(const std::string &filePath, OpenMode openMode = ReadWrite);

    virtual ~FileChannel();

    /**
     * @brief 打开数据通道
     * @return true:打开成功,false打开失败
     */
    bool open();

    /**
     * @brief 关闭数据通道
     */
    void close();

    /**
     * @brief 读取数据
     * @param [out] buffer,待读取数据存储缓冲区
     * @param [in] size，缓冲区大小
     * @return 实际读取数据值,如果读取错误则返回-1
     */
    int read(char* buffer, int size);

    /**
     * @brief 写入数据
     * @param [in] buffer : 写入数据缓冲区
     * @param [in] size : 缓冲区大小
     * @return 实际写入数据长度，写入错误返回-1
     */
    int write(const char* buffer, int size);

    /**
     * @brief sizeOfIn
     * @return
     */
    int sizeOfIn() const;

    /**
     * @brief sizeOfOut
     * @return
     */
    int sizeOfOut() const;

    /**
     * @brief flush
     */
    void flush();

    /**
     * @brief isOpen
     * @return
     */
    bool isOpen() const;

    /**
     * @brief config
     * @return "file: -f ["filePath"] -m [r][w]"
     */
    std::string config() const;

    /**
     * @brief setConfig
     * @param config : "file: -f ["filePath"] -m [r][w]"
     * @return
     */
    bool setConfig(const std::string &config);

    /**
     * @brief desc
     * @return
     */
    std::string desc() const;

    /**
     * @brief filePath
     * @return
     */
    std::string filePath() const;

    /**
     * @brief setFilePath
     * @param filePath
     */
    void setFilePath(const std::string &filePath);

    /**
     * @brief openMode
     * @return
     */
    OpenMode openMode() const;

    /**
     * @brief setOpenMode
     * @param openMode
     */
    void setOpenMode(OpenMode openMode);

    /**
     * @brief saveFormat
     * @return
     */
    SaveFormat saveFormat() const;

    /**
     * @brief setSaveFormat
     * @param saveFormat
     */
    void setSaveFormat(SaveFormat saveFormat);

    /**
     * @brief fileHeader
     * @return
     */
    unsigned int fileHeader() const;

    /**
     * @brief setFileHeader
     * @param header
     */
    void setFileHeader(unsigned int header);

    /**
     * @brief domain
     * @return
     */
    std::string domain() const;

    /**
     * @brief setDomain
     * @param domain
     */
    void setDomain(const std::string &domain);

    /**
     * @brief lastErrorCode
     * @return
     */
    int lastErrorCode() const;

    /**
     * @brief lastError
     * @return
     */
    std::string lastError() const;

private:
    JHandlePtr<FileChannelData> d;
};

} // end of namespace Icd

#endif // ICDCOMM_FILECHANNEL_H
