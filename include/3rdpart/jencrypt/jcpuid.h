#ifndef JCPUID_H
#define JCPUID_H

#include "jencrypt_global.h"

namespace jencrypt {

struct JCpuIdData;

class JENCRYPT_EXPORT JCpuId
{
public:
    explicit JCpuId();
    ~JCpuId();

    void reset();
    bool hasNext() const;
    std::string next();

private:
	std::string getCpuId();

private:
    JCpuIdData *data;
};

} // end of namespace jencrypt

#endif // JCPUID_H
