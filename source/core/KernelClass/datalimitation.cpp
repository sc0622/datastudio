#include "datalimitation.h"

class DataLimitationPrivate
{
    friend class DataLimitation;
public:
    DataLimitationPrivate()
        : min(0.0)
        , max(0.0)
        , leftInf(true)
        , rightInf(true)
    {}

private:
    double min;     // ��Сֵ
    double max;     // ���ֵ
    bool leftInf;   // �Ƿ���СֵΪ-����
    bool rightInf;  // �Ƿ����ֵΪ+����
};

DataLimitation::DataLimitation()
    : d(new DataLimitationPrivate())
{

}

DataLimitation::~DataLimitation()
{

}

double DataLimitation::min() const
{
    return d->min;
}

void DataLimitation::setMin(double min)
{
    d->min = min;
}

double DataLimitation::max() const
{
    return d->max;
}

void DataLimitation::setMax(double max)
{
    d->max = max;
}

bool DataLimitation::leftInf() const
{
    return d->leftInf;
}

void DataLimitation::setLeftInf(bool leftInf)
{
    d->leftInf = leftInf;
}

bool DataLimitation::rightInf() const
{
    return d->rightInf;
}

void DataLimitation::setRightInf(bool rightInf)
{
    d->rightInf = rightInf;
}
