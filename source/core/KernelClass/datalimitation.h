#ifndef DATALIMITATION_H
#define DATALIMITATION_H

#include "icdmetadata.h"

class DataLimitationPrivate;
/**
 * @brief 数据项范围类
 */
class KERNELCLASS_EXPORT DataLimitation
{
public:
    /**
     * @brief DataItemLimit
     */
    explicit DataLimitation();

    ~DataLimitation();

    /**
     * @brief min
     * @return
     */
    double min() const;

    /**
     * @brief setMin
     * @param min
     */
    void setMin(double min);

    /**
     * @brief max
     * @return
     */
    double max() const;

    /**
     * @brief setMax
     * @param max
     */
    void setMax(double max);

    /**
     * @brief leftInf
     * @return
     */
    bool leftInf() const;

    /**
     * @brief setLeftInf
     * @param leftInf
     */
    void setLeftInf(bool leftInf);

    /**
     * @brief rightInf
     * @return
     */
    bool rightInf() const;

    /**
     * @brief setRightInf
     * @param rightInf
     */
    void setRightInf(bool rightInf);

private:
    DataLimitationPrivate   *d;
};

#endif // DATALIMITATION_H
