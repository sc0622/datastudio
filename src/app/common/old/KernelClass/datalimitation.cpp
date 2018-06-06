#include "datalimitation.h"

class DataLimitationPrivate
{
    friend class DataLimitation;
public:
    DataLimitationPrivate()
        : minimum(0.0)
        , maximum(0.0)
        , leftInf(true)
        , rightInf(true)
    {}

private:
    double minimum;     // ��Сֵ
    double maximum;     // ���ֵ
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

double DataLimitation::minimum() const
{
    return d->minimum;
}

void DataLimitation::setMinimum(double minimum)
{
    d->minimum = minimum;
}

double DataLimitation::maximum() const
{
    return d->maximum;
}

void DataLimitation::setMaximum(double maximum)
{
    d->maximum = maximum;
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
