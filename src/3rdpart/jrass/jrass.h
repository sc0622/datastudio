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
struct MonitorGroupDescNative;
struct LogInfo;
struct MappingVarDesc;

typedef int (*FuncMonitorGotNewData)(const char *monListName, double time);
typedef int (*FuncAllServerMonitorGotNewData)(long long id, const char *monListName, double time);
typedef int (*FuncFindNewServer)(const char *name, const char *addr, unsigned short port);

#ifndef NONE_ENOUGH_MEMORY
#define NONE_ENOUGH_MEMORY -200     //如果在返回字符串的时候，缓存空间不够，返回此值
#endif

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
    int getLoadedInfo(char *path, unsigned int pathLen, char *name, unsigned int nameLenth,
                      int waitTime = -1);
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
    int setPauseCheckExpress(const char *express, int waitTime = -1);
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
    static int startGMonAllServer();
    static int stopGMonAllServer();

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
    int getAllCMonDes(MonitorGroupDescNative *&groupDescs, int &count, int waitTime = -1);
    int getAllCMonDesFreeRes(MonitorGroupDescNative *&groupDescs);
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

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

// class JRass

struct MonitorGroupDesc;

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

    JRass() {}
    ~JRass() {}

    int connect(const std::string &address, int port = 0, const std::string &localIf = "");
    int close();

    // for model control
    inline int load(const std::string &modelPath, const std::string &modelName, int waitTime,
                    bool clearVarLogTable = true, bool clearVarMappingTable = true,
                    bool clearVarMonitorTable = true, bool forceClearLogMappingMonitorVars = false,
                    double freqScale = 1, bool freeRun = false);
    inline int getLoadedInfo(std::string &path, std::string &name, int waitTime = -1);
    inline int unload(bool reserveLogMappingMonitor, int waitTime = -1);
    inline int reloadSymbolTable(int waitTime = -1);
    inline int getModelBaseFreq(double &freqTime, int waitTime = -1);
    inline int setModelRunMode(double freqScale = 1, bool freeRun = false, int waitTime = -1);
    inline int getModelRunMode(double &freqScale, bool &freeRun, int waitTime = -1);
    inline int syncServer(int waitTime = -1);
    inline int getAllVarsList(std::vector<std::string> &vars);
    inline int init(int waitTime = -1);
    inline int run(int waitTime = -1);
    inline int pause(int waitTime = -1);
    inline int step(unsigned int step, int waitTime = -1);
    inline int stepSync(unsigned int step, int waitTime = -1);
    inline int stop(int waitTime = -1);
    inline int getModelRunStatus(bool &haveLoadModel, int &modelStatus, int waitTime = -1);
    inline int getStepRunningStatus(bool &isRunningStep, int waitTime = -1);
    inline int setModelRunTimeLimit(double timeLimit, int waitTime = -1);
    inline int getModelRunTimeLimit(double &timeLimit, int waitTime = -1);
    inline int getCurrentModelRunTime(double &time, int waitTime = -1);
    inline int setPauseCheckExpress(const std::string &express, int waitTime = -1);
    inline int getPauseCheckExpress(std::string &express, int waitTime = -1);
    inline int clearPauseCheckExpress(int waitTime = -1);
    inline int setEnablePauseCheck(bool enabled, int waitTime = -1);
    inline int isEnablePauseCheck(bool &enabled, int waitTime = -1);
    inline int resetModelRunTimeStatInfo(int waitTime = -1);
    inline int setModelCanModifyItselfSymbolTable(bool enabled, int waitTime = -1);

    // modify variable

    inline int isVarOperable(const std::string &varName, bool &isOperable, int waitTime = -1);
    inline int getVarDes(const std::string &varName, ModelVarDesc &varDes, int waitTime = -1);
    inline int resetVarValue(const std::string &varName, int waitTime = -1);
    inline int resetAllVarValues(int waitTime = -1);

    inline int modifyVar(const std::string &varName, bool value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, char value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, unsigned char value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, short value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, unsigned short value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, int value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, unsigned int value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, float value, int waitTime = -1);
    inline int modifyVar(const std::string &varName, double value, int waitTime = -1);

    inline int modifyVarComplex(const std::string &varName, char real, char imag, int waitTime = -1);
    inline int modifyVarComplex(const std::string &varName, unsigned char real, unsigned char imag, int waitTime = -1);
    inline int modifyVarComplex(const std::string &varName, short real, short imag, int waitTime = -1);
    inline int modifyVarComplex(const std::string &varName, unsigned short real, unsigned short imag, int waitTime = -1);
    inline int modifyVarComplex(const std::string &varName, int real, int imag, int waitTime = -1);
    inline int modifyVarComplex(const std::string &varName, unsigned int real, unsigned int imag, int waitTime = -1);
    inline int modifyVarComplex(const std::string &varName, float real, float imag, int waitTime = -1);
    inline int modifyVarComplex(const std::string &varName, double real, double imag, int waitTime = -1);

    inline int modifyVarFixPoint(const std::string &varName, double value, int waitTime = -1);
    inline int modifyVarComplexFixPoint(const std::string &varName, double real, double imag, int waitTime = -1);

    inline int modifyVar(const std::string &varName, void *pNewValue, unsigned int size, int waitTime);

    inline int readVar(const std::string &varName, bool &value, int waitTime = -1);
    inline int readVar(const std::string &varName, char &value, int waitTime = -1);
    inline int readVar(const std::string &varName, unsigned char &value, int waitTime = -1);
    inline int readVar(const std::string &varName, short &value, int waitTime = -1);
    inline int readVar(const std::string &varName, unsigned short &value, int waitTime = -1);
    inline int readVar(const std::string &varName, int &value, int waitTime = -1);
    inline int readVar(const std::string &varName, unsigned int &value, int waitTime = -1);
    inline int readVar(const std::string &varName, float &value, int waitTime = -1);
    inline int readVar(const std::string &varName, double &value, int waitTime = -1);

    inline int readVarComplex(const std::string &varName, char &real, char &imag, int waitTime = -1);
    inline int readVarComplex(const std::string &varName, unsigned char &real, unsigned char &imag, int waitTime = -1);
    inline int readVarComplex(const std::string &varName, short &real, short& imag, int waitTime = -1);
    inline int readVarComplex(const std::string &varName, unsigned short &real, unsigned short &imag, int waitTime = -1);
    inline int readVarComplex(const std::string &varName, int &real, int &imag, int waitTime = -1);
    inline int readVarComplex(const std::string &varName, unsigned int &real, unsigned int &imag, int waitTime = -1);
    inline int readVarComplex(const std::string &varName, float &real, float &imag, int waitTime = -1);
    inline int readVarComplex(const std::string &varName, double &real, double &imag, int waitTime = -1);

    inline int readVarFixPoint(const std::string &varName, double &value, int waitTime = -1);
    inline int readVarComplexFixPoint(const std::string &varName, double &real, double &imag, int waitTime = -1);

    inline int readVar(const std::string &varName, void *pNewValue, unsigned int size, int waitTime);

    inline int setVarEntireItem(const std::string &varName, bool value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, char value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, unsigned char value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, short value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, unsigned short value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, int value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, unsigned int value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, float value, int waitTime = -1);
    inline int setVarEntireItem(const std::string &varName, double value, int waitTime = -1);

    inline int saveModelVarStateToServerFile(const std::string &fileName, int waitTime = -1);
    inline int loadModelVarStateFromServerFile(const std::string &fileName, int waitTime = -1);
    inline int loadModelVarStateFromLocalFile(const std::string &fileName, int &successCount,
                                              int &failCount, int waitTime = -1);

    // log

    inline int setLogMasterFileName(const std::string &fileName, int waitTime = -1);
    inline int getLogMasterFileName(std::string &fileName, int waitTime = -1);
    inline int getLogFinalFileName(std::string &fileName, int waitTime = -1);
    inline int initLog(int waitTime = -1);
    inline int closeLog(int waitTime = -1);
    inline int addLogVar(const std::string &varName, int waitTime = -1);
    inline int delLogVar(const std::string &varName, int waitTime = -1);
    inline int delAllLogVar(int waitTime = -1);
    inline int startLog(int waitTime = -1);
    inline int stopLog(int waitTime = -1);
    inline int setLogIntervalTime(int interval, int waitTime = -1);
    inline int getLogIntervalTime(int &interval, int waitTime = -1);
    inline int addLogVarFromFile(const std::string &fileName, int waitTime = -1);
    inline int setLogFileNameUnTimeAppend(bool flag, int waitTime = -1);
    inline int isLogFileNameTimeAppend(bool &flag, int waitTime = -1);
    inline int getLogVarList(std::vector<std::string> &logVars, int waitTime = -1);
    inline int getLogInfo(LogInfo &logInfos, int waitTime = -1);

    // mapping

    inline int addMappingVar(const std::string &varName, unsigned int direction, unsigned int addr,
                             unsigned int mediumType, unsigned int interval, int waitTime = -1);
    inline int delMappingVar(const std::string &varName, int waitTime = -1);
    inline int delAllMappingVar(int waitTime = -1);
    inline int startMapping(int waitTime = -1);
    inline int stopMapping(int waitTime = -1);
    inline int isMappingStarted(bool &enabled, int waitTime = -1);
    inline int clearShareMemoryByAddr(unsigned int shareMemType, unsigned int beginAddr, unsigned int endAddr,
                                      int waitTime = -1);
    inline int clearShareMemoryOfMappingVars(unsigned int cleanFlag, int waitTime = -1);
    inline int addMappingVarFromFile(const std::string &fileName, int waitTime = -1);
    inline int getMappingVarList(std::vector<MappingVarDesc> &vars, int waitTime = -1);

    // gmon

    inline int addGMonVar(const std::string &varName, int waitTime = -1);
    inline int delGMonVar(const std::string &varName, int waitTime = -1);
    inline int delAllGMonVar(int waitTime = -1);
    inline int enableGMon(int waitTime = -1);
    inline int disableGMon(int waitTime = -1);
    inline int setGMonIntervalTime(unsigned int interval, int waitTime = -1);
    inline int isGMonEnable(bool &enable, int waitTime = -1);
    inline int getGMonIntervalTime(unsigned int &interval, int waitTime = -1);
    inline static void setMCastIp(const std::string &localIp, const std::string &mcastIp = "255.5.5.5",
                                  unsigned short port = 22000);
    inline static int startGMonAllServer();
    inline static int stopGMonAllServer();

    // cmon
    inline int openCMon(const std::string &groupName, int waitTime = -1);
    inline int closeCMon(const std::string &groupName, int waitTime = -1);
    inline int addCMonVar(const std::string &groupName, const std::string &varName, int waitTime = -1);
    inline int delCMonVar(const std::string &groupName, const std::string &varName, int waitTime = -1);
    inline int delAllCMonVar(const std::string &groupName, int waitTime = -1);
    inline int enableCMon(const std::string &groupName, int waitTime = -1);
    inline int disableCMon(const std::string &groupName, int waitTime = -1);
    inline int setCMonIntervalTime(const std::string &groupName, unsigned int interval, int waitTime = -1);
    inline int isCMonEnable(const std::string &groupName, bool &enable, int waitTime = -1);
    inline int getCMonIntervalTime(const std::string &groupName, unsigned int &interval, int waitTime = -1);
    inline int closeAllCMon(int waitTime = -1);
    inline int getAllCMonDes(std::vector<MonitorGroupDesc> &groupDescs, int waitTime = -1);
    inline int forceCMonRefreshOneFrameData(int waitTime = -1);

    inline static void setFindNewServerCallback(FuncFindNewServer callback);
    inline static void setAllServerMonitorReadDataCallback(FuncAllServerMonitorGotNewData callback);
    inline void setMonitorReadDataCallback(FuncMonitorGotNewData callback);

    inline static int getMonData(const std::string &varName, bool &data);
    inline static int getMonData(const std::string &varName, char &data);
    inline static int getMonData(const std::string &varName, unsigned char &data);
    inline static int getMonData(const std::string &varName, short &data);
    inline static int getMonData(const std::string &varName, unsigned short &data);
    inline static int getMonData(const std::string &varName, int &data);
    inline static int getMonData(const std::string &varName, unsigned int &data);
    inline static int getMonData(const std::string &varName, float &data);
    inline static int getMonData(const std::string &varName, double &data);
    inline static int getMonData(const std::string &varName, void *pData, unsigned int length,
                                 unsigned int type);

    // target machine status management

    inline int rebootMachine();
    inline int saveLogMapMonitorStatus(int waitTime = -1);
    inline int isServerValid(bool &valid);
    inline bool isConnected();
    inline long long getId();
    inline bool isVersionSameOfServer(int waitTime = -1);
    inline int setFloatPrecision(double prec, int waitTime = -1);
    inline int getFloatPrecision(double &prec, int waitTime = -1);

    // file operation [ftp]

    inline int ftpGet(const std::string &fileName, const std::string &toPath, int waitTime = -1);
    inline int ftpPut(const std::string &fileName, int waitTime = -1);
    inline int ftpExists(const std::string &fileName, bool &exists, int waitTime = -1);
    inline int ftpIsDir(const std::string &fileName, bool &isDir, int waitTime = -1);
    inline int ftpCD(const std::string &path, bool isRelativeDir, int waitTime = -1);
    inline int ftpMkdir(const std::string &path, int waitTime = -1);
    inline int ftpPwd(std::string &path, int waitTime = -1);
    inline int ftpList(std::vector<FileDesc> &fileDescs, int waitTime = -1);
    inline int ftpDel(const std::string &fileName, int waitTime = -1);
    inline int ftpRd(const std::string &fileName, int waitTime = -1);

    // express management

    inline int insertExpress(const std::string &express, int &expressId, int waitTime = -1);
    inline int deleteExpress(const std::string &express, int waitTime = -1);
    inline int deleteExpress(int expressId, int waitTime = -1);
    inline int deleteAllExpress(int waitTime = -1);
    inline int getAllExpress(std::unordered_map<int, std::string> &expresses, int waitTime = -1);

