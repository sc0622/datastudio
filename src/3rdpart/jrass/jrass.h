#ifndef JRASS_H
#define JRASS_H

#ifdef JRASS_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef JRASS_BUILD
#       define JRASS_EXPORT __declspec(dllexport)
#   else
#       define JRASS_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "jrassd.lib")
#       else
#           pragma comment(lib, "jrass.lib")
#       endif
#   endif // !JRASS_BUILD
#endif // _MSC_VER || ...
#endif // JRASS_LIB

#ifndef JRASS_EXPORT
#define JRASS_EXPORT
#endif

#include <stdlib.h>

struct ModelVarDesc;
struct FileDesc;
struct MonitorGroupDesc;
class LogInfo;
class MappingVarDesc;

typedef int (*FuncMonitorGotNewData)(const char *monListName, double time);
typedef int (*FuncAllServerMonitorGotNewData)(long long id, const char *monListName, double time);
typedef int (*FuncFindNewServer)(const char *name, const char *addr, unsigned short port);

// class JRassWrap

class JRassWrapData;

class JRASS_EXPORT JRassWrap
{
public:
    JRassWrap();
    ~JRassWrap();

    int connect(const char *address, int port = 0, const char *localIf = "");
    int close();

    // for model control
    int load(const char *modelPath, const char *modelName, int waitTime,
             bool clearVarLogTable = true, bool clearVarMappingTable = true,
             bool clearVarMonitorTable = true, bool forceClearLogMappingMonitorVars = false,
             double freqScale = 1, bool freeRun = false);
    int getLoadedInfo(char *path, unsigned int pathLen, char *name, unsigned int nameLenth, int waitTime = -1);
    int unload(bool reserveLogMappingMonitor, int waitTime = -1);
    int reloadSymbolTable(int waitTime = -1);
    int getModelBaseFreq(double &freqTime, int waitTime = -1);
    int setModelRunMode(double freqScale = 1, bool freeRun = false, int waitTime = -1);
    int getModelRunMode(double &freqScale, bool &freeRun, int waitTime = -1);
    int syncServer(int waitTime = -1);
    int getAllVarsList(char **&vars, int &count);
    int getAllVarsListFreeRes(char **&vars, int count);
    int init(int waitTime = -1);
    int run(int waitTime = -1);
    int pause(int waitTime = -1);
    int step(unsigned int step, int waitTime = -1);
    int stepSync(unsigned int step, int waitTime = -1);
    int stop(int waitTime = -1);
    int getModelRunStatus(bool &haveLoadModel, int &modelStatus, int waitTime = -1);
    int getStepRunningStatus(bool &isRunningStep, int waitTime = -1);
    int setModelRunTimeLimit(double timeLimit, int waitTime = -1);
    int getModelRunTimeLimit(double &timeLimit, int waitTime = -1);
    int getCurrentModelRunTime(double &time, int waitTime = -1);
    int SetPauseCheckExpress(const char *express, int waitTime = -1);
    int getPauseCheckExpress(char *express, int &expressLength, int waitTime = -1);
    int clearPauseCheckExpress(int waitTime = -1);
    int setEnablePauseCheck(bool enabled, int waitTime = -1);
    int isEnablePauseCheck(bool &enabled, int waitTime = -1);
    int resetModelRunTimeStatInfo(int waitTime = -1);
    int setModelCanModifyItselfSymbolTable(bool enabled, int waitTime = -1);

    // modify variable

    int isVarOperable(const char *varName, bool &isOperable, int waitTime = -1);
    int getVarDes(const char *varName, ModelVarDesc &varDes, int waitTime = -1);
    int resetVarValue(const char *varName, int waitTime = -1);
    int resetAllVarValues(int waitTime = -1);

    int modifyVar(const char *varName, bool value, int waitTime = -1);
    int modifyVar(const char *varName, char value, int waitTime = -1);
    int modifyVar(const char *varName, unsigned char value, int waitTime = -1);
    int modifyVar(const char *varName, short value, int waitTime = -1);
    int modifyVar(const char *varName, unsigned short value, int waitTime = -1);
    int modifyVar(const char *varName, int value, int waitTime = -1);
    int modifyVar(const char *varName, unsigned int value, int waitTime = -1);
    int modifyVar(const char *varName, float value, int waitTime = -1);
    int modifyVar(const char *varName, double value, int waitTime = -1);

