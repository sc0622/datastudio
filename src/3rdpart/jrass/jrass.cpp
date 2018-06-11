#include "jrass.h"
#include "private/rass.h"

static std::string charPtrToString(const char *str, int size)
{
    return std::string(str, size);
}

// JRassWrapData

class JRassWrapData
{
public:
    JRassWrapData()
        : rass(CRass::NewRef())
    {

    }

    ~JRassWrapData()
    {
        CRass::Delete(rass);
    }

    CRass &rass;
};

// class JRassWrap

JRassWrap::JRassWrap()
    : d(new JRassWrapData())
{

}

JRassWrap::~JRassWrap()
{
    delete d;
}

int JRassWrap::connect(const char *address, int port, const char *localIf)
{
    return d->rass.Connect(std::string(address), port, std::string(localIf));
}

int JRassWrap::close()
{
    return d->rass.Close();
}

int JRassWrap::load(const char *modelPath, const char *modelName, int waitTime,
                    bool clearVarLogTable, bool clearVarMappingTable, bool clearVarMonitorTable,
                    bool forceClearLogMappingMonitorVars, double freqScale, bool freeRun)
{
    return d->rass.Load(std::string(modelPath), std::string(modelName), waitTime,
                        clearVarLogTable, clearVarMappingTable, clearVarMonitorTable,
                        forceClearLogMappingMonitorVars, freqScale, freeRun);
}

int JRassWrap::getLoadedInfo(char *path, unsigned int pathLen, char *name, unsigned int nameLenth, int waitTime)
{
    return d->rass.GetLoadedInfo(path, pathLen, name, nameLenth, waitTime);
}

int JRassWrap::unload(bool reserveLogMappingMonitor, int waitTime)
{
    return d->rass.Unload(reserveLogMappingMonitor, waitTime);
}

int JRassWrap::reloadSymbolTable(int waitTime)
{
    return d->rass.ReloadSymbolTalbe(waitTime);
}

int JRassWrap::getModelBaseFreq(double &freqTime, int waitTime)
{
    return d->rass.GetModelBaseFreq(freqTime, waitTime);
}

int JRassWrap::setModelRunMode(double freqScale, bool freeRun, int waitTime)
{
    return d->rass.SetModelRunMode(freqScale, freeRun, waitTime);
}

int JRassWrap::getModelRunMode(double &freqScale, bool &freeRun, int waitTime)
{
    return d->rass.GetModelRunMode(freqScale, freeRun, waitTime);
}

int JRassWrap::syncServer(int waitTime)
{
    return d->rass.SyncServer(waitTime);
}

int JRassWrap::getAllVarsList(char **&vars, int &count)
{
    std::vector<std::string> *_vars = nullptr;
    int result = d->rass.GetAllVarsList(_vars);
    if (result == 0 && _vars) {
        count = int(_vars->size());
        vars = new char*[count];
        for (int i = 0; i < count; ++i) {
            const std::string &_var = _vars->at(i);
            const int size = int(_var.size());
            char *newVar = new char[size + 1];
            memcpy(newVar, _var.c_str(), size);
            newVar[size] = '\0';
            vars[i] = newVar;
        }
        d->rass.GetAllVarsList_FreeRes(_vars);
    }

    return result;
}

int JRassWrap::getAllVarsListFreeRes(char **&vars, int count)
{
    if (vars) {
        for (int i = 0; i < count; ++i) {
            delete[] vars[i];
        }
        delete[] vars;
        vars = nullptr;
    }

    return 0;
}

int JRassWrap::init(int waitTime)
{
    return d->rass.Init(waitTime);
}

int JRassWrap::run(int waitTime)
{
    return d->rass.Run(waitTime);
}

int JRassWrap::pause(int waitTime)
{
    return d->rass.Pause(waitTime);
}

int JRassWrap::step(unsigned int step, int waitTime)
{
    return d->rass.Step(step, waitTime);
}

int JRassWrap::stepSync(unsigned int step, int waitTime)
{
    return d->rass.StepSync(step, waitTime);
}

