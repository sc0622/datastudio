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
     * @brief minimum
     * @return
     */
    double minimum() const;

    /**
     * @brief setMinimum
     * @param minimum
     */
    void setMinimum(double minimum);

    /**
     * @brief maximum
     * @return
     */
    double maximum() const;

    /**
     * @brief setMaximum
     * @param maximum
     */
    void setMaximum(double maximum);

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
