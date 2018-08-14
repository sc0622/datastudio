#ifndef ICDWORKER_UTIL_H
#define ICDWORKER_UTIL_H

#include "icdworker_global.h"

class QFileDevice;

namespace Icd {

class Table;
typedef std::shared_ptr<Table> TablePtr;

/**
 * @brief 使用查表法进行CRC校验值计算
 * @param data ：需要进行CRC校验计算机的数据段指针
 * @param size :需要进行CRC校验计算的数据段长度
 * @return
 */
ICDWORKER_EXPORT unsigned short gCalcCrc16(const unsigned char *data, unsigned int size);

/**
 * @brief doCheck
 * @param table
 * @param buffer
 * @return
 */
ICDWORKER_EXPORT bool doCheck(const Icd::TablePtr &table, const char *buffer);

/**
 * @brief checkData
 * @param table
 * @param fileHeaderSize
 * @param source
 * @param target
 * @return
 */
ICDWORKER_EXPORT bool checkData(const Icd::TablePtr &table, int fileHeaderSize,
                                QFileDevice *source, QFileDevice *target);

} // end of namespace Icd

#endif // ICDWORKER_UTIL_H