int JRassWrap::stop(int waitTime)
{
    return d->rass.Stop(waitTime);
}

int JRassWrap::getModelRunStatus(bool &haveLoadModel, int &modelStatus, int waitTime)
{
    SERVER_RUN_STATUS status;
    int result = d->rass.GetModelRunStatus(haveLoadModel, status, waitTime);
    modelStatus = status;
    return result;
}

int JRassWrap::getStepRunningStatus(bool &isRunningStep, int waitTime)
{
    return d->rass.GetStepRuningStatus(isRunningStep, waitTime);
}

int JRassWrap::setModelRunTimeLimit(double timeLimit, int waitTime)
{
    return d->rass.SetModelRunTimeLimit(timeLimit, waitTime);
}

int JRassWrap::getModelRunTimeLimit(double &timeLimit, int waitTime)
{
    return d->rass.GetModelRunTimeLimit(timeLimit, waitTime);
}

int JRassWrap::getCurrentModelRunTime(double &time, int waitTime)
{
    return d->rass.GetCurrentModelRunTime(time, waitTime);
}

int JRassWrap::setPauseCheckExpress(const char *express, int waitTime)
{
    return d->rass.SetPauseCheckExpression(std::string(express), waitTime);
}

int JRassWrap::getPauseCheckExpress(char *express, int &expressLength, int waitTime)
{
    return d->rass.GetPauseCheckExpression(express, expressLength, waitTime);
}

int JRassWrap::clearPauseCheckExpress(int waitTime)
{
    return d->rass.ClearPauseCheckExpression(waitTime);
}

int JRassWrap::setEnablePauseCheck(bool enabled, int waitTime)
{
    return d->rass.SetEnablePauseCheck(enabled, waitTime);
}

int JRassWrap::isEnablePauseCheck(bool &enabled, int waitTime)
{
    return d->rass.IsEnablePauseCheck(enabled, waitTime);
}

int JRassWrap::resetModelRunTimeStatInfo(int waitTime)
{
    return d->rass.ResetModelRunTimeStatInfo(waitTime);
}

int JRassWrap::setModelCanModifyItselfSymbolTable(bool enabled, int waitTime)
{
    return d->rass.SetModelCanModifyItselfSymbolTable(enabled, waitTime);
}

int JRassWrap::isVarOperable(const char *varName, bool &isOperable, int waitTime)
{
    return d->rass.IsVarOperable(std::string(varName), isOperable, waitTime);
}

int JRassWrap::getVarDes(const char *varName, ModelVarDesc &varDes, int waitTime)
{
    ModelVarDes_t t;
    int result = d->rass.GetVarDes(std::string(varName), t, waitTime);
    if (result == 0) {
        memcpy(&varDes, &t, sizeof(ModelVarDes_t));
        switch (varDes.varClass) {
        case 1:
            varDes.varClass = JRass::VC_Signal;
            break;
        case 2:
            varDes.varClass = JRass::VC_ModuleProperty;
            break;
        default:
            varDes.varClass = JRass::VC_Unknown;
            break;
        }
    }

    return result;
}

int JRassWrap::resetVarValue(const char *varName, int waitTime)
{
    return d->rass.ResetVarValue(std::string(varName), waitTime);
}

int JRassWrap::resetAllVarValues(int waitTime)
{
    return d->rass.ResetAllVarValues(waitTime);
}