private:
    JRass(const JRass &);
    JRass &operator=(const JRass &);
    JRassWrap d;
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

    std::string dimToString() const
    {
        std::stringstream ss;
        for (unsigned int i = 0; i < nDims; ++i) {
            ss << dim[0] << 'x';
        }
        return ss.str();
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

// 映射队列信息描述 [native]
struct MonitorGroupDescNative
{
    char *name;
    char** vars;
    int varCount;

    MonitorGroupDescNative() : name(nullptr), vars(nullptr), varCount(0) {}
    ~MonitorGroupDescNative()
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
    MonitorGroupDescNative(const MonitorGroupDescNative &other) { *this = other; }
    const MonitorGroupDescNative &operator =(const MonitorGroupDescNative &other)
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

// 映射队列信息描述

struct MonitorGroupDesc
{
    std::string name;
    std::vector<std::string> vars;

    MonitorGroupDesc() {}
    MonitorGroupDesc(const MonitorGroupDescNative &native)
    {
        // name
        name.clear();
        if (native.name) {
            name = std::string(native.name);
        }
        // vars
        vars.clear();
        if (native.vars && native.varCount > 0) {
            for (int i = 0; i < native.varCount; ++i) {
                vars.push_back(std::string(native.vars[i]));
            }
        }
    }

    MonitorGroupDesc(const MonitorGroupDesc &other) { *this = other; }
    MonitorGroupDesc &operator =(const MonitorGroupDesc &other)
    {
        if (this == &other) {
            return *this;
        }
        name = other.name;
        vars = other.vars;
        return *this;
    }
};

//

inline int JRass::connect(const std::string &address, int port, const std::string &localIf)
{
    return d.connect(address.c_str(), port, localIf.c_str());
}

inline int JRass::close()
{
    return d.close();
}

inline int JRass::load(const std::string &modelPath, const std::string &modelName,
                       int waitTime, bool clearVarLogTable, bool clearVarMappingTable,
                       bool clearVarMonitorTable, bool forceClearLogMappingMonitorVars,
                       double freqScale, bool freeRun)
{
    return d.load(modelPath.c_str(), modelName.c_str(), waitTime, clearVarLogTable, clearVarMappingTable,
                  clearVarMonitorTable, forceClearLogMappingMonitorVars, freqScale, freeRun);
}

inline int JRass::getLoadedInfo(std::string &path, std::string &name, int waitTime)
{
    char _path[256], _name[128];
    memset(_path, 0, 256);
    memset(_name, 0, 128);
    int result = d.getLoadedInfo(_path, 256, _name, 128, waitTime);
    if (result == 0) {
        path = std::string(_path);
        name = std::string(_name);
    }
    return result;
}

inline int JRass::unload(bool reserveLogMappingMonitor, int waitTime)
{
    return d.unload(reserveLogMappingMonitor, waitTime);
}

inline int JRass::reloadSymbolTable(int waitTime)
{
    return d.reloadSymbolTable(waitTime);
}

inline int JRass::getModelBaseFreq(double &freqTime, int waitTime)
{
    return d.getModelBaseFreq(freqTime, waitTime);
}

inline int JRass::setModelRunMode(double freqScale, bool freeRun, int waitTime)
{
    return d.setModelRunMode(freqScale, freeRun, waitTime);
}

inline int JRass::getModelRunMode(double &freqScale, bool &freeRun, int waitTime)
{
    return d.getModelRunMode(freqScale, freeRun, waitTime);
}

inline int JRass::syncServer(int waitTime)
{
    return d.syncServer(waitTime);
}

inline int JRass::getAllVarsList(std::vector<std::string> &vars)
{
    vars.clear();
    char **_vars = nullptr;
    int count = 0;
    int result = d.getAllVarsList(_vars, count);
    if (result == 0 && _vars && count > 0) {
        for (int i = 0; i < count; ++i) {
            vars.push_back(std::string(_vars[i]));
        }
        d.getAllVarsListFreeRes(_vars, count);
    }
    return result;
}

inline int JRass::init(int waitTime)
{
    return d.init(waitTime);
}

inline int JRass::run(int waitTime)
{
    return d.run(waitTime);
}

inline int JRass::pause(int waitTime)
{
    return d.pause(waitTime);
}

inline int JRass::step(unsigned int step, int waitTime)
{
    return d.step(step, waitTime);
}

inline int JRass::stepSync(unsigned int step, int waitTime)
{
    return d.stepSync(step, waitTime);
}

inline int JRass::stop(int waitTime)
{
    return d.stop(waitTime);
}

inline int JRass::getModelRunStatus(bool &haveLoadModel, int &modelStatus, int waitTime)
{
    return d.getModelRunStatus(haveLoadModel, modelStatus, waitTime);
}

inline int JRass::getStepRunningStatus(bool &isRunningStep, int waitTime)
{
    return d.getStepRunningStatus(isRunningStep, waitTime);
}

inline int JRass::setModelRunTimeLimit(double timeLimit, int waitTime)
{
    return d.setModelRunTimeLimit(timeLimit, waitTime);
}

inline int JRass::getModelRunTimeLimit(double &timeLimit, int waitTime)
{
    return d.getModelRunTimeLimit(timeLimit, waitTime);
}

inline int JRass::getCurrentModelRunTime(double &time, int waitTime)
{
    return d.getCurrentModelRunTime(time, waitTime);
}

inline int JRass::setPauseCheckExpress(const std::string &express, int waitTime)
{
    return d.setPauseCheckExpress(express.c_str(), waitTime);
}

inline int JRass::clearPauseCheckExpress(int waitTime)
{
    return d.clearPauseCheckExpress(waitTime);
}

inline int JRass::setEnablePauseCheck(bool enabled, int waitTime)
{
    return d.setEnablePauseCheck(enabled, waitTime);
}

inline int JRass::isEnablePauseCheck(bool &enabled, int waitTime)
{
    return d.isEnablePauseCheck(enabled, waitTime);
}

inline int JRass::resetModelRunTimeStatInfo(int waitTime)
{
    return d.resetModelRunTimeStatInfo(waitTime);
}

inline int JRass::setModelCanModifyItselfSymbolTable(bool enabled, int waitTime)
{
    return d.setModelCanModifyItselfSymbolTable(enabled, waitTime);
}

// modify variable

inline int JRass::isVarOperable(const std::string &varName, bool &isOperable, int waitTime)
{
    return d.isVarOperable(varName.c_str(), isOperable, waitTime);
}

inline int JRass::getVarDes(const std::string &varName, ModelVarDesc &varDes, int waitTime)
{
    return d.getVarDes(varName.c_str(), varDes, waitTime);
}

inline int JRass::resetVarValue(const std::string &varName, int waitTime)
{
    return d.resetVarValue(varName.c_str(), waitTime);
}

inline int JRass::resetAllVarValues(int waitTime)
{
    return d.resetAllVarValues(waitTime);
}

inline int JRass::modifyVar(const std::string &varName, bool value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, char value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, unsigned char value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, short value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, unsigned short value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, int value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, unsigned int value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, float value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, double value, int waitTime)
{
    return d.modifyVar(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, char real, char imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, unsigned char real, unsigned char imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, short real, short imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, unsigned short real, unsigned short imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, int real, int imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, unsigned int real, unsigned int imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, float real, float imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarComplex(const std::string &varName, double real, double imag, int waitTime)
{
    return d.modifyVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVarFixPoint(const std::string &varName, double value, int waitTime)
{
    return d.modifyVarFixPoint(varName.c_str(), value, waitTime);
}

inline int JRass::modifyVarComplexFixPoint(const std::string &varName, double real, double imag, int waitTime)
{
    return d.modifyVarComplexFixPoint(varName.c_str(), real, imag, waitTime);
}

inline int JRass::modifyVar(const std::string &varName, void *pNewValue, unsigned int size, int waitTime)
{
    return d.modifyVar(varName.c_str(), pNewValue, size, waitTime);
}

inline int JRass::readVar(const std::string &varName, bool &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, char &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, unsigned char &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, short &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, unsigned short &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, int &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, unsigned int &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, float &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVar(const std::string &varName, double &value, int waitTime)
{
    return d.readVar(varName.c_str(), value, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, char &real, char &imag, int waitTime)
{
    return d.readVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, unsigned char &real, unsigned char &imag, int waitTime)
{
    return d.readVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, short &real, short &imag, int waitTime)
{
    return d.readVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, unsigned short &real, unsigned short &imag, int waitTime)
{
    return d.readVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, int &real, int &imag, int waitTime)
{
    return d.readVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, unsigned int &real, unsigned int &imag, int waitTime)
{
    return d.readVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, float &real, float &imag, int waitTime)
{
    return d.readVarComplex(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarComplex(const std::string &varName, double &real, double &imag, int waitTime)
{
    return d.readVarComplexFixPoint(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVarFixPoint(const std::string &varName, double &value, int waitTime)
{
    return d.readVarFixPoint(varName.c_str(), value, waitTime);
}

inline int JRass::readVarComplexFixPoint(const std::string &varName, double &real, double &imag, int waitTime)
{
    return d.readVarComplexFixPoint(varName.c_str(), real, imag, waitTime);
}

inline int JRass::readVar(const std::string &varName, void *pNewValue, unsigned int size, int waitTime)
{
    return d.readVar(varName.c_str(), pNewValue, size, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, bool value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, char value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, unsigned char value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, short value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, unsigned short value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, int value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, unsigned int value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, float value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::setVarEntireItem(const std::string &varName, double value, int waitTime)
{
    return d.setVarEntireItem(varName.c_str(), value, waitTime);
}

inline int JRass::saveModelVarStateToServerFile(const std::string &fileName, int waitTime)
{
    return d.saveModelVarStateToServerFile(fileName.c_str(), waitTime);
}

inline int JRass::loadModelVarStateFromServerFile(const std::string &fileName, int waitTime)
{
    return d.loadModelVarStateFromServerFile(fileName.c_str(), waitTime);
}

inline int JRass::loadModelVarStateFromLocalFile(const std::string &fileName, int &successCount, int &failCount, int waitTime)
{
    return d.loadModelVarStateFromLocalFile(fileName.c_str(), successCount, failCount, waitTime);
}

// log

inline int JRass::setLogMasterFileName(const std::string &fileName, int waitTime)
{
    return d.setLogMasterFileName(fileName.c_str(), waitTime);
}

inline int JRass::getLogMasterFileName(std::string &fileName, int waitTime)
{
    char _fileName[256];
    memset(_fileName, 0, 256);
    int result = d.getLogMasterFileName(_fileName, waitTime);
    fileName = std::string(_fileName);
    return result;
}

inline int JRass::getLogFinalFileName(std::string &fileName, int waitTime)
{
    char _fileName[256];
    memset(_fileName, 0, 256);
    int result = d.getLogFinalFileName(_fileName, waitTime);
    fileName = std::string(_fileName);
    return result;
}

inline int JRass::initLog(int waitTime)
{
    return d.initLog(waitTime);
}

inline int JRass::closeLog(int waitTime)
{
    return d.closeLog(waitTime);
}

inline int JRass::addLogVar(const std::string &varName, int waitTime)
{
    return d.addLogVar(varName.c_str(), waitTime);
}

inline int JRass::delLogVar(const std::string &varName, int waitTime)
{
    return d.delLogVar(varName.c_str(), waitTime);
}

inline int JRass::delAllLogVar(int waitTime)
{
    return d.delAllLogVar(waitTime);
}

inline int JRass::startLog(int waitTime)
{
    return d.startLog(waitTime);
}

inline int JRass::stopLog(int waitTime)
{
    return d.stopLog(waitTime);
}

inline int JRass::setLogIntervalTime(int interval, int waitTime)
{
    return d.setLogIntervalTime(interval, waitTime);
}

inline int JRass::getLogIntervalTime(int &interval, int waitTime)
{
    return d.getLogIntervalTime(interval, waitTime);
}

inline int JRass::addLogVarFromFile(const std::string &fileName, int waitTime)
{
    return d.addLogVarFromFile(fileName.c_str(), waitTime);
}

inline int JRass::setLogFileNameUnTimeAppend(bool flag, int waitTime)
{
    return d.setLogFileNameUnTimeAppend(flag, waitTime);
}

inline int JRass::isLogFileNameTimeAppend(bool &flag, int waitTime)
{
    return d.isLogFileNameTimeAppend(flag, waitTime);
}

inline int JRass::getLogVarList(std::vector<std::string> &logVars, int waitTime)
{
    logVars.clear();
    char **_logVars = nullptr;
    int count = 0;
    int result = d.getLogVarList(_logVars, count, waitTime);
    if (result == 0 && _logVars && count > 0) {
        for (int i = 0; i < count; ++i) {
            logVars.push_back(std::string(_logVars[i]));
        }
        d.getLogVarListFreeRes(_logVars, count);
    }
    return result;
}

inline int JRass::getLogInfo(LogInfo &logInfos, int waitTime)
{
    return d.getLogInfo(logInfos, waitTime);
}

inline int JRass::addMappingVar(const std::string &varName, unsigned int direction,
                                unsigned int addr, unsigned int mediumType,
                                unsigned int interval, int waitTime)
{
    return d.addMappingVar(varName.c_str(), direction, addr, mediumType, interval, waitTime);
}

inline int JRass::delMappingVar(const std::string &varName, int waitTime)
{
    return d.delMappingVar(varName.c_str(), waitTime);
}

inline int JRass::delAllMappingVar(int waitTime)
{
    return d.delAllMappingVar(waitTime);
}

inline int JRass::startMapping(int waitTime)
{
    return d.startMapping(waitTime);
}

inline int JRass::stopMapping(int waitTime)
{
    return d.stopMapping(waitTime);
}

inline int JRass::isMappingStarted(bool &enabled, int waitTime)
{
    return d.isMappingStarted(enabled, waitTime);
}

inline int JRass::clearShareMemoryByAddr(unsigned int shareMemType, unsigned int beginAddr,
                                         unsigned int endAddr, int waitTime)
{
    return d.clearShareMemoryByAddr(shareMemType, beginAddr, endAddr, waitTime);
}

inline int JRass::clearShareMemoryOfMappingVars(unsigned int cleanFlag, int waitTime)
{
    return d.clearShareMemoryOfMappingVars(cleanFlag, waitTime);
}

inline int JRass::addMappingVarFromFile(const std::string &fileName, int waitTime)
{
    return d.addMappingVarFromFile(fileName.c_str(), waitTime);
}

inline int JRass::getMappingVarList(std::vector<MappingVarDesc> &vars, int waitTime)
{
    vars.clear();
    MappingVarDesc *_vars = nullptr;
    int count = 0;
    int result = d.getMappingVarList(_vars, count, waitTime);
    if (result == 0 && _vars && count > 0) {
        for (int i = 0; i < count; ++i) {
            vars.push_back(_vars[i]);
        }
        d.getMappingVarListFreeRes(_vars);
    }
    return result;
}

// global monitor

inline int JRass::addGMonVar(const std::string &varName, int waitTime)
{
    return d.addGMonVar(varName.c_str(), waitTime);
}

inline int JRass::delGMonVar(const std::string &varName, int waitTime)
{
    return d.delGMonVar(varName.c_str(), waitTime);
}

inline int JRass::delAllGMonVar(int waitTime)
{
    return d.delAllGMonVar(waitTime);
}

inline int JRass::enableGMon(int waitTime)
{
    return d.enableGMon(waitTime);
}

inline int JRass::disableGMon(int waitTime)
{
    return d.disableGMon(waitTime);
}

inline int JRass::setGMonIntervalTime(unsigned int interval, int waitTime)
{
    return d.setGMonIntervalTime(interval, waitTime);
}

inline int JRass::isGMonEnable(bool &enable, int waitTime)
{
    return d.isGMonEnable(enable, waitTime);
}

inline int JRass::getGMonIntervalTime(unsigned int &interval, int waitTime)
{
    return d.getGMonIntervalTime(interval, waitTime);
}

inline void JRass::setMCastIp(const std::string &localIp, const std::string &mcastIp, unsigned short port)
{
    JRassWrap::setMCastIp(localIp.c_str(), mcastIp.c_str(), port);
}

inline int JRass::startGMonAllServer()
{
    return JRassWrap::startGMonAllServer();
}

inline int JRass::stopGMonAllServer()
{
    return JRassWrap::stopGMonAllServer();
}

// group monitor

inline int JRass::openCMon(const std::string &groupName, int waitTime)
{
    return d.openCMon(groupName.c_str(), waitTime);
}

inline int JRass::closeCMon(const std::string &groupName, int waitTime)
{
    return d.closeCMon(groupName.c_str(), waitTime);
}

inline int JRass::addCMonVar(const std::string &groupName, const std::string &varName, int waitTime)
{
    return d.addCMonVar(groupName.c_str(), varName.c_str(), waitTime);
}

inline int JRass::delCMonVar(const std::string &groupName, const std::string &varName, int waitTime)
{
    return d.delCMonVar(groupName.c_str(), varName.c_str(), waitTime);
}

inline int JRass::delAllCMonVar(const std::string &groupName, int waitTime)
{
    return d.delAllCMonVar(groupName.c_str(), waitTime);
}

inline int JRass::enableCMon(const std::string &groupName, int waitTime)
{
    return d.enableCMon(groupName.c_str(), waitTime);
}

inline int JRass::disableCMon(const std::string &groupName, int waitTime)
{
    return d.disableCMon(groupName.c_str(), waitTime);
}

inline int JRass::setCMonIntervalTime(const std::string &groupName, unsigned int interval, int waitTime)
{
    return d.setCMonIntervalTime(groupName.c_str(), interval, waitTime);
}

inline int JRass::isCMonEnable(const std::string &groupName, bool &enable, int waitTime)
{
    return d.isCMonEnable(groupName.c_str(), enable, waitTime);
}

inline int JRass::getCMonIntervalTime(const std::string &groupName, unsigned int &interval, int waitTime)
{
    return d.getCMonIntervalTime(groupName.c_str(), interval, waitTime);
}

inline int JRass::closeAllCMon(int waitTime)
{
    return d.closeAllCMon(waitTime);
}

inline int JRass::getAllCMonDes(std::vector<MonitorGroupDesc> &groupDescs, int waitTime)
{
    groupDescs.clear();
    MonitorGroupDescNative *_groupDescs = nullptr;
    int count = 0;
    int result = d.getAllCMonDes(_groupDescs, count, waitTime);
    if (result == 0 && _groupDescs && count > 0) {
        for (int i = 0; i < count; ++i) {
            const MonitorGroupDescNative &_groupDesc = _groupDescs[i];
            groupDescs.push_back(MonitorGroupDesc());
            MonitorGroupDesc &groupDesc = *groupDescs.rbegin();
            // name
            groupDesc.name = std::string(_groupDesc.name);
            // vars
            for (int j = 0; j < _groupDesc.varCount; ++j) {
                groupDesc.vars.push_back(std::string(_groupDesc.vars[j]));
            }
        }
        d.getAllCMonDesFreeRes(_groupDescs);
    }
    return result;
}

inline int JRass::forceCMonRefreshOneFrameData(int waitTime)
{
    return d.forceCMonRefreshOneFrameData(waitTime);
}

inline void JRass::setFindNewServerCallback(FuncFindNewServer callback)
{
    JRassWrap::setFindNewServerCallback(callback);
}

inline void JRass::setAllServerMonitorReadDataCallback(FuncAllServerMonitorGotNewData callback)
{
    JRassWrap::setAllServerMonitorReadDataCallback(callback);
}

inline void JRass::setMonitorReadDataCallback(FuncMonitorGotNewData callback)
{
    d.setMonitorReadDataCallback(callback);
}

inline int JRass::getMonData(const std::string &varName, bool &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, char &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, unsigned char &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, short &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, unsigned short &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, int &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, unsigned int &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, float &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, double &data)
{
    return JRassWrap::getMonData(varName.c_str(), data);
}

inline int JRass::getMonData(const std::string &varName, void *pData, unsigned int length, unsigned int type)
{
    return JRassWrap::getMonData(varName.c_str(), pData, length, type);
}

// target machine status management

inline int JRass::rebootMachine()
{
    return d.rebootMachine();
}

inline int JRass::saveLogMapMonitorStatus(int waitTime)
{
    return d.saveLogMapMonitorStatus(waitTime);
}

inline int JRass::isServerValid(bool &valid)
{
    return d.isServerValid(valid);
}

inline bool JRass::isConnected()
{
    return d.isConnected();
}

inline long long JRass::getId()
{
    return d.getId();
}

inline bool JRass::isVersionSameOfServer(int waitTime)
{
    return d.isVersionSameOfServer(waitTime);
}

inline int JRass::setFloatPrecision(double prec, int waitTime)
{
    return d.setFloatPrecision(prec, waitTime);
}

inline int JRass::getFloatPrecision(double &prec, int waitTime)
{
    return d.getFloatPrecision(prec, waitTime);
}

// file operation [ftp]

inline int JRass::ftpGet(const std::string &fileName, const std::string &toPath, int waitTime)
{
    return d.ftpGet(fileName.c_str(), toPath.c_str(), waitTime);
}

inline int JRass::ftpPut(const std::string &fileName, int waitTime)
{
    return d.ftpPut(fileName.c_str(), waitTime);
}

inline int JRass::ftpExists(const std::string &fileName, bool &exists, int waitTime)
{
    return d.ftpExists(fileName.c_str(), exists, waitTime);
}

inline int JRass::ftpIsDir(const std::string &fileName, bool &isDir, int waitTime)
{
    return d.ftpIsDir(fileName.c_str(), isDir, waitTime);
}

inline int JRass::ftpCD(const std::string &path, bool isRelativeDir, int waitTime)
{
    return d.ftpCD(path.c_str(), isRelativeDir, waitTime);
}

inline int JRass::ftpMkdir(const std::string &path, int waitTime)
{
    return d.ftpMkdir(path.c_str(), waitTime);
}

inline int JRass::ftpPwd(std::string &path, int waitTime)
{
    char _path[512];
    memset(_path, 0, 512);
    int result = d.ftpPwd(_path, path.size(), waitTime);
    if (result == 0) {
        path = std::string(_path);
    }
    return result;
}

inline int JRass::ftpList(std::vector<FileDesc> &fileDescs, int waitTime)
{
    fileDescs.clear();
    FileDesc *_fileDescs = nullptr;
    int count = 0;
    int result = d.ftpList(_fileDescs, waitTime);
    if (result == 0 && _fileDescs && count > 0) {
        for (int i = 0; i < count; ++i) {
            fileDescs.push_back(_fileDescs[i]);
        }
        d.ftpListFreeRes(_fileDescs);
    }
    return result;
}

inline int JRass::ftpDel(const std::string &fileName, int waitTime)
{
    return d.ftpDel(fileName.c_str(), waitTime);
}

inline int JRass::ftpRd(const std::string &fileName, int waitTime)
{
    return d.ftpRd(fileName.c_str(), waitTime);
}

// express management

inline int JRass::insertExpress(const std::string &express, int &expressId, int waitTime)
{
    return d.insertExpress(express.c_str(), expressId, waitTime);
}

inline int JRass::deleteExpress(const std::string &express, int waitTime)
{
    return d.deleteExpress(express.c_str(), waitTime);
}

inline int JRass::deleteExpress(int expressId, int waitTime)
{
    return d.deleteExpress(expressId, waitTime);
}

inline int JRass::deleteAllExpress(int waitTime)
{
    return d.deleteAllExpress(waitTime);
}

inline int JRass::getAllExpress(std::unordered_map<int, std::string> &expresses, int waitTime)
{
    expresses.clear();
    char **_expresses = nullptr;
    int expressCount = 0;
    int *_expressIds = nullptr;
    int expressIdCount = 0;
    int result = d.getAllExpress(_expresses, expressCount, _expressIds, expressIdCount, waitTime);
    if (result == 0 && _expresses && expressCount > 0 && _expressIds && expressIdCount > 0) {
        for (int i = 0; i < expressCount && i < expressIdCount; ++i) {
            expresses[_expressIds[i]] = std::string(_expresses[i]);
        }
        d.getAllExpressFreeRes(_expresses, expressCount, _expressIds);
    }
    return result;
}

#endif // JRASS_H
