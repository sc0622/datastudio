#include "stdafx.h"
#include "JMacAddress.h"
#ifdef _MSC_VER
#include <Windows.h>
#include <IPHlpApi.h>
#pragma warning(disable : 4474)
#pragma warning(disable : 4996)
#endif

#pragma comment (lib, "IPHlpApi")

namespace jencrypt {

// struct JMacAddressData

struct JMacAddressData
{
    IP_ADAPTER_INFO adapterInfo[16];    // Allocate information
    PIP_ADAPTER_INFO pAdapterInfo;      // Contains pointer to
};

// class JMacAddress

JMacAddress::JMacAddress()
    : data(new JMacAddressData)
{
    reset();
}

JMacAddress::~JMacAddress()
{
    delete data;
}

void JMacAddress::reset()
{
	data->pAdapterInfo = NULL;
	DWORD dwBufLen = sizeof(data->adapterInfo);		// Save memory size of buffer
	DWORD dwStatus = GetAdaptersInfo(data->adapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS) {
		return;
	}

	data->pAdapterInfo = data->adapterInfo;
}

bool JMacAddress::hasNext() const
{
    return (data->pAdapterInfo != NULL);
}

std::string JMacAddress::next()
{
    if (hasNext()) {
        // 把网卡MAC地址格式化成常用的16进制形式，例如0010-A4E4-5802
		char mac[13] = { '\0' };
		sprintf(mac, "%02X%02X-%02X%02X-%02X%02X", 
			data->pAdapterInfo->Address[0],
			data->pAdapterInfo->Address[1],
			data->pAdapterInfo->Address[2],
			data->pAdapterInfo->Address[3],
			data->pAdapterInfo->Address[4],
			data->pAdapterInfo->Address[5]);
        data->pAdapterInfo = data->adapterInfo->Next;	// Progress through linked list
        return mac;
    } else {
        return std::string();
    }
}

} // end of namespace jencrypt
