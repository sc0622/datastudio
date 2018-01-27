#include "stdafx.h"
#include "jbiosinfo.h"
#include <algorithm>
#ifdef _MSC_VER
#include <Windows.h>
#endif

namespace jencrypt {

typedef struct _UNICODE_STRING {
    USHORT		Length;			// ����
    USHORT		MaxLen;			// ��󳤶�
    LPSTR		Buffer;			// ����ָ�룬���������ַ�ڴ�ʱ���˴�ָ��UNICODE�ַ���"\device\physicalmemory"
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG				Length;			// ���� 18h
    HANDLE				RootDirectory;	// 00000000
    PUNICODE_STRING		ObjectName;		// ָ���������ָ��
    ULONG				Attributes;		// ��������00000040H
    PVOID				SecurityDescriptor;			// Points to type POBJECT_ATTRIBUTOR, 0
    PVOID				SecurityQualityOfService;	// Points to type SECURITY_QUALITY_OF_SERVICE, 0
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef DWORD (WINAPI *ZWOS)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
typedef DWORD (WINAPI *ZWMV)(HANDLE, HANDLE, PVOID, ULONG, ULONG, PLARGE_INTEGER,
                             PSIZE_T, DWORD, ULONG, ULONG);

// struct JBiosInfoData

struct JBiosInfoData
{
    LPSTR lpBiosInfo;
};

// class JBiosInfo

JBiosInfo::JBiosInfo()
	: data(new JBiosInfoData)
{
	memset(data, 0, sizeof(JBiosInfoData));
	initialize();
}

JBiosInfo::~JBiosInfo()
{
	delete data;
}

std::string JBiosInfo::biosId() const
{
	if (data->lpBiosInfo == NULL) {
		return std::string();
	}

	std::string info;
	info = awardBiosId();
	if (info.empty()) {
		info = amibBiosId();
		if (info.empty()) {
			info = biosDate();
			if (info.empty()) {
				return std::string();
			}
		}
	}

	std::transform(info.begin(), info.end(), info.begin(), toupper);

	return info;
}

bool JBiosInfo::initialize()
{
    if (data->lpBiosInfo != NULL) {
        return true;
    }

    // ����ntdll.dll���õ�������ַ
    HMODULE hInstLib = NULL;
    ZWOS zwOpenSection = NULL;
    ZWMV zwMapViewOfSection = NULL;

    hInstLib = ::LoadLibraryA("ntdll.dll");
    if (hInstLib == NULL) {
        return false;
    }

    zwOpenSection = (ZWOS)::GetProcAddress(hInstLib, "ZwOpenSection");
    zwMapViewOfSection = (ZWMV)::GetProcAddress(hInstLib, "ZwMapViewOfSection");
    if (zwOpenSection == NULL || zwMapViewOfSection == NULL) {
        return false;
    }

    // ���ú������������ڴ����ӳ��
    wchar_t strPH[] = L"\\device\\physicalmemory";
    UNICODE_STRING strUnicodePH;
    strUnicodePH.Buffer = (LPSTR)strPH;
    strUnicodePH.Length = 0x2c;	// ע���С�ǰ��ֽ��㣬˫�ֽ�һ���ַ�
    strUnicodePH.MaxLen = 0x2e;	// Ҳ�ǰ��ֽ���
    OBJECT_ATTRIBUTES obj_ar;
    obj_ar.Attributes = 64;	// ���ԣ�40H
    obj_ar.Length = 24;		// OBJECT_ATTRIBUTES���͵ĳ���: 18H
    obj_ar.ObjectName = &strUnicodePH;
    obj_ar.RootDirectory = NULL;
    obj_ar.SecurityDescriptor = NULL;
    obj_ar.SecurityQualityOfService = NULL;
    HANDLE hSection = NULL;
    if (zwOpenSection(&hSection, 4, &obj_ar) != 0) {
        return false;
    }

    DWORD ba = 0;		// ��ϵ��Ļ���ַ�������ﷵ��
    LARGE_INTEGER so;
    SIZE_T ssize;
    so.LowPart = 0x000f0000;		// �����ڴ��ַ������f000:0000
    so.HighPart = 0x00000000;		//
    ssize = 0xffff;
    if (zwMapViewOfSection((HANDLE)hSection, (HANDLE)0xffffffff, &ba, 0, 0xffff,
                           &so, &ssize, 1, 0, 2) != 0) {
        return false;
    }

    data->lpBiosInfo = (LPSTR)ba;

    return true;
}

std::string JBiosInfo::awardBiosId() const
{
    /*if (strstr(data->lpBiosInfo + 0xe061, "Award") == 0) {
        return std::string();
    }*/

    if ((isalnum(*(data->lpBiosInfo + 0xe061)) == 0)
            || (isalnum(*(data->lpBiosInfo + 0xec71)) == 0)) {
        return std::string();
    }

    return std::string(data->lpBiosInfo + 0xec71);
}

std::string JBiosInfo::amibBiosId() const
{
    /*if (strnicmp(data->lpBiosInfo + 0xf400, "AMIBIOS", lstrlen("AMIBIOS")) != 0) {
        return std::string();
    }*/

    if ((isalnum(*(data->lpBiosInfo + 0xf400)) == 0)
            || (isalnum(*(data->lpBiosInfo + 0xf478)) == 0)) {
        return std::string();
    }

    return std::string(data->lpBiosInfo + 0xf478);
}

std::string JBiosInfo::biosDate() const
{
    if (isalnum(*(data->lpBiosInfo + 0xfff5)) == 0) {
        return std::string();
    }

    return std::string(data->lpBiosInfo + 0xfff5);
}

} // end of namespace jencrypt