    int modifyVarComplex(const char *varName, char real, char imag, int waitTime = -1);
    int modifyVarComplex(const char *varName, unsigned char real, unsigned char imag, int waitTime = -1);
    int modifyVarComplex(const char *varName, short real, short imag, int waitTime = -1);
    int modifyVarComplex(const char *varName, unsigned short real, unsigned short imag, int waitTime = -1);
    int modifyVarComplex(const char *varName, int real, int imag, int waitTime = -1);
    int modifyVarComplex(const char *varName, unsigned int real, unsigned int imag, int waitTime = -1);
    int modifyVarComplex(const char *varName, float real, float imag, int waitTime = -1);
    int modifyVarComplex(const char *varName, double real, double imag, int waitTime = -1);

    int modifyVarFixPoint(const char *varName, double value, int waitTime = -1);
    int modifyVarComplexFixPoint(const char *varName, double real, double imag, int waitTime = -1);

    int modifyVar(const char *varName, void *pNewValue, unsigned int size, int waitTime);

    int readVar(const char *varName, bool &value, int waitTime = -1);
    int readVar(const char *varName, char &value, int waitTime = -1);
    int readVar(const char *varName, unsigned char &value, int waitTime = -1);
    int readVar(const char *varName, short &value, int waitTime = -1);
    int readVar(const char *varName, unsigned short &value, int waitTime = -1);
    int readVar(const char *varName, int &value, int waitTime = -1);
    int readVar(const char *varName, unsigned int &value, int waitTime = -1);
    int readVar(const char *varName, float &value, int waitTime = -1);
    int readVar(const char *varName, double &value, int waitTime = -1);

    int readVarComplex(const char *varName, char &real, char &imag, int waitTime = -1);
    int readVarComplex(const char *varName, unsigned char &real, unsigned char &imag, int waitTime = -1);
    int readVarComplex(const char *varName, short &real, short& imag, int waitTime = -1);
    int readVarComplex(const char *varName, unsigned short &real, unsigned short &imag, int waitTime = -1);
    int readVarComplex(const char *varName, int &real, int &imag, int waitTime = -1);
    int readVarComplex(const char *varName, unsigned int &real, unsigned int &imag, int waitTime = -1);
    int readVarComplex(const char *varName, float &real, float &imag, int waitTime = -1);
    int readVarComplex(const char *varName, double &real, double &imag, int waitTime = -1);

    int readVarFixPoint(const char *varName, double &value, int waitTime = -1);
    int readVarComplexFixPoint(const char *varName, double &real, double &imag, int waitTime = -1);

    int readVar(const char *varName, void *pNewValue, unsigned int size, int waitTime);

    int setVarEntireItem(const char *varName, bool value, int waitTime = -1);
    int setVarEntireItem(const char *varName, char value, int waitTime = -1);
    int setVarEntireItem(const char *varName, unsigned char value, int waitTime = -1);
    int setVarEntireItem(const char *varName, short value, int waitTime = -1);
    int setVarEntireItem(const char *varName, unsigned short value, int waitTime = -1);
    int setVarEntireItem(const char *varName, int value, int waitTime = -1);
    int setVarEntireItem(const char *varName, unsigned int value, int waitTime = -1);
    int setVarEntireItem(const char *varName, float value, int waitTime = -1);
    int setVarEntireItem(const char *varName, double value, int waitTime = -1);

    int saveModelVarStateToServerFile(const char *fileName, int waitTime = -1);
    int loadModelVarStateFromServerFile(const char *fileName, int waitTime = -1);
    int loadModelVarStateFromLocalFile(const char *fileName, int &successCount, int &failCount, int waitTime = -1);

    // log

    int setLogMasterFileName(const char *fileName, int waitTime = -1);
    int getLogMasterFileName(char *fileName, int size, int waitTime = -1);
    int getLogFinalFileName(char *fileName, int size, int waitTime = -1);
    int initLog(int waitTime = -1);
    int closeLog(int waitTime = -1);
    int addLogVar(const char *varName, int waitTime = -1);
    int delLogVar(const char *varName, int waitTime = -1);
    int delAllLogVar(int waitTime = -1);
    int startLog(int waitTime = -1);
    int stopLog(int waitTime = -1);
    int setLogIntervalTime(int interval, int waitTime = -1);
    int getLogIntervalTime(int &interval, int waitTime = -1);
    int addLogVarFromFile(const char *fileName, int waitTime = -1);
    int setLogFileNameUnTimeAppend(bool flag, int waitTime = -1);
    int isLogFileNameTimeAppend(bool &flag, int waitTime = -1);
    int getLogVarList(char **&logVars, int &count, int waitTime = -1);
    int getLogVarListFreeRes(char **&logVars, int count);
    int getLogInfo(LogInfo &logInfos, int waitTime = -1);

