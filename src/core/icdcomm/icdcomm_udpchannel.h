﻿#ifndef UDPCHANNEL_H
#define UDPCHANNEL_H

#include <string>
#include "icdcomm_channel.h"

namespace Icd {

class UdpChannel;
class UdpChannelData;
typedef std::shared_ptr<UdpChannel> UdpChannelPtr;

class ICDCOMM_EXPORT UdpChannel : public Channel
{
public:
    enum OpenMode {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly
    };

    explicit UdpChannel();
    explicit UdpChannel(std::string localIP, int localPort,
                        std::string remoteIP, int remotePort,
                        OpenMode openMode = ReadWrite);
    virtual ~UdpChannel() override;

public:
    bool isOpen() const override;

    /**
     * @brief 打开数据通道
     * @return true:打开成功,false打开失败
     */
    bool open() override;

    /**
     * @brief 关闭数据通道
     */
    void close() override;

    /**
     * @brief 读取数据
     * @param [out] buffer,待读取数据存储缓冲区
     * @param [in] size，缓冲区大小
     * @return 实际读取数据值,如果读取错误则返回-1
     */
    int read(char* buffer, int size) override;

    /**
     * @brief 写入数据
     * @param [in] buffer : 写入数据缓冲区
     * @param [in] size : 缓冲区大小
     * @return 实际写入数据长度，写入错误返回-1
     */
    int write(const char* buffer, int size) override;

    /**
     * @brief sizeOfIn
     * @return
     */
    int sizeOfIn() const override;

    /**
     * @brief sizeOfOut
     * @return
     */
    int sizeOfOut() const override;

    /**
     * @brief flush
     */
    void flush() override;

    /**
     * @brief config
     * @return "udp: -local [ip]:[port] -remote [ip]:[port]"
     */
    std::string config() const override;

    /**
     * @brief setConfig
     * @param config : "udp: -local [ip]:[port] -remote [ip]:[port]"
     * @return
     */
    bool setConfig(const std::string &config) override;

    /**
     * @brief desc
     * @return
     */
    std::string desc() const override;

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
     * @brief setConfig
     * @param localIP
     * @param localPort
     * @param remoteIP
     * @param remotePort
     * @return
     */
    bool setConfig(const std::string &localIP, int localPort,
                   const std::string &remoteIP, int remotePort);

    /**
     * @brief isMulticastIP
     * @param ip
     * @return
     */
    bool isMulticastIP(const std::string &ip) const;

    /**
     * @brief 获取本地IP地址
     * @return
     */
    std::string localIP() const;

    /**
     * @brief setLocalIP
     * @param ip
     */
    void setLocalIP(const std::string &ip);

    /**
     * @brief 获取本地端口
     * @return
     */
    int localPort() const;

    /**
     * @brief setLocalPort
     * @param port
     */
    void setLocalPort(int port);

    /**
     * @brief 获取远程IP地址
     * @return
     */
    std::string remoteIP() const;

    /**
     * @brief setRemoteIP
     * @param ip
     */
    void setRemoteIP(const std::string &ip);

    /**
     * @brief 获取远程端口
     * @return
     */
    int remotePort() const;

    /**
     * @brief setRemotePort
     * @param port
     */
    void setRemotePort(int port);

    /**
     * @brief lastErrorCode
     * @return
     */
    int lastErrorCode() const override;

    /**
     * @brief lastError
     * @return
     */
    std::string lastError() const override;

private:
    std::shared_ptr<UdpChannelData> d;
};

} // end of namespace Icd

#endif // UDPCHANNEL_H
