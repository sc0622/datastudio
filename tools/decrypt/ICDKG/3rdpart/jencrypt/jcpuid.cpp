#include "stdafx.h"
#include "jcpuid.h"
#ifdef _MSC_VER
#include <Windows.h>
#pragma warning(disable : 4996)
#endif

namespace jencrypt {

// struct JCpuIdData

struct JCpuIdData
{

    JCpuId *q;
	friend class JCpuId;
    HANDLE hCurProcHandle;
    DWORD dwProcAffinity;
    DWORD dwSysAffinity;
    DWORD dwAffinityMask;
};

// class JCpuId

JCpuId::JCpuId()
    : data(new JCpuIdData)
{
	memset(data, 0, sizeof(JCpuIdData));
    reset();
}

JCpuId::~JCpuId()
{
    reset();
    delete data;
}

void JCpuId::reset()
{
	// Reset the Process affinity.
	if (data->hCurProcHandle != NULL) {
		data->dwProcAffinity = 255;
		SetProcessAffinityMask(data->hCurProcHandle, data->dwProcAffinity);
	}

	data->hCurProcHandle = GetCurrentProcess();
	GetProcessAffinityMask(data->hCurProcHandle, &data->dwProcAffinity, &data->dwSysAffinity);

	// Check if available process affinity mask is equal to the
	// available system affinity mask.
	if (data->dwProcAffinity != data->dwSysAffinity) {
		return;
	}

	data->dwAffinityMask = 1;  
}

bool JCpuId::hasNext() const
{
    return (data->dwAffinityMask != 0 && (data->dwAffinityMask <= data->dwProcAffinity));
}

std::string JCpuId::next()
{
    if (hasNext()) {
        // Check if this CPU is available.
        if (data->dwAffinityMask & data->dwProcAffinity) {
            if (SetProcessAffinityMask(data->hCurProcHandle, data->dwAffinityMask)) {
                Sleep(0);		// Give OS time to switch CPU.
            }
        }

        const std::string id = getCpuId();

        data->dwAffinityMask <<= 1;

        return id;
    } else {
        // Reset the Process affinity.
        SetProcessAffinityMask(data->hCurProcHandle, data->dwProcAffinity);
        return std::string();
    }
}

std::string JCpuId::getCpuId()
{
	DWORD x[] = {0, 0, 0};
	CHAR str[30] = {0};

	_asm {
		mov eax, 1
			cpuid
			mov dword ptr x, eax
			mov eax, 3
			cpuid
			mov x + 4, edx
			mov x + 8, ecx
	}

	sprintf(str, "%08X-%08X-%08X", x[0], x[1], x[2]);

	return std::string(str);
}

} // end of namespace jencrypt