    // mapping

    int addMappingVar(const char *varName, unsigned int direction, unsigned int addr,
                      unsigned int mediumType, unsigned int interval, int waitTime = -1);
    int delMappingVar(const char *varName, int waitTime = -1);
    int delAllMappingVar(int waitTime = -1);
    int startMapping(int waitTime = -1);
    int stopMapping(int waitTime = -1);
    int isMappingStarted(bool &enabled, int waitTime = -1);
    int clearShareMemoryByAddr(unsigned int shareMemType, unsigned int beginAddr, unsigned int endAddr,
                               int waitTime = -1);
    int clearShareMemoryOfMappingVars(unsigned int cleanFlag, int waitTime = -1);
    int addMappingVarFromFile(const char *fileName, int waitTime = -1);
    int getMappingVarList(MappingVarDesc *&vars, int &count, int waitTime = -1);
    int getMappingVarListFreeRes(MappingVarDesc *&vars);

    // gmon

    int addGMonVar(const char *varName, int waitTime = -1);
    int delGMonVar(const char *varName, int waitTime = -1);
    int delAllGMonVar(int waitTime = -1);
    int enableGMon(int waitTime = -1);
    int disableGMon(int waitTime = -1);
    int setGMonIntervalTime(unsigned int interval, int waitTime = -1);
    int isGMonEnable(bool &enable, int waitTime = -1);
    int getGMonIntervalTime(unsigned int &interval, int waitTime = -1);
    static void setMCastIp(const char *localIp, const char *mcastIp = "255.5.5.5", unsigned short port = 22000);
    int startGMonAllServer();
    int stopGMonAllServer();

    // cmon
    int openCMon(const char *groupName, int waitTime = -1);
    int closeCMon(const char *groupName, int waitTime = -1);
    int addCMonVar(const char *groupName, const char *varName, int waitTime = -1);
    int delCMonVar(const char *groupName, const char *varName, int waitTime = -1);
    int delAllCMonVar(const char *groupName, int waitTime = -1);
    int enableCMon(const char *groupName, int waitTime = -1);
    int disableCMon(const char *groupName, int waitTime = -1);
    int setCMonIntervalTime(const char *groupName, unsigned int interval, int waitTime = -1);
    int isCMonEnable(const char *groupName, bool &enable, int waitTime = -1);
    int getCMonIntervalTime(const char *groupName, unsigned int &interval, int waitTime = -1);
    int closeAllCMon(int waitTime = -1);
    int getAllCMonDes(MonitorGroupDesc *&groupDescs, int &count, int waitTime = -1);
    int getAllCMonDesFreeRes(MonitorGroupDesc *&groupDescs);
    int forceCMonRefreshOneFrameData(int waitTime = -1);

    static void setFindNewServerCallback(FuncFindNewServer callback);
    static void setAllServerMonitorReadDataCallback(FuncAllServerMonitorGotNewData callback);
    void setMonitorReadDataCallback(FuncMonitorGotNewData callback);

    static int getMonData(const char *varName, bool &data);
    static int getMonData(const char *varName, char &data);
    static int getMonData(const char *varName, unsigned char &data);
    static int getMonData(const char *varName, short &data);
    static int getMonData(const char *varName, unsigned short &data);
    static int getMonData(const char *varName, int &data);
    static int getMonData(const char *varName, unsigned int &data);
    static int getMonData(const char *varName, float &data);
    static int getMonData(const char *varName, double &data);
    static int getMonData(const char *varName, void *pData, unsigned int length,
                          unsigned int type);

    // target machine status management

    int rebootMachine();
    int saveLogMapMonitorStatus(int waitTime = -1);
    int isServerValid(bool &valid);
    bool isConnected();
    long long getId();
    bool isVersionSameOfServer(int waitTime = -1);
    int setFloatPrecision(double prec, int waitTime = -1);
    int getFloatPrecision(double &prec, int waitTime = -1);

    // file operation [ftp]

