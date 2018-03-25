#ifndef JSERIALPORT_H
#define JSERIALPORT_H

#include "../icdcomm_global.h"
#include "../icdcomm_serialchannel.h"

#ifdef SERIAL_USE_PCOMM
#include "moxa/pcomm.h"
#else
#include <QSerialPort>
#endif

namespace Icd {

class JSerialPortData;

/**
 * @brief The JSerialPort class
 */
class ICDCOMM_EXPORT JSerialPort
{
public:
    /**
     * @brief The DataBits enum
     */
    enum DataBits {
        Data5 = 5,
        Data6 = 6,

        Data7 = 7,
        Data8 = 8,
        InvalidDataBits = -1
    };

    /**
     * @brief The StopBits enum
     */
    enum StopBits {
        OneStop = 1,
        OneAndHalfStop = 3,
        TwoStop = 2,
        InvalidStopBits = -1
    };

    /**
     * @brief The Parity enum
     */
    enum Parity {
        NoParity = 0,
        EvenParity = 2,
        OddParity = 3,
        SpaceParity = 4,
        MarkParity = 5,
        InvalidParity = -1
    };

    /**
     * @brief JSerialPort
     */
    explicit JSerialPort();

    /**
     * @brief JSerialPort
     * @param portName
     */
    explicit JSerialPort(const std::string &portName);

    ~JSerialPort();

    /**
     * @brief portName
     * @return
     */
    std::string portName() const;

    /**
     * @brief setPortName
     * @param portName
     */
    void setPortName(const std::string &portName);

    /**
     * @brief baudRate
     * @return
     */
    int baudRate() const;

    /**
     * @brief setBaudRate
     * @param value
     */
    void setBaudRate(int value);

    /**
     * @brief dataBits
     * @return
     */
    DataBits dataBits() const;

    /**
     * @brief setDataBits
     * @param value
     */
    void setDataBits(DataBits value);

    /**
     * @brief stopBits
     * @return
     */
    StopBits stopBits() const;

    /**
     * @brief setStopBits
     * @param value
     */
    void setStopBits(StopBits value);

    /**
     * @brief parity
     * @return
     */
    Parity parity() const;

    /**
     * @brief setParity
     * @param parity
     */
    void setParity(Parity parity);

    /**
     * @brief isOpen
     * @return
     */
    bool isOpen() const;

    /**
     * @brief open
     * @return
     */
    bool open();

    /**
     * @brief close
     */
    void close();

    /**
     * @brief waitForReadyRead
     * @param msecs
     * @return
     */
    bool waitForReadyRead(int msecs);

    /**
     * @brief read
     * @param buffer
     * @param size
     * @return
     */
    int read(char* buffer, int size);

    /**
     * @brief write
     * @param buffer
     * @param size
     * @return
     */
    int write(const char *buffer, int size);

    /**
     * @brief sizeOfIn
     * @return
     */
    long sizeOfIn() const;

    /**
     * @brief sizeOfOut
     * @return
     */
    long sizeOfOut() const;

    /**
     * @brief flush
     */
    void flush();

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
    JHandlePtr<JSerialPortData> d;
};

} // end of namespace Icd

#endif // JSERIALPORT_H
