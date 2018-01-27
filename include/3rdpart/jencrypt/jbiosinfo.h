#ifndef JBIOSINFO_H
#define JBIOSINFO_H

#include "jencrypt_global.h"

namespace jencrypt {

struct JBiosInfoData;

class JENCRYPT_EXPORT JBiosInfo
{
public:
    explicit JBiosInfo();
    ~JBiosInfo();

    std::string biosId() const;

private:
	bool initialize();
	std::string awardBiosId() const;
	std::string amibBiosId() const;
	std::string biosDate() const;

private:
	JBiosInfoData *data;
};

} // end of namespace jencrypt

#endif // JBIOSINFO_H