    int ftpGet(const char *fileName, const char *toPath, int waitTime = -1);
    int ftpPut(const char *fileName, int waitTime = -1);
    int ftpExists(const char *fileName, bool &exists, int waitTime = -1);
    int ftpIsDir(const char *fileName, bool &isDir, int waitTime = -1);
    int ftpCD(const char *path, bool isRelativeDir, int waitTime = -1);
    int ftpMkdir(const char *path, int waitTime = -1);
    int ftpPwd(char *path, unsigned int size, int waitTime = -1);
    int ftpList(FileDesc *&fileDescs, int &count, int waitTime = -1);
    int ftpListFreeRes(FileDesc *&fileDescs);
    int ftpDel(const char *fileName, int waitTime = -1);
    int ftpRd(const char *fileName, int waitTime = -1);

    // express management

    int insertExpress(const char *express, int &expressId, int waitTime = -1);
    int deleteExpress(const char *express, int waitTime = -1);
    int deleteExpress(int expressId, int waitTime = -1);
    int deleteAllExpress(int waitTime = -1);
    int getAllExpress(char **&expresses, int &expressCount, int *&expressIds,
                      int &expressIdCount, int waitTime = -1);
    int getAllExpressFreeRes(char **&expresses, int expressCount, int *&expressIds);

private:
    JRassWrapData *d;
};

// class JRass

class JRass
{
public:
    enum VarClass {
        VC_Unknown = -1,
        VC_Signal = 1,
        VC_ModuleProperty = 2,
    };

    enum ModelVarType {
        MV_Invalid = -1,
        MV_Double,
        MV_Float,
        MV_Char,
        MV_UChar,
        MV_Short,
        MV_UShort,
        MV_Int,
        MV_UInt,
        MV_Bool,
        MV_Fixpoint,
        MV_Struct
    };

    enum ModelStatus {
        Srv_Unknown = 0,
        Srv_Run = 1,
        Srv_Ready = 2,
        Srv_Step = 3,
        Srv_Stop = 4
    };

    JRass() : d(new JRassWrap()) {}
    ~JRass() { delete d; }

private:
    JRass(const JRass &);
    JRass &operator=(const JRass &);
    JRassWrap *d;
};

#pragma pack(push)
#pragma pack(1)
// 变量描述
struct ModelVarDesc
{
    unsigned char   varClass;       // 变量类型，是否是信号 (0: 不是；1：是)
    unsigned char   dataType;       // 模型变量类型 (0:double, 1:float, 2:char, 3:uchar, 4:short, 5:ushort, 6:int, 7:uint, 8:bool, 200:fixpoint, 255:struct)
    unsigned short  dataSize;       // 模型变量每项大小,总大小为维数的乘积和数据项大小的乘积
    unsigned int    nDims;          // 几维
    unsigned int    dim[10];        // 维数
    bool            isComplex;      // 是否是复数
    bool            isPointer;      // 是否需要指针访问
    bool            isFixPointData; // 是否是定点数

    ModelVarDesc()
        : varClass(0), dataType(-1), nDims(0)
        , isComplex(false), isPointer(false), isFixPointData(false)
    {

    }

    ModelVarDesc(const ModelVarDesc *other)
    {
        *this = other;
    }

    const ModelVarDesc &operator =(const ModelVarDesc &other)
    {
        if (this == &other) {
            return *this;
        }
        varClass = other.varClass;
        dataType = other.dataType;
        dataSize = other.dataSize;
        nDims = other.nDims;
        memcpy(dim, other.dim, sizeof(unsigned int) * 10);
        isComplex = other.isComplex;
        isPointer = other.isPointer;
        isFixPointData = other.isFixPointData;
        return *this;
    }

    bool isNull() const
    {
        return (dataType == JRass::MV_Invalid);
    }

    bool isValid() const
    {
        return (dataType > JRass::MV_Invalid);
    }

    bool IsSimpleVar(void) {
        if (isComplex || isPointer || isFixPointData) {
            return false;
        }

        if (dataType == 255) {
            return false;
        }

        unsigned int size = 1;
        for (unsigned int i = 0; i < nDims; ++i) {
            size *= dim[i];
        }
        if (1 != size) {
            return false;
        }

        return true;
    }

    void reset()
    {
        dataType = JRass::MV_Invalid;
    }