int JRassWrap::modifyVar(const char *varName, bool value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, char value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, unsigned char value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, short value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, unsigned short value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, int value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, unsigned int value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, float value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVar(const char *varName, double value, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, char real, char imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, unsigned char real, unsigned char imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, short real, short imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, unsigned short real, unsigned short imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, int real, int imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, unsigned int real, unsigned int imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, float real, float imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarComplex(const char *varName, double real, double imag, int waitTime)
{
    return d->rass.ModifyVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVarFixPoint(const char *varName, double value, int waitTime)
{
    return d->rass.ModifyVarFixPoint(std::string(varName), value, waitTime);
}

int JRassWrap::modifyVarComplexFixPoint(const char *varName, double real, double imag, int waitTime)
{
    return d->rass.ModifyVarComplexFixPoint(std::string(varName), real, imag, waitTime);
}

int JRassWrap::modifyVar(const char *varName, void *pNewValue, unsigned int size, int waitTime)
{
    return d->rass.ModifyVar(std::string(varName), pNewValue, size, waitTime);
}

int JRassWrap::readVar(const char *varName, bool &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, char &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, unsigned char &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, short &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, unsigned short &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, int &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, unsigned int &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, float &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVar(const char *varName, double &value, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), value, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, char &real, char &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, unsigned char &real, unsigned char &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, short &real, short &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, unsigned short &real, unsigned short &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, int &real, int &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, unsigned int &real, unsigned int &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, float &real, float &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarComplex(const char *varName, double &real, double &imag, int waitTime)
{
    return d->rass.ReadVarComplex(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVarFixPoint(const char *varName, double &value, int waitTime)
{
    return d->rass.ReadVarFixPoint(std::string(varName), value, waitTime);
}

int JRassWrap::readVarComplexFixPoint(const char *varName, double &real, double &imag, int waitTime)
{
    return d->rass.ReadVarComplexFixPoint(std::string(varName), real, imag, waitTime);
}

int JRassWrap::readVar(const char *varName, void *pNewValue, unsigned int size, int waitTime)
{
    return d->rass.ReadVar(std::string(varName), pNewValue, size, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, bool value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, char value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, unsigned char value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, short value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, unsigned short value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, int value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, unsigned int value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, float value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);
}

int JRassWrap::setVarEntireItem(const char *varName, double value, int waitTime)
{
    return d->rass.SetVarEntireItem(std::string(varName), value, waitTime);

}

int JRassWrap::saveModelVarStateToServerFile(const char *fileName, int waitTime)
{
    return d->rass.SaveModelVarStateToServerFile(std::string(fileName), waitTime);
}

int JRassWrap::loadModelVarStateFromServerFile(const char *fileName, int waitTime)
{
    return d->rass.LoadModelVarStateFromServerFile(std::string(fileName), waitTime);
}

int JRassWrap::loadModelVarStateFromLocalFile(const char *fileName, int &successCount,
                                              int &failCount, int waitTime)
{
    return d->rass.LoadModelVarStateFromLocalFile(std::string(fileName), successCount, failCount, waitTime);
}

int JRassWrap::setLogMasterFileName(const char *fileName, int waitTime)
{
    return d->rass.SetLogMasterFileName(std::string(fileName), waitTime);
}

int JRassWrap::getLogMasterFileName(char *fileName, int size, int waitTime)
{
    return d->rass.GetLogMasterFileName(fileName, size, waitTime);
}

int JRassWrap::getLogFinalFileName(char *fileName, int size, int waitTime)
{
    return d->rass.GetLogFinalFileName(fileName, size, waitTime);
}

int JRassWrap::initLog(int waitTime)
{
    return d->rass.InitLog(waitTime);
}

int JRassWrap::closeLog(int waitTime)
{
    return d->rass.CloseLog(waitTime);
}

int JRassWrap::addLogVar(const char *varName, int waitTime)
{
    return d->rass.AddLogVar(std::string(varName), waitTime);
}

int JRassWrap::delLogVar(const char *varName, int waitTime)
{
    return d->rass.DelLogVar(std::string(varName), waitTime);
}

int JRassWrap::delAllLogVar(int waitTime)
{
    return d->rass.DelAllLogVar(waitTime);
}

int JRassWrap::startLog(int waitTime)
{
    return d->rass.StartLog(waitTime);
}

int JRassWrap::stopLog(int waitTime)
{
    return d->rass.StopLog(waitTime);
}

int JRassWrap::setLogIntervalTime(int interval, int waitTime)
{
    return d->rass.SetLogIntervalTime(interval, waitTime);
}

int JRassWrap::getLogIntervalTime(int &interval, int waitTime)
{
    return d->rass.GetLogIntervalTime(interval, waitTime);
}

int JRassWrap::addLogVarFromFile(const char *fileName, int waitTime)
{
    return d->rass.AddLogVarFromFile(std::string(fileName), waitTime);
}

int JRassWrap::setLogFileNameUnTimeAppend(bool flag, int waitTime)
{
    return d->rass.SetLogFileNameUnTimeAppend(flag, waitTime);
}

int JRassWrap::isLogFileNameTimeAppend(bool &flag, int waitTime)
{
    return d->rass.IsLogFileNameTimeAppend(flag, waitTime);
}

int JRassWrap::getLogVarList(char **&logVars, int &count, int waitTime)
{
    std::vector<std::string> *_logVars = nullptr;
    int result = d->rass.GetLogVarList(_logVars, waitTime);
    if (result == 0 && _logVars) {
        count = int(_logVars->size());
        logVars = new char*[count];
        for (int i = 0; i < count; ++i) {
            const std::string &_logVar = _logVars->at(i);
            const int size = int(_logVar.size());
            char *newLogVar = new char[size + 1];
            memcpy(newLogVar, _logVar.c_str(), size);
            newLogVar[size] = '\0';
            logVars[i] = newLogVar;
        }
        d->rass.GetAllVarsList_FreeRes(_logVars);
    }

    return result;
}

int JRassWrap::getLogVarListFreeRes(char **&logVars, int count)
{
    if (logVars) {
        for (int i = 0; i < count; ++i) {
            delete[] logVars[i];
        }
        delete[] logVars;
        logVars = nullptr;
    }

    return 0;
}

int JRassWrap::getLogInfo(LogInfo &logInfos, int waitTime)
{
    LogInfo_t t;
    int result = d->rass.GetLogInfo(t, waitTime);
    if (result == 0) {
        memcpy(&logInfos, &t, sizeof(LogInfo_t));
    }

    return result;
}

int JRassWrap::addMappingVar(const char *varName, unsigned int direction, unsigned int addr,
                             unsigned int mediumType, unsigned int interval, int waitTime)
{
    return d->rass.AddMappingVar(std::string(varName), direction, addr, mediumType, interval, waitTime);
}

int JRassWrap::delMappingVar(const char *varName, int waitTime)
{
    return d->rass.DelMappingVar(std::string(varName), waitTime);
}

int JRassWrap::delAllMappingVar(int waitTime)
{
    return d->rass.DelAllMappingVar(waitTime);
}

int JRassWrap::startMapping(int waitTime)
{
    return d->rass.StartMapping(waitTime);
}

int JRassWrap::stopMapping(int waitTime)
{
    return d->rass.StopMapping(waitTime);
}

int JRassWrap::isMappingStarted(bool &enabled, int waitTime)
{
    return d->rass.IsMappingStarted(enabled, waitTime);
}

int JRassWrap::clearShareMemoryByAddr(unsigned int shareMemType, unsigned int beginAddr,
                                      unsigned int endAddr, int waitTime)
{
    return d->rass.ClearShareMemoryByAddr(shareMemType, beginAddr, endAddr, waitTime);
}

int JRassWrap::clearShareMemoryOfMappingVars(unsigned int cleanFlag, int waitTime)
{
    return d->rass.ClearShareMemoryOfMappingVars(cleanFlag, waitTime);
}

int JRassWrap::addMappingVarFromFile(const char *fileName, int waitTime)
{
    return d->rass.AddMappingVarFromFile(std::string(fileName), waitTime);
}

int JRassWrap::getMappingVarList(MappingVarDesc *&vars, int &count, int waitTime)
{
    std::vector<MappingVarDes_t> *_vars = nullptr;
    int result = d->rass.GetMappingVarList(_vars, waitTime);
    if (result == 0 && _vars) {
        count = int(_vars->size());
        vars = new MappingVarDesc[count];
        for (int i = 0; i < count; ++i) {
            const MappingVarDes_t &_var = _vars->at(i);
            MappingVarDesc &var_ = vars[i];
            memcpy(var_.name, _var.name.c_str(),
                   std::min<int>(MAPPINGVAR_NAME_LENGTH - 1, _var.name.size()));
            var_.name[MAPPINGVAR_NAME_LENGTH - 1] = '\0';
            var_.direction = _var.direction;
            var_.shareMemType = _var.shareMemType;
            var_.mapAddress = _var.mapAddress;
            var_.length = _var.length;
            var_.intervalCount = _var.intervalCount;
        }
        d->rass.GetMappingVarList_FreeRes(_vars);
    }

    return result;
}

int JRassWrap::getMappingVarListFreeRes(MappingVarDesc *&vars)
{
    if (vars) {
        delete[] vars;
        vars = nullptr;
    }

    return 0;
}

int JRassWrap::addGMonVar(const char *varName, int waitTime)
{
    return d->rass.AddGMonVar(std::string(varName), waitTime);
}

int JRassWrap::delGMonVar(const char *varName, int waitTime)
{
    return d->rass.DelGMonVar(std::string(varName), waitTime);
}

int JRassWrap::delAllGMonVar(int waitTime)
{
    return d->rass.DelAllGMonVar(waitTime);
}

int JRassWrap::enableGMon(int waitTime)
{
    return d->rass.EnableGMon(waitTime);
}

int JRassWrap::disableGMon(int waitTime)
{
    return d->rass.DisableGMon(waitTime);
}

int JRassWrap::setGMonIntervalTime(unsigned int interval, int waitTime)
{
    return d->rass.SetGMonIntervalTime(interval, waitTime);
}

int JRassWrap::isGMonEnable(bool &enable, int waitTime)
{
    return d->rass.IsGMonEnable(enable, waitTime);
}

int JRassWrap::getGMonIntervalTime(unsigned int &interval, int waitTime)
{
    return d->rass.GetGMonIntervalTime(interval, waitTime);
}

void JRassWrap::setMCastIp(const char *localIp, const char *mcastIp, unsigned short port)
{
    return CRass::SetMCastIp(std::string(localIp), std::string(mcastIp), port);
}

int JRassWrap::startGMonAllServer()
{
    return CRass::StartGMonAllServer();
}

int JRassWrap::stopGMonAllServer()
{
    return CRass::StopGMonAllServer();
}

int JRassWrap::openCMon(const char *groupName, int waitTime)
{
    return d->rass.OpenCMon(std::string(groupName), waitTime);
}

int JRassWrap::closeCMon(const char *groupName, int waitTime)
{
    return d->rass.CloseCMon(std::string(groupName), waitTime);
}

int JRassWrap::addCMonVar(const char *groupName, const char *varName, int waitTime)
{
    return d->rass.AddCMonVar(std::string(groupName), std::string(varName), waitTime);
}

int JRassWrap::delCMonVar(const char *groupName, const char *varName, int waitTime)
{
    return d->rass.DelCMonVar(std::string(groupName), std::string(varName), waitTime);
}

int JRassWrap::delAllCMonVar(const char *groupName, int waitTime)
{
    return d->rass.DelAllCMonVar(std::string(groupName), waitTime);
}

int JRassWrap::enableCMon(const char *groupName, int waitTime)
{
    return d->rass.EnableCMon(std::string(groupName), waitTime);
}

int JRassWrap::disableCMon(const char *groupName, int waitTime)
{
    return d->rass.DisableCMon(std::string(groupName), waitTime);
}

int JRassWrap::setCMonIntervalTime(const char *groupName, unsigned int interval, int waitTime)
{
    return d->rass.SetCMonIntervalTime(std::string(groupName), interval, waitTime);
}

int JRassWrap::isCMonEnable(const char *groupName, bool &enable, int waitTime)
{
    return d->rass.IsCMonEnable(std::string(groupName), enable, waitTime);
}

int JRassWrap::getCMonIntervalTime(const char *groupName, unsigned int &interval, int waitTime)
{
    return d->rass.GetCMonIntervalTime(std::string(groupName), interval, waitTime);
}

int JRassWrap::closeAllCMon(int waitTime)
{
    return d->rass.CloseAllCMon(waitTime);
}

int JRassWrap::getAllCMonDes(MonitorGroupDescNative *&groupDescs, int &count, int waitTime)
{
    std::vector<MonitorGroupDes_t> *_groupDescs = nullptr;
    int result = d->rass.GetAllCMonDes(_groupDescs, waitTime);
    if (result == 0 && _groupDescs) {
        count = int(_groupDescs->size());
        groupDescs = new MonitorGroupDescNative[count];
        for (int i = 0; i < count; ++i) {
            const MonitorGroupDes_t &_groupDesc = _groupDescs->at(i);
            MonitorGroupDescNative &groupDesc = groupDescs[i];
            if (!_groupDesc.name.empty()) {
                const int size = _groupDesc.name.size();
                groupDesc.name = new char[size + 1];
                memcpy(groupDesc.name, _groupDesc.name.c_str(), size);
                groupDesc.name[size] = '\0';
            }
            if (_groupDesc.varsList.empty()) {
                const int size = _groupDesc.varsList.size();
                groupDesc.varCount = size;
                groupDesc.vars = new char*[size];
                for (int j = 0; j < size; ++j) {
                    const std::string &_var = _groupDesc.varsList[j];
                    const int varSize = int(_var.size());
                    char *newVar = new char[varSize + 1];
                    memcpy(newVar, _var.c_str(), varSize);
                    newVar[size] = '\0';
                    groupDesc.vars[j] = newVar;
                }
            }
        }
        d->rass.GetAllCMonDes_FreeRes(_groupDescs);
    }

    return result;
}

int JRassWrap::getAllCMonDesFreeRes(MonitorGroupDescNative *&groupDescs)
{
    if (groupDescs) {
        delete[] groupDescs;
        groupDescs = nullptr;
    }

    return 0;
}

int JRassWrap::forceCMonRefreshOneFrameData(int waitTime)
{
    return d->rass.ForceCMonRefreshOneFrameData(waitTime);
}

void JRassWrap::setFindNewServerCallback(FuncFindNewServer callback)
{
    (void)callback;
    //TODO
}

void JRassWrap::setAllServerMonitorReadDataCallback(FuncAllServerMonitorGotNewData callback)
{
    (void)callback;
    //TODO
}

void JRassWrap::setMonitorReadDataCallback(FuncMonitorGotNewData callback)
{
    (void)callback;
    //TODO
}

int JRassWrap::getMonData(const char *varName, bool &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, char &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, unsigned char &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, short &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, unsigned short &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, int &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, unsigned int &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, float &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, double &data)
{
    return CRass::GetMonData(std::string(varName), data);
}

int JRassWrap::getMonData(const char *varName, void *pData, unsigned int length, unsigned int type)
{
    return CRass::GetMonData(std::string(varName), pData, length, type);
}

int JRassWrap::rebootMachine()
{
    return d->rass.RebootMachine();
}

int JRassWrap::saveLogMapMonitorStatus(int waitTime)
{
    return d->rass.SaveLogMapMonitorStatus(waitTime);
}

int JRassWrap::isServerValid(bool &valid)
{
    return d->rass.IsServerValid(valid);
}

bool JRassWrap::isConnected()
{
    return d->rass.IsConnect();
}

long long JRassWrap::getId()
{
    return d->rass.GetId();
}

bool JRassWrap::isVersionSameOfServer(int waitTime)
{
    return d->rass.IsVersionSameOfServer(waitTime);
}

int JRassWrap::setFloatPrecision(double prec, int waitTime)
{
    return d->rass.SetFloatPrecision(prec, waitTime);
}

int JRassWrap::getFloatPrecision(double &prec, int waitTime)
{
    return d->rass.GetFloatPrecision(prec, waitTime);
}

int JRassWrap::ftpGet(const char *fileName, const char *toPath, int waitTime)
{
    return d->rass.Ftp_Get(std::string(fileName), std::string(toPath), waitTime);
}

int JRassWrap::ftpPut(const char *fileName, int waitTime)
{
    return d->rass.Ftp_Put(std::string(fileName), waitTime);
}

int JRassWrap::ftpExists(const char *fileName, bool &exists, int waitTime)
{
    return d->rass.Ftp_IsExist(std::string(fileName), exists, waitTime);
}

int JRassWrap::ftpIsDir(const char *fileName, bool &isDir, int waitTime)
{
    return d->rass.Ftp_IsDir(std::string(fileName), isDir, waitTime);
}

int JRassWrap::ftpCD(const char *path, bool isRelativeDir, int waitTime)
{
    return d->rass.Ftp_CD(std::string(path), isRelativeDir, waitTime);
}

int JRassWrap::ftpMkdir(const char *path, int waitTime)
{
    return d->rass.Ftp_MDIR(std::string(path), waitTime);
}

int JRassWrap::ftpPwd(char *path, unsigned int size, int waitTime)
{
    return d->rass.Ftp_PWD(path, size, waitTime);
}

int JRassWrap::ftpList(FileDesc *&fileDescs, int &count, int waitTime)
{
    std::vector<FileDes_t> *_fileDescs = nullptr;
    int result = d->rass.Ftp_GetList(_fileDescs, waitTime);
    if (result == 0 && _fileDescs) {
        count = int(_fileDescs->size());
        fileDescs = new FileDesc[count];
        for (int i = 0; i < count; ++i) {
            memcpy(&fileDescs[i], &_fileDescs[i], sizeof(FileDes_t));
        }
        d->rass.Ftp_GetList_FreeRes(_fileDescs);
    }

    return result;
}

int JRassWrap::ftpListFreeRes(FileDesc *&fileDescs)
{
    if (fileDescs) {
        delete[] fileDescs;
        fileDescs = nullptr;
    }

    return 0;
}

int JRassWrap::ftpDel(const char *fileName, int waitTime)
{
    return d->rass.Ftp_Del(std::string(fileName), waitTime);
}

int JRassWrap::ftpRd(const char *fileName, int waitTime)
{
    return d->rass.Ftp_RD(std::string(fileName), waitTime);
}

int JRassWrap::insertExpress(const char *express, int &expressId, int waitTime)
{
    return d->rass.InsExp(std::string(express), expressId, waitTime);
}

int JRassWrap::deleteExpress(const char *express, int waitTime)
{
    return d->rass.DelExp(std::string(express), waitTime);
}

int JRassWrap::deleteExpress(int expressId, int waitTime)
{
    return d->rass.DelExp(expressId, waitTime);
}

int JRassWrap::deleteAllExpress(int waitTime)
{
    return d->rass.DelAllExp(waitTime);
}

int JRassWrap::getAllExpress(char **&expresses, int &expressCount,
                             int *&expressIds, int &expressIdCount, int waitTime)
{
    std::vector<std::string> *_expresses = nullptr;
    std::vector<int> *_expressIds = nullptr;
    int result = d->rass.GetAllExp(_expresses, _expressIds, waitTime);
    if (result == 0) {
        if (_expresses) {
            expressCount = int(_expresses->size());
            expresses = new char*[expressCount];
            for (int i = 0; i < expressCount; ++i) {
                const std::string &_express = _expresses->at(i);
                const int size = _express.size();
                char *newExpress = new char[size + 1];
                memcpy(newExpress, _express.c_str(), size);
                newExpress[size] = '\0';
                expresses[i] = newExpress;
            }
        }
        if (_expressIds) {
            expressIdCount = int(_expressIds->size());
            expressIds = new int[expressIdCount];
            for (int i = 0; i < expressIdCount; ++i) {
                expressIds[i] = _expressIds->at(i);
            }
        }
        d->rass.GetAllExp_FreeRes(_expresses, _expressIds);
    }

    return result;
}

int JRassWrap::getAllExpressFreeRes(char **&expresses, int expressCount, int *&expressIds)
{
    if (expresses) {
        for (int i = 0; i < expressCount; ++i) {
            delete[] expresses[i];
        }
        delete[] expresses;
        expresses = nullptr;
    }

    if (expressIds) {
        delete[] expressIds;
        expressIds = nullptr;
    }

    return 0;
}
