#ifndef ICDCOMM_SERIALCHANNEL_H
#define ICDCOMM_SERIALCHANNEL_H

#include "icdcomm_channel.h"

namespace Icd {

class SerialChannel;
class SerialChannelData;
typedef std::shared_ptr<SerialChannel> SerialChannelPtr;

class ICDCOMM_EXPORT SerialChannel : public Channel
{
public:
    enum DataBits {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8,
        UnknownDataBits = -1
    };

    enum StopBits {
        OneStop = 1,
        OneAndHalfStop = 3,
        TwoStop = 2,
        UnknownStopBits = -1
    };

    enum Parity {
        NoParity = 0,
        EvenParity = 2,
        OddParity = 3,
        SpaceParity = 4,
        MarkParity = 5,
        UnknownParity = -1
    };

    explicit SerialChannel();

    /**
     * @brief SerialChannel
     * @param portName
     * @param baudRate
     * @param dataBits
     * @param stopBits
     * @param parity
     */
    explicit SerialChannel(const std::string &portName, int baudRate, DataBits dataBits = Data8,
                           StopBits stopBits = OneStop, Parity parity = NoParity);

    virtual ~SerialChannel() override;

    /**
     * @brief isOpen
     * @return
     */
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
     * @brief waitForReadyRead
     * @param msecs
     * @return
     */
    bool waitForReadyRead(int msecs) override;

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
     * @return "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
     */
    std::string config() const override;

    /**
     * @brief setConfig
     * @param config : "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
     * @return
     */
    bool setConfig(const std::string &config) override;

    /**
     * @brief desc
     * @return
     */
    std::string desc() const override;

    /**
     * @brief 设置串口参数
     * @param [in] portName : 串口名称
     * @param [in] baudRate ：波特率
     * @param [in] dataBits : 数据位
     * @param [in] stopBits : 停止 位
     * @param [in] parity : 校验方式
     * @return
     */
    bool setConfig(const std::string &portName, int baudRate, DataBits dataBits,
                   StopBits stopBits, Parity parity);

    /**
     * @brief 获取串口名称
     * @return
     */
    std::string portName() const;

    /**
     * @brief setPortName
     * @param portName
     */
    void setPortName(const std::string &portName);

    /**
     * @brief 获取波特率
     * @return
     */
    int baudRate() const;

    /**
     * @brief setBaudRate
     * @param baudRate
     */
    void setBaudRate(int baudRate);

    /**
     * @brief 获取数据位
     * @return
     */
    DataBits dataBits() const;

    /**
     * @brief setDataBits
     * @param dataBits
     */
    void setDataBits(DataBits dataBits);

    /**
     * @brief 获取停止位
     * @return
     */
    StopBits stopBits() const;

    /**
     * @brief setStopBits
     * @param stopBits
     */
    void setStopBits(StopBits stopBits);

    /**
     * @brief 获取校验方式
     * @return
     */
    Parity parity() const;

    /**
     * @brief setParity
     * @param parity
     */
    void setParity(Parity parity);

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
    std::shared_ptr<SerialChannelData> d;
};

} // end of namespace Icd

#endif // SERIALCHANNEL_H