    bool dimToString(char *str, int size) const
    {
        if (!str) {
            return false;
        }

        int count = nDims;
        if (count > 10) {
            count = 10;
        }

        if (size < count * 10) {
            return false;
        }

        char *_str = str;

        for (unsigned int i = 0; i < count; ++i) {
            char buffer[10];
            memset(buffer, 0, sizeof(char) * 10);
            char *tmp = itoa(dim[i], buffer, 10);
            if (!tmp) {
                return false;
            }

            const int length = strlen(tmp);
            memcpy(_str, tmp, length);
            _str += length;
            if (i < count - 1) {
                *_str++ = 'x';
            }
        }
        return true;
    }
};

// 文件描述结构
struct FileDesc
{
    long long int fileSize;         // 文件大小
    char          fileName[512];    // 文件名
    bool          isDirectory;      // 是否路径
    unsigned int  year;             // 年
    unsigned char month;            // 月
    unsigned char day;              // 日
    unsigned char hour;             // 时
    unsigned char minute;           // 分
    unsigned char second;           // 秒
};

// 映射变量结构描述
#define MAPPINGVAR_NAME_LENGTH 512
struct MappingVarDesc
{
    char name[MAPPINGVAR_NAME_LENGTH]; // 映射的模型可操作变量名称
    unsigned int direction;         // 映射的方向，1:模型到内存 2:内存到模型
    unsigned int shareMemType;      // 共享内存类型，1表示反射内存，2表示UDP共享内存，3表示本地共享内存
    unsigned int mapAddress;        // 共享内存地址
    unsigned int length;            // 映射的长度
    unsigned int intervalCount;     // 变量映射间隔次数，即在次数的整数倍上才进行映射操作，提供用户低速率信号要求，但是过高的映射速度占用极大带宽，甚至模型运行超时

    MappingVarDesc()
    {

    }

    MappingVarDesc(const MappingVarDesc &other)
    {
        *this = other;
    }

    const MappingVarDesc &operator =(const MappingVarDesc &other)
    {
        if (this == &other) {
            return *this;
        }
        memcpy(name, other.name, sizeof(name));
        direction = other.direction;
        shareMemType = other.shareMemType;
        mapAddress = other.mapAddress;
        length = other.length;
        intervalCount = other.intervalCount;
        return *this;
    }
};

// 记录信息结构
struct LogInfo
{
    bool         bLogStarted;                   // 记录是否已经开始，开始表示记录文件已经打开
    bool         bLogEnable;                    // 记录是否使能，表示记录使能。
    char         logMasterFileName[1024];       // 记录主文件名
    bool         bDoNotAutoGenTimeFileName;     // 记录文件名是否使用时间，配合主文件名生成最终文件名
    char         logFileName[1024];             // 记录最终文件名
    int          logIntervalTime;               // 间隔记录周期，为基频率的整数倍
    unsigned int logVarsStatusCnt;              // 记录队列的变更计数器，发现数据比本地更新，那么需要进行记录变量列表同步
};

#pragma pack(pop)

// 映射队列信息描述
struct MonitorGroupDesc
{
    char *name;
    char** vars;
    int varCount;

    MonitorGroupDesc() : name(nullptr), vars(nullptr), varCount(0) {}
    ~MonitorGroupDesc()
    {
        if (name) {
            delete[] name;
        }

        if (vars) {
            for (int i = 0; i < varCount; ++i) {
                delete[] vars[i];
            }
            delete[] vars;
        }
    }

    MonitorGroupDesc(const MonitorGroupDesc &other)
    {
        *this = other;
    }

    const MonitorGroupDesc &operator =(const MonitorGroupDesc &other)
    {
        if (this == &other) {
            return *this;
        }
        // name
        if (name) {
            delete[] name;
            name = nullptr;
        }
        if (other.name) {
            int length = strlen(other.name);
            name = new char[length + 1];
            memcpy(other.name, name, length);
            name[length] = '\0';
        }
        // vars
        if (vars) {
            for (int i = 0; i < varCount; ++i) {
                delete[] vars[i];
            }
            delete[] vars;
        }
        if (other.vars && other.varCount > 0) {
            vars = new char*[other.varCount];
            for (int i = 0; i < other.varCount; ++i) {
                char *_var = other.vars[i];
                const int length = strlen(_var);
                char *newVar = new char[length + 1];
                memcpy(newVar, _var, length);
                newVar[length] = '\0';
                vars[i] = newVar;
            }
        }
        return *this;
    }
};

#endif // JRASS_H
