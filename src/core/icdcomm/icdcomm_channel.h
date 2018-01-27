#ifndef ICDCOMM_CHANNEL_H
#define ICDCOMM_CHANNEL_H

#include "icdcomm_global.h"
#include <string>
#include <vector>

namespace Icd {

/**
 * @brief The ChannelType enum
 */
enum ChannelType {
    ChannelInvalid = -1,    // 通信类型不可用
    ChannelSerial = 0,      // 串口通信方式
    ChannelUdp,             // UDP通信方式
    ChannelFile,            // 文件方式
    ChannelTotal            // 标记边界
};

class Channel;
class ChannelData;
typedef JHandlePtr<Channel> ChannelPtr;
typedef std::vector<ChannelPtr> ChannelPtrArray;

/**
 * @brief The Channel class
 */
class ICDCOMM_EXPORT Channel
{
public:
    /**
     * @brief Channel
     */
    explicit Channel(ChannelType channelType = ChannelInvalid);

    virtual ~Channel();

    /**
     * @brief createInstance
     * @param channelType
     * @return
     */
    static ChannelPtr createInstance(ChannelType channelType);

    /**
     * @brief 根据通道配置信息创建通道
     *  通道配置信息格式如下：
     *  通道类型 其他信息
     *  如果通道类型为串口(0)，则其他信息格式为：
     *  串口号 波特率 数据位 停止位 校验方式
     *  如果通道类型为UDP，则其他信息格式为：
     *  本地地址 本地端口 目标地址 目标端口
     * @param config : 通道配置信息
     * @return true，新建成功；false：新建失败
     */
    static ChannelPtr createInstance(const std::string &config);

    /**
     * @brief 打开数据通道
     * @return true:打开成功,false打开失败
     */
    virtual bool open() = 0;

    /**
     * @brief 关闭数据通道
     */
    virtual void close() = 0;

    /**
     * @brief waitForReadyRead
     * @param msecs
     * @return
     */
    virtual bool waitForReadyRead(int msecs) { (void)msecs; return false; }

    /**
     * @brief 读取数据
     * @param [out] buffer,待读取数据存储缓冲区
     * @param [in] size，缓冲区大小
     * @return 实际读取数据值,如果读取错误则返回-1
     */
    virtual int read(char* buffer, int size) = 0;

    /**
     * @brief 写入数据
     * @param [in] buffer : 写入数据缓冲区
     * @param [in] size : 缓冲区大小
     * @return 实际写入数据长度，写入错误返回-1
     */
    virtual int write(const char* buffer, int size) = 0;

    /**
     * @brief sizeOfIn
     * @return
     */
    virtual int sizeOfIn() const;

    /**
     * @brief sizeOfOut
     * @return
     */
    virtual int sizeOfOut() const;

    /**
     * @brief flush
     */
    virtual void flush();

    /**
     * @brief 判断通道是否已打开
     * @return
     */
    virtual bool isOpen() const = 0;

    /**
     * @brief 获取通道类型
     * @return
     */
    ChannelType channelType() const;

    /**
     * @brief 获取通道对象唯一标识
     * @return
     */
    std::string identity() const;

    /**
     * @brief setIdentity
     * @param identity
     */
    void setIdentity(const std::string &identity);

    /**
     * @brief config
     * @return
     */
    virtual std::string config() const;

    /**
     * @brief setConfig
     * @param config
     * @return
     */
    virtual bool setConfig(const std::string &config);

    /**
     * @brief name
     * @return
     */
    virtual std::string name() const;

    /**
     * @brief setName
     * @param name
     */
    void setName(const std::string &name);

    /**
     * @brief desc
     * @return
     */
    virtual std::string desc() const;

    /**
     * @brief setDesc
     * @param desc
     */
    virtual void setDesc(const std::string &desc);

    /**
     * @brief lastErrorCode
     * @return
     */
    virtual int lastErrorCode() const { return 0; }

    /**
     * @brief lastError
     * @return
     */
    virtual std::string lastError() const { return ""; }

    /**
     * @brief relayer
     * @return
     */
    Icd::ChannelPtr relayer() const;

    /**
     * @brief setRelayer
     * @param relayer
     */
    void setRelayer(const Icd::ChannelPtr &relayer);

    /**
     * @brief parseConfig
     * @param config
     * @return tuple<name, value>
     */
    static std::map<std::string, std::string> parseConfig(const std::string &config);

private:
    JHandlePtr<ChannelData> d;
};

} // end of namespace Icd

#endif // ICDCOMM_CHANNEL_H
