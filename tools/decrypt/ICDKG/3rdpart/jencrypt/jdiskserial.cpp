#include "stdafx.h"
#include "JDiskSerial.h"
#ifdef _MSC_VER
#include <Windows.h>
#include <WinIoCtl.h>
#pragma warning(disable : 4996)
#endif

namespace jencrypt {

	extern void __convertToString(LPSTR szDiskSerial, DWORD dwDiskData[], int nSize);
	extern void __flipAndCodeBytes(LPSTR szResult, LPCSTR cszStr);
	extern void __flipBlankSpace(LPSTR szResult, LPCSTR cszStr);

//
typedef struct _IDINFO {
    USHORT wGenCOnfig;
    USHORT wNumCyls;
    USHORT wReserved2;
    USHORT wNumHeads;
    USHORT wReserved4;
    USHORT wReserved5;
    USHORT wNumSectorsPerTrack;
    USHORT wVendorUnique[3];
    CHAR sSerialNumber[20];
    USHORT sBufferType;
    USHORT wBufferSize;
    USHORT wECCSize;
    CHAR sFirmwareRev[8];
    CHAR sModelNumber[40];
    USHORT wMoreVendorUnique;
    USHORT wReserved48;

    struct {
        USHORT reserved1 : 8;
        USHORT DMA : 1;
        USHORT LBA : 1;
        USHORT DislORDY : 1;
        USHORT IORDY : 1;
        USHORT SoftReset : 1;
        USHORT Overlap : 1;
        USHORT Queue : 1;
        USHORT IniDMA : 1;
    } wCapabilities;

    USHORT wReserved1;
    USHORT wPIOTiming;
    USHORT wDMATiming;

    struct {
        USHORT CHSNumber : 1;
        USHORT CycleNumber : 1;
        USHORT UnltraDMA : 1;
        USHORT reserved : 13;
    } wFieldValidity;

    USHORT wNumCurCyls;
    USHORT wNumCurHeads;
    USHORT wNumCurSectorsPerTrack;
    USHORT wCurSectorsLow;
    USHORT wCurSectorsHigh;

    struct {
        USHORT CurNumber : 8;
        USHORT Multi : 1;
        USHORT reserved1 : 7;
    } wMultiSectorStuff;

    ULONG dwTotalSectors;
    USHORT wSingleWordDMA;

    struct {
        USHORT Mode0 : 1;
        USHORT Mode1 : 1;
        USHORT Mode2 : 1;
        USHORT Reserved1 : 5;
        USHORT Mode0Sel : 1;
        USHORT Mode1Sel : 1;
        USHORT Mode2Sel : 1;
        USHORT Reserved2 : 5;
    } wMultiWordDMA;

    struct {
        USHORT AdvPOIModes : 8;
        USHORT Reserved : 8;
    } wPIOCapacity;

    USHORT wMinMultiWordDMACycle;
    USHORT wRecMultiWordDMACycle;
    USHORT wMinPIONoFlowCycle;
    USHORT wMinPOIFlowCycle;
    USHORT wReserved69[11];
    struct {
        USHORT Reserved1 : 1;
        USHORT ATA1 : 1;
        USHORT ATA2 : 1;
        USHORT ATA3 : 1;
        USHORT ATA4 : 1;
        USHORT ATA5 : 1;
        USHORT ATA6 : 1;
        USHORT ATA7 : 1;
        USHORT ATA8 : 1;
        USHORT ATA9 : 1;
        USHORT ATA10 : 1;
        USHORT ATA11 : 1;
        USHORT ATA12 : 1;
        USHORT ATA13 : 1;
        USHORT ATA14 : 1;
        USHORT Reserved2 : 1;
    } wMajorVersion;

    USHORT wMinorVersin;
    USHORT wReserved82[6];

    struct {
        USHORT Mode0 : 1;
        USHORT Mode1 : 1;
        USHORT Mode2 : 1;
        USHORT Mode3 : 1;
        USHORT Mode4 : 1;
        USHORT Mode5 : 1;
        USHORT Mode6 : 1;
        USHORT Mode7 : 1;
        USHORT Mode0Sel : 1;
        USHORT Mode1Sel : 1;
        USHORT Mode2Sel : 1;
        USHORT Mode3Sel : 1;
        USHORT Mode4Sel : 1;
        USHORT Mode5Sel : 1;
        USHORT Mode6Sel : 1;
        USHORT Mode7Sel : 1;
    } wUltraDMA;

    USHORT wReserved89[167];
} IDINFO, *PIDINFO;

// (*output bBuffer for the VxD (rt_IdeDInfo record)*)
typedef struct _rt_IdeDInfo {
    BYTE	IDEExists[4];
    BYTE	DiskExists[8];
    WORD	DiskRawInfo[8 * 256];
} rt_IdeDInfo, *prt_IdeDInfo;

#define IDENTIFY_BUFFER_SIZE	512

// IOCTRL commands
#define DFP_GET_VERSION			0x00074080
#define DFP_SEND_DRIVE_COMMAND	0x0007c084
#define DFP_RECEIVE_DRIVE_DATA	0x0007c088

// Valid values for the bCommandReg member of IDEREGS
#define IDE_ATAPI_IDENTIFY		0xa1	// Returns ID sector for ATAPI.
#define IDE_ATA_INDENTIFY		0xec	// Returns ID sector for ATA.

// GETVERSIONOUTPARAMS contains the data returned from the
// Get Driver Version function.
typedef struct _GETVERSIONOUTPARAMS {
    BYTE	bVersion;		// Binary driver version.
    BYTE	bRevision;		// Binary driver revision,
    BYTE	bReserved;		// Not used.
    BYTE	bIDEDeviceMap;	// Bit map of IDE devices.
    DWORD	dwCapabilities;	// Bit map of driver capabilities.
    DWORD	dwReserved[4];	// For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

// IDE registers
// 	typedef struct _IDEREGS {
// 		BYTE	bFeaturesReg;		// Used for specifying SMART "commands".
// 		BYTE	bSectorCountReg;	// IDE sector count register.
// 		BYTE	bSectorNumberReg;	// IDE sector number register.
// 		BYTE	bCylLowReg;			// IDE low order cylinder value.
// 		BYTE	bCylHighReg;		// IDE high order cylinder value.
// 		BYTE	bDriveHeadReg;		// IDE drive/head register.
// 		BYTE	bCommandReg;		// Actual IDE command.
// 		BYTE	bReserved;			// reserved for future use and must be Zero.
// 	} IDEREGS, *PIDEREGS, *LPIDEREGS;

// SENDCMDINPARAMS contains the input parameters, for the
// Send Command to Drive function.
// 	typedef struct _SENDCMDINPARAMS {
// 		DWORD		dwBufferSize;		// Buffer size in bytes.
// 		IDEREGS		irDriveRegs;		// Structure with drive register.
// 		BYTE		bDriveNumber;		// Physical drive number to send command to (0, 1, 2, 3).
// 		BYTE		bReserved[3];		// Reserved for future expansion.
// 		DWORD		dwReserved[4];		// For future use.
// 		BYTE		bBuffer[1];			// Input buffer.
// 	} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

// Status returned from driver.
// 	typedef struct _DRIVERSTATUS {
// 		BYTE	bDriverError;			// Error from driver, or 0 if no error.
// 		BYTE	bIDEStatus;				// Content if IDE Error register.
// 		// Only valid when bDri8verError is SMART_IDE_ERROR.
// 		BYTE	bReserved[2];			// Reserved for future expansion.
// 		DWORD	dwReserved[2];			// Reserved for future expansion.
// 	} DRIVERSSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

// Structure returned by PhysicalDriver IOCTRL for several commands.
// 	typedef struct _SENDCMDOUTPARAMS {
// 		DWORD			dwBufferSize;	// Size of bBuffer in bytes.
// 		DRIVERSSTATUS	DriverStatus;	// Driver status structure.
// 		BYTE			bBuffer[1];		// Buffer of arbitrary length in which to store the data read from drive.
// 	} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;

#define SENDIDLENGTH					(sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE)

#define FILE_DEVICE_SCSI				0x0000001b
#define IOCTL_SCSI_MINIPORT_IDENTIFY	((FILE_DEVICE_SCSI << 16) + 0x00000501)
#define IOCTL_SCSI_MINIPORT				0x0004d008	// see NTDOSSCSI.H for definition.

//
typedef struct _SRB_IO_CONTROL {
    ULONG	HeaderLength;		//
    UCHAR	Signature[8];		//
    ULONG	Timeout;			//
    ULONG	ControlCode;		//
    ULONG	ReturnCode;			//
    ULONG	Length;				//
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

// The following struct defines the interesting part of the IDENTIFY buffer.
typedef struct _IDSECTOR {
    USHORT	wGetConfig;
    USHORT	wNumCyls;
    USHORT	wReserved;
    USHORT	wNumHeads;
    USHORT	wBytesPerTrack;
    USHORT	wBytesPerSector;
    USHORT	wSectorsPerTrack;
    USHORT	wVendorUnique[3];
    CHAR	sSerialNumber[20];
    USHORT	wBufferType;
    USHORT	wBufferSize;
    USHORT	wECCSize;
    CHAR	sFirmwareRev[8];
    CHAR	sModeNumber[40];
    USHORT	wMoreVenderUnique;
    USHORT	wDoubleWordIO;
    USHORT	wCapabilities;
    USHORT	wReserved1;
    USHORT	wPIOTiming;
    USHORT	wDMATiming;
    USHORT	wBS;
    USHORT	wNumCurrentCyls;
    USHORT	wNumCurrentHeads;
    USHORT	wNumCurrentSectorsPerTrack;
    ULONG	ulCurrentSectorsCapacity;
    USHORT	wMultSectorStuff;
    ULONG	ulTotalAddressableSectors;
    USHORT	wSingleWordDMA;
    USHORT	wMultiWordDMA;
    BYTE	bReserved[128];
} IDSECTOR, *PIDSECTOR;

#define IOCTL_STORAGE_QUERY_PROPERTY			CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER	CTL_CODE(IOCTL_STORAGE_BASE, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MEDIA_SERIAL_NUMBER_DATA {
    ULONG	SerialNumberLength;		//
    ULONG	Result;					//
    ULONG	Reserved[2];			//
    ULONG	SerialNumberData[1];	//
} MEDIA_SERIAL_NUMBER_DATA, *PMEDIA_SERIAL_NUMBER_DATA;

// class JDiskSerialData

struct JDiskSerialData
{
    int nSerialCount;
    int nReadCount;
    CHAR szSerials[32][64];
};

// class JDiskSerial

JDiskSerial::JDiskSerial()
    : data(new JDiskSerialData)
{
	memset(data, 0, sizeof(JDiskSerialData));
    reset();
}

JDiskSerial::~JDiskSerial()
{
    delete data;
}

void JDiskSerial::reset()
{
	data->nReadCount = 0;

	OSVERSIONINFO verInfo;
	std::memset(&verInfo, 0, sizeof(OSVERSIONINFO));
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&verInfo);

	DWORD dwPlatform = verInfo.dwPlatformId;
	BOOL done = FALSE;

	if (VER_PLATFORM_WIN32_NT == dwPlatform) {
		done = readPhysicalDriveInNTWithAdminRights();
		if (FALSE == done) {
			done = readIdeDriveAsScsiDriveNT();
		}

		if (FALSE == done) {
			done = readPhysicalDriveNTWithZeroRights();
		}
	} else {
		for (int i = 0; i < 10 && !done; ++i) {
			done = readDrivePortsInWin9x();
		}
	}
}

bool JDiskSerial::hasNext() const
{
    return (data->nReadCount < data->nSerialCount);
}

std::string JDiskSerial::next()
{
    if (hasNext()) {
        return std::string(data->szSerials[data->nReadCount++]);
    } else {
        return std::string();
    }
}

bool JDiskSerial::readDrivePortsInWin9x()
{
	BOOL done = FALSE;

	// Set the thread priority high to that we get exclusive access to the disk.
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	// 1. Try to load the VxD.
	HANDLE hVxDHandle = CreateFileA("\\\\.\\IDE21201.VXD", 0, 0, 0, 0, FILE_FLAG_DELETE_ON_CLOSE, 0);
	if (hVxDHandle != INVALID_HANDLE_VALUE) {
		// 2. Make an output buffer for the VxD.
		DWORD dwBytesReturned = 0;
		rt_IdeDInfo rtinfo;
		prt_IdeDInfo pOutBuffVxD = &rtinfo;

		// WARNING!!!
		// HAVE to zero out the buffer space for the IDE information!
		// IF this is NOT done then garbage could be in the memory
		// locations indicating if a disk exists not.
		ZeroMemory(&rtinfo, sizeof(rtinfo));

		// 3. Run VxD function.
		DeviceIoControl(hVxDHandle, 1, 0, 0, pOutBuffVxD, sizeof(prt_IdeDInfo), &dwBytesReturned, 0);

		// 4. Unload VxD.
		CloseHandle(hVxDHandle);

		// 5. Translate and store data.
		for (int i = 0; i < 8; ++i) {
			if ((0 != pOutBuffVxD->DiskExists[i]) && (0 != pOutBuffVxD->DiskExists[i / 2])) {
				// process the information for this buffer.
				DWORD dwDiskInfo[10];
				for (int j = 10; j < 20; ++j) {
					dwDiskInfo[j - 10] = pOutBuffVxD->DiskRawInfo[i * 256 + j];
				}

				__convertToString(data->szSerials[data->nSerialCount++], dwDiskInfo, 10);
			}
		}

		done = TRUE;
	}

	// Reset the thread priority back to normal.
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

	return (done == TRUE);
}

bool JDiskSerial::readPhysicalDriveInNTWithAdminRights()
{
	BOOL done = FALSE;

	for (int i = 0; i < 16; ++i) {
		CHAR driveName[256];
		sprintf(driveName, "\\\\.\\PhysicalDrive%d", i);

		HANDLE hDevice = CreateFileA(driveName, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hDevice == INVALID_HANDLE_VALUE) {
			continue;
		}

		GETVERSIONOUTPARAMS outParams;
		DWORD dwBytesReturned = 0;
		std::memset((void*)&outParams, 0, sizeof(GETVERSIONOUTPARAMS));

		BOOL result = DeviceIoControl(hDevice, DFP_GET_VERSION,
			NULL, 0, &outParams, sizeof(GETVERSIONOUTPARAMS),
			&dwBytesReturned, NULL)/* && (verOutParams.bIDEDeviceMap > 0)*/;
		if (!result) {
			CloseHandle(hDevice);
			continue;
		}

		// Now, get the ID driver for all IDE devices in the system.
		// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
		// otherwise use the IDE_ATA_IDENTIFY command.
		BYTE IdOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
		BYTE btIDCmd = (outParams.bIDEDeviceMap >> i && 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_INDENTIFY;
		SENDCMDINPARAMS scip;
		std::memset((void*)&scip, 0, sizeof(SENDCMDINPARAMS));
		std::memset(IdOutCmd, 0, sizeof(IdOutCmd));

		scip.cBufferSize = IDENTIFY_BUFFER_SIZE;
		scip.irDriveRegs.bFeaturesReg = 0;
		scip.irDriveRegs.bSectorCountReg = 1;
		scip.irDriveRegs.bCylLowReg = 0;
		scip.irDriveRegs.bCylHighReg = 0;
		scip.irDriveRegs.bDriveHeadReg = 0xA0 | ((i & 1) << 4);
		scip.irDriveRegs.bCommandReg = btIDCmd;
		scip.bDriveNumber = i;

		result = DeviceIoControl(hDevice, DFP_RECEIVE_DRIVE_DATA,
			(LPVOID)&scip, sizeof(SENDCMDINPARAMS) - 1,
			(LPVOID)(PSENDCMDOUTPARAMS)&IdOutCmd,
			sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
			&dwBytesReturned, NULL);
		if (!result) {
			CloseHandle(hDevice);
			continue;
		}

		// process the information for this buffer.
		DWORD dwDiskInfo[10];
		USHORT *pIdSector = (USHORT*) ((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;

		for (int j = 10; j < 20; ++j) {
			dwDiskInfo[j - 10] = pIdSector[j];
		}

		__convertToString(data->szSerials[data->nSerialCount++], dwDiskInfo, 10);

		done = TRUE;

		CloseHandle(hDevice);
	}

	return (done == TRUE);
}

bool JDiskSerial::readIdeDriveAsScsiDriveNT()
{
	BOOL done = FALSE;

	for (int i = 0; i < 16; ++i) {
		CHAR driveName[256];
		sprintf(driveName, "\\\\.\\Scsi%d:", i);

		HANDLE hScsiDriveIOCTL = CreateFileA(driveName,
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE) {
			for (int j = 0; j < 2; ++j) {
				CHAR buffer[sizeof(SRB_IO_CONTROL) + SENDIDLENGTH];
				PSRB_IO_CONTROL pSRBIOCTL = (PSRB_IO_CONTROL) buffer;
				PSENDCMDINPARAMS pIn = (PSENDCMDINPARAMS) (buffer + sizeof(SRB_IO_CONTROL));
				DWORD dummy;

				std::memset(buffer, 0, sizeof(buffer));
				pSRBIOCTL->HeaderLength = sizeof(SRB_IO_CONTROL);
				pSRBIOCTL->Timeout = 10000;	// 10s
				pSRBIOCTL->Length = SENDIDLENGTH;
				pSRBIOCTL->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				strncpy((CHAR*) pSRBIOCTL->Signature, "SCSIDISK", 8);

				pIn->irDriveRegs.bCommandReg = IDE_ATA_INDENTIFY;
				pIn->bDriveNumber = j;

				if (DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT,
					buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
					buffer, sizeof(SRB_IO_CONTROL) + SENDIDLENGTH, &dummy, NULL)) {
						PSENDCMDOUTPARAMS pOut = (PSENDCMDOUTPARAMS) (buffer + sizeof(SRB_IO_CONTROL));
						PIDSECTOR pId = (PIDSECTOR) (pOut->bBuffer);

						if (pId->sModeNumber[0] != 0) {
							// process the information for this buffer.
							DWORD dwDiskInfo[10];
							USHORT *pIdSector = (USHORT*) pId;

							for (int k = 10; k < 20; ++k) {
								dwDiskInfo[k - 10] = pIdSector[k];
							}

							__convertToString(data->szSerials[data->nSerialCount++], dwDiskInfo, 10);

							done = TRUE;
						}
				}
			}

			CloseHandle(hScsiDriveIOCTL);
		}
	}

	return (done == TRUE);
}

bool JDiskSerial::readPhysicalDriveNTWithZeroRights()
{
	BOOL done = FALSE;

	for (int i = 0; i < 16; ++i) {
		CHAR driveName[256];
		sprintf(driveName, "\\\\.\\PhysicalDrive&d", i);

		HANDLE hPhysicalDriveIOCTL = CreateFileA(driveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE) {
			int nCurrentCount = data->nSerialCount;

			STORAGE_PROPERTY_QUERY query;
			DWORD dwBytesReserved = 0;
			CHAR buffer[8192];

			std::memset((void*)&query, 0, sizeof(STORAGE_PROPERTY_QUERY));

			query.PropertyId = StorageDeviceIdProperty;
			query.QueryType = PropertyStandardQuery;

			std::memset(buffer, 0, sizeof(buffer));
			if (DeviceIoControl(hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
				&query, sizeof(STORAGE_PROPERTY_QUERY), buffer,
				sizeof(buffer), &dwBytesReserved, NULL)) {
					PSTORAGE_DEVICE_DESCRIPTOR  pDescrip = (PSTORAGE_DEVICE_DESCRIPTOR) buffer;

					__flipAndCodeBytes(data->szSerials[data->nSerialCount], &buffer[pDescrip->SerialNumberOffset]);

					//Serial number must be alphanumeric.
					if ((isalnum(data->szSerials[data->nSerialCount][0]))
						/*|| isalnum(data->szSerials[data->nSerialCount][19])*/) {
						data->nSerialCount++;
						done = TRUE;
					}
			}

			if (nCurrentCount == data->nSerialCount) {
				std::memset(buffer, 0, sizeof(buffer));
				if (DeviceIoControl(hPhysicalDriveIOCTL, IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER,
					NULL, 0, buffer, sizeof(buffer), &dwBytesReserved, NULL)) {
						PMEDIA_SERIAL_NUMBER_DATA pMediaSerialNumber = (PMEDIA_SERIAL_NUMBER_DATA) buffer;

						__flipAndCodeBytes(data->szSerials[data->nSerialCount], (CHAR*) pMediaSerialNumber->SerialNumberData);

						// Serial number must be alphanumeric.
						if ((isalnum(data->szSerials[data->nSerialCount][0]))
							/*|| isalnum(data->szSerials[data->nSerialCount][19])*/) {
							data->nSerialCount++;
							done = TRUE;
						}
				}
			}

			CloseHandle(hPhysicalDriveIOCTL);
		}
	}

	return (done == TRUE);
}

void __convertToString(LPSTR szDiskSerial, DWORD dwDiskData[], int nSize)
{
	int pos = 0;

	for (int i = 0; i < nSize; ++i) {
		szDiskSerial[pos] = (CHAR) (dwDiskData[i] / 256);
		if (' ' != szDiskSerial[pos]) {	// 不接受空格
			pos ++;
		}

		szDiskSerial[pos] = (CHAR) (dwDiskData[i] % 256);
		if (' ' != szDiskSerial[pos]) {	// 不接受空格
			pos ++;
		}
	}

	szDiskSerial[pos] = '\0';
}

void __flipAndCodeBytes(LPSTR szResult, LPCSTR cszStr)
{
	int num = strlen(cszStr);
	int nPos = 0;

	for (int i = 0; i < num; i += 4) {
		for (int j = 1; j >= 0; --j) {
			int sum = 0;
			for (int k = 0; k < 2; ++k) {
				sum *= 16;
				switch(cszStr[i + j * 2 + k]) {
				case '0': sum += 0; break;
				case '1': sum += 1; break;
				case '2': sum += 2; break;
				case '3': sum += 3; break;
				case '4': sum += 4; break;
				case '5': sum += 5; break;
				case '6': sum += 6; break;
				case '7': sum += 7; break;
				case '8': sum += 8; break;
				case '9': sum += 9; break;
				case 'a': sum += 10; break;
				case 'b': sum += 11; break;
				case 'c': sum += 12; break;
				case 'd': sum += 13; break;
				case 'e': sum += 14; break;
				case 'f': sum += 15; break;
				case 'A': sum += 10; break;
				case 'B': sum += 11; break;
				case 'C': sum += 12; break;
				case 'D': sum += 13; break;
				case 'E': sum += 14; break;
				case 'F': sum += 15; break;
				default: break;
				}
			}

			if (sum > 0 && ' ' != (CHAR) sum)  {
				szResult[nPos++] = (CHAR)sum;
			}
		}
	}

	szResult[nPos] = '\0';
}

void __flipBlankSpace(LPSTR szResult, LPCSTR cszStr)
{
	int num = strlen(cszStr);
	int nPos = 0;

	for (int i = 0; i < num; i += 4) {
		if (' ' != cszStr[i]) {
			szResult[nPos++] = cszStr[i];
		}
	}

	szResult[nPos] = '\0';
}

} // end of namespace jencrypt
