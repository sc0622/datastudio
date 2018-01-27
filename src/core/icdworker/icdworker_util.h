#ifndef ICDWORKER_UTIL_H
#define ICDWORKER_UTIL_H

#include "icdworker_global.h"

class QFileDevice;

namespace Icd {

template<typename T> class JHandlePtr;

class Table;
typedef JHandlePtr<Table> TablePtr;

/**
 * @brief ʹ�ò������CRCУ��ֵ����
 * @param data ����Ҫ����CRCУ�����������ݶ�ָ��
 * @param size :��Ҫ����CRCУ���������ݶγ���
 * @return
 */
extern "C" ICDWORKER_EXPORT unsigned short gCalcCrc16(unsigned char *data, unsigned int size);

/**
 * @brief doCheck
 * @param table
 * @param buffer
 * @return
 */
extern "C" ICDWORKER_EXPORT bool doCheck(const Icd::TablePtr &table, const char *buffer);

/**
 * @brief checkData
 * @param table
 * @param fileHeaderSize
 * @param source
 * @param target
 * @return
 */
extern "C" ICDWORKER_EXPORT bool checkData(const Icd::TablePtr &table, int fileHeaderSize,
                                           QFileDevice *source, QFileDevice *target);

} // end of namespace Icd

#endif // ICDWORKER_UTIL_H
