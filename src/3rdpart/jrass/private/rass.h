//Rass���ƿ�ͷ�ļ�
#ifndef RASS_H
#define RASS_H

#include <string>
#include <vector>

#define NONE_ENOUGH_MEMORY -200     //����ڷ����ַ�����ʱ�򣬻���ռ䲻�������ش�ֵ

//ģ������״̬
enum SERVER_RUN_STATUS {
    SRS_UNKONW = 0,
    SRS_RUN,
    SRS_READY,
    SRS_STEP,
    SRS_STOP
};

//��������
#pragma pack(1)
struct ModelVarDes_t {
    unsigned char   varClass;       //�������ͣ��Ƿ����ź�
    unsigned char   dataType;       //ģ�ͱ�������(0:double, 1:float, 2:char, 3:uchar, 4:short, 5:ushort, 6:int, 7:uint, 8:bool, 200:fixpoint, 255:struct)
    unsigned short  dataSize;       //ģ�ͱ���ÿ���С,�ܴ�СΪά���ĳ˻����������С�ĳ˻�
    unsigned int    nDims;          //��ά
    unsigned int    dim[10];        //ά��
    bool            isComplex;      //�Ƿ��Ǹ���
    bool            isPointer;      //�Ƿ���Ҫָ�����
    bool            isFixPointData; //�Ƿ��Ƕ�����

    bool IsSimpleVar(void) {
        if (isComplex || isPointer || isFixPointData)
        {
            return false;
        }

        if (dataType == 255)
        {
            return false;
        }

        unsigned int size = 1;
        for (unsigned int i=0; i<nDims; ++i)
        {
            size *= dim[i];
        }
        if (1 != size)
        {
            return false;
        }

        return true;
    }
};

//�ļ������ṹ
struct FileDes_t
{
    long long int   fileSize;       //�ļ���С
    char            filename[512];  //�ļ���
    bool            isDirectory;    //�Ƿ�·��
    unsigned int    year;           //��
    unsigned char   month;          //��
    unsigned char   day;            //��
    unsigned char   hour;           //ʱ
    unsigned char   minute;         //��
    unsigned char   second;         //��
};

//ӳ������ṹ����
struct MappingVarDes_t{
    std::string         name;               //ӳ���ģ�Ϳɲ�����������
    unsigned int        direction;          //ӳ��ķ���1:ģ�͵��ڴ� 2:�ڴ浽ģ��
    unsigned int        shareMemType;       //�����ڴ����ͣ�1��ʾ�����ڴ棬2��ʾUDP�����ڴ棬3��ʾ���ع����ڴ�
    unsigned int        mapAddress;         //�����ڴ��ַ
    unsigned int        length;             //ӳ��ĳ���
    unsigned int        intervalCount;      //����ӳ�������������ڴ������������ϲŽ���ӳ��������ṩ�û��������ź�Ҫ�󣬵��ǹ��ߵ�ӳ���ٶ�ռ�ü����������ģ�����г�ʱ

    MappingVarDes_t()
    {

    }

    MappingVarDes_t(const MappingVarDes_t & rsh)
    {
        if (this == &rsh)
        {
            return;
        }

        this->name         = rsh.name;
        this->direction    = rsh.direction;
        this->shareMemType = rsh.shareMemType;
        this->mapAddress   = rsh.mapAddress;
        this->length       = rsh.length;
        this->intervalCount= rsh.intervalCount;
    }

    const MappingVarDes_t & operator= (const MappingVarDes_t & rsh) {
        if (this == &rsh)
        {
            return *this;
        }

        this->name         = rsh.name;
        this->direction    = rsh.direction;
        this->shareMemType = rsh.shareMemType;
        this->mapAddress   = rsh.mapAddress;
        this->length       = rsh.length;
        this->intervalCount= rsh.intervalCount;

        return *this;
    }
};

//ӳ�������Ϣ����
struct MonitorGroupDes_t {
    std::string                 name;
    std::vector<std::string>    varsList;

    MonitorGroupDes_t()
    {

    }

    MonitorGroupDes_t(const MonitorGroupDes_t & rsh)
    {
        if (this == &rsh)
        {
            return;
        }

        this->name         = rsh.name;
        this->varsList     = rsh.varsList;
    }

    const MonitorGroupDes_t & operator= (const MonitorGroupDes_t & rsh) {
        if (this == &rsh)
        {
            return *this;
        }

        this->name         = rsh.name;
        this->varsList     = rsh.varsList;

        return *this;
    }
};

//��¼��Ϣ�ṹ
struct LogInfo_t {
    bool                bLogStarted;                    //��¼�Ƿ��Ѿ���ʼ����ʼ��ʾ��¼�ļ��Ѿ���
    bool                bLogEnable;                     //��¼�Ƿ�ʹ�ܣ���ʾ��¼ʹ�ܡ�
    char                logMasterFileName[1024];        //��¼���ļ���
    bool                bDoNotAutoGenTimeFileName;      //��¼�ļ����Ƿ�ʹ��ʱ�䣬������ļ������������ļ���
    char                logFileName[1024];              //��¼�����ļ���
    int                 logIntervalTime;                //�����¼���ڣ�Ϊ��Ƶ�ʵ�������
    unsigned int        logVarsStatusCnt;               //��¼���еı�����������������ݱȱ��ظ��£���ô��Ҫ���м�¼�����б�ͬ��
};
#pragma pack()

typedef int (__stdcall * MonitorGotNewData_f)(const std::string & monListName, double time);
typedef int (__stdcall * AllServerMonitorGotNewData_f)(long long Id, const std::string & monListName, double time);
typedef int (__stdcall * FindNewServer_f)(const std::string & name, const std::string & addr, unsigned short port);

//////////////////////////////////////////////////////////////////////////
//ģ�Ϳ�����
//�ȴ���ʱΪ
//  -1   :   ʱ��ʾ���޵ȴ���
//  0    :   ��ʱ���ʾ�������أ����������ִ�н��
//  >0   :   ��ʱ���ʾ�ĵȴ�ʱ�䣬��λms��
//  ���� :   Ϊ�Ƿ�ֵ
//���ء�ж��ģ�ͺ�ʱ��Ƚϳ���������Ҫ�ȴ�ʱ��ϳ���ͨ��������Ϊ���޵ȴ������ߴ���100ms
//ģ�����п���ʱ����Ϊ����5ms���ɣ��������޵ȴ�
//////////////////////////////////////////////////////////////////////////

class __declspec(dllexport) CRass
{
public:
    //////////////////////////////////////////////////////////////////////////
    //Ŀ�������״̬�趨                                                    //
    //��������IP�����������ļ���                                            //
    //////////////////////////////////////////////////////////////////////////

    //����Ŀ���
    const int Connect(const std::string & targetIP = "", int port = 0, std::string localif = "");

    //�ر�Ŀ���
    const int Close(void);

    //////////////////////////////////////////////////////////////////////////
    //ģ�Ϳ���                                                              //
    //����Ŀ���ģ�͵ļ��غ�����                                            //
    //////////////////////////////////////////////////////////////////////////
    //ģ�ͼ���
    const int Load(
        const std::string & modelPath,                          //ģ��ȫ·��
        const std::string & modelName,                          //ģ������
        int                 waitTime,                           //�ȴ���ʱʱ��
        bool                bClearVarLogTable     = true,       //�Ƿ����������¼��
        bool                bClearVarMappingTable = true,       //�Ƿ��������ӳ���
        bool                bClearVarMonitorTable = true,       //�Ƿ����������ر�
        bool                bForeceClearLogMappingMonitorVars = false,  //�Ƿ�ǿ��ִ������������������ָ��
        double              freqScale = 1,                      //ģ��Ƶ�ʵĵ�����
        bool                freeRun = false                     //�Ƿ���������
        );

    //��ȡģ��·��������
    const int GetLoadedInfo(char * path, unsigned int pathLen, char * name, unsigned int nameLen, int waitTime = -1);

    //ģ��ж��
    const int Unload(bool reserveLogMappingMonitor, int waitTime = -1) ;

    //���¼���ģ�ͷ��ű�
    const int ReloadSymbolTalbe(int waitTime = -1);

    //��ȡģ�ͻ���Ƶ��ʱ��
    const int GetModelBaseFreq(double & freqTime, int waitTime = -1);

    //����ģ������ģʽ
    const int SetModelRunMode(double freqScale = 1, bool freeRun = false, int waitTime = -1);

    //��ȡģ������ģʽ��Ϣ
    const int GetModelRunMode(double & freqScale, bool & freeRun, int waitTime = -1);

    //ͬ��������ȫ������
    const int SyncServer(int waitTime = -1);

    //��ȡ���б������б�
    const int GetAllVarsList(std::vector<std::string> * & varsList);

    //�ͷŶ�ȡ�������б�
    void GetAllVarsList_FreeRes(std::vector<std::string> * & pStr);

    //ģ�ͳ�ʼ��
    const int Init(int waitTime = -1) ;

    //ģ������
    const int Run(int waitTime = -1) ;

    //��ͣģ������
    const int Pause(int waitTime = -1) ;

    //����ģ�ͣ������ǵ����Ͷಽ
    const int Step(const unsigned int nStep, int waitTime = -1) ;

    //����ģ�ͣ������ǵ����Ͷಽ
    const int StepSync(const unsigned int nStep, int waitTime = -1) ;

    //ֹͣģ��
    const int Stop(int waitTime = -1) ;

    //��ѯģ������״̬
    const int GetModelRunStatus(bool & haveLoadModel, SERVER_RUN_STATUS & modelStatus, int waitTime = -1) ;

    //��ȡģ�Ͳ�������״̬���Ƿ��ڲ���֮��
    const int GetStepRuningStatus(bool & isRunningStep, int waitTime = -1) ;

    //����ģ����������ʱ��
    const int SetModelRunTimeLimit(double timeLimit, int waitTime = -1) ;

    //��ȡģ����������ʱ��
    const int GetModelRunTimeLimit(double & timeLimit, int waitTime = -1);

    //��ȡģ�͵�ǰ����ʱ��
    const int GetCurrentModelRunTime(double & time, int waitTime = -1) ;

    //����ģ����ͣ�����ʽ
    const int SetPauseCheckExpression(const std::string & expressionStr, int waitTime = -1);

    //��ȡģ����ͣ�����ʽ
    const int GetPauseCheckExpression(char * expressionStr, unsigned int expStrLen, int waitTime = 1);

    //���ģ����ͣ�����ʽ
    const int ClearPauseCheckExpression(int waitTime = -1);

    //����ʹ��ģ����ͣ�����ʽ
    const int SetEnablePauseCheck(bool enalbeFlag, int waitTime = -1);

    //��ѯʹ��ģ����ͣ�����ʽ
    const int IsEnablePauseCheck(bool & enalbeFlag, int waitTime = -1);

    //��λģ������ʱ��ͳ����Ϣ
    const int ResetModelRunTimeStatInfo(int waitTime = -1);

    //����ģ�Ϳ����޸�������ķ��ű�
    const int SetModelCanModifyItselfSymbolTable(bool enableFlag, int waitTime = -1);

    //////////////////////////////////////////////////////////////////////////
    //ģ�ͱ����޸Ĳ�ѯ                                                      //
    //��������޸ĺͲ�ѯģ���еı���                                        //
    //////////////////////////////////////////////////////////////////////////

    //��ѯģ�ͱ�������Ϣ
    const int IsVarOperable(const std::string & varStr, bool & isOperable, int waitTime = -1) ;

    //��ѯģ�ͱ�������Ϣ
    const int GetVarDes(const std::string & varStr, ModelVarDes_t & varDes, int waitTime = -1);

    //�ָ��޸ı�����ԭʼֵ
    const int ResetVarValue(const std::string & varStr, int waitTime = -1) ;

    //�ָ����б�����ԭʼֵ
    const int ResetAllVarValues(int waitTime = -1) ;

    //�޸ĵ���ģ�͵ı�����ʵ��
    const int ModifyVar(const std::string & varStr, char            i8  , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, unsigned char   ui8 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, short           i16 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, unsigned short  ui16, int waitTime = -1);
    const int ModifyVar(const std::string & varStr, int             i32 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, unsigned int    ui32, int waitTime = -1);
    const int ModifyVar(const std::string & varStr, float           f32 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, double          f64 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, bool            b8  , int waitTime = -1);

    //�޸ĵ���ģ�͵ı���������
    const int ModifyVarComplex(const std::string & varStr, char            real_i8  , char            imn_i8  , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, unsigned char   real_ui8 , unsigned char   imn_ui8 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, short           real_i16 , short           imn_i16 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, unsigned short  real_ui16, unsigned short  imn_ui16, int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, int             real_i32 , int             imn_i32 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, unsigned int    real_ui32, unsigned int    imn_ui32, int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, float           real_f32 , float           imn_f32 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, double          real_f64 , double          imn_f64 , int waitTime = -1);
                                                             
    //�޸ĵ����Ķ�����
    const int ModifyVarFixPoint(const std::string & varStr, double f64 , int waitTime = -1);
    const int ModifyVarComplexFixPoint(const std::string & varStr, double real_f64 , double imn_f64 , int waitTime = -1);

    //���ڴ����ģ�ͱ����������ﲻ�ܽ����ϸ�����ͼ�飬���Խ���ʹ��
    const int ModifyVar(const std::string & varStr, void * pNewVale, unsigned int size, int waitTime = -1);

    //��ȡ����ģ�͵ı�����ʵ��
    const int ReadVar(const std::string & varStr, char            & i8  , int waitTime = -1);
    const int ReadVar(const std::string & varStr, unsigned char   & ui8 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, short           & i16 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, unsigned short  & ui16, int waitTime = -1);
    const int ReadVar(const std::string & varStr, int             & i32 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, unsigned int    & ui32, int waitTime = -1);
    const int ReadVar(const std::string & varStr, float           & f32 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, double          & f64 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, bool            & b8  , int waitTime = -1);

    //��ȡ����ģ�͵ı���������
    const int ReadVarComplex(const std::string & varStr, char            & real_i8  , char            & imn_i8  , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, unsigned char   & real_ui8 , unsigned char   & imn_ui8 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, short           & real_i16 , short           & imn_i16 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, unsigned short  & real_ui16, unsigned short  & imn_ui16, int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, int             & real_i32 , int             & imn_i32 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, unsigned int    & real_ui32, unsigned int    & imn_ui32, int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, float           & real_f32 , float           & imn_f32 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, double          & real_f64 , double          & imn_f64 , int waitTime = -1);
                                      
    //��ȡ�����Ķ�����
    const int ReadVarFixPoint(const std::string & varStr, double & f64 , int waitTime = -1);
    const int ReadVarComplexFixPoint(const std::string & varStr, double & real_f64 , double & imn_f64 , int waitTime = -1);

    //���ڴ����ģ�ͱ����������ﲻ�ܽ����ϸ�����ͼ�飬���Խ���ʹ��
    const int ReadVar(const std::string & varStr, void * pNewVale, unsigned int size, int waitTime = -1);

    //����ĳ������������ά�������е�ʵ�����鲿����Ϊĳһ������
    const int SetVarEntireItem(const std::string & varStr, char           i8  , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, unsigned char  ui8 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, short          i16 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, unsigned short ui16, int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, int            i32 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, unsigned int   ui32, int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, float          f32 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, double         f64 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, bool           b8  , int waitTime = -1);

    //�������״̬���������ļ�
    const int SaveModelVarStateToServerFile(const std::string & fileName, int waitTime = -1);

    //�ӷ������ļ��������״̬
    const int LoadModelVarStateFromServerFile(const std::string & fileName, int waitTime = -1);

    //�ӱ����ļ��������״̬
    //�˺���������ͬ��������֮�����ʹ�ã�SyncServer��
    const int LoadModelVarStateFromLocalFile(const std::string & fileName, int & successItemCnt, int & failItemCnt, int itemModifyWaitTime = -1);

    //////////////////////////////////////////////////////////////////////////
    //ģ�ͱ�����¼                                                          //
    //������μ�¼ģ���еı���                                              //
    //һ��ģ��ͬһ��ʱ��ֻ�����¼һ���ļ�                                  //
    //////////////////////////////////////////////////////////////////////////

    //����ģ�ͼ�¼���ļ���
    const int SetLogMasterFileName(const std::string & mainFileName, int waitTime = -1) ;

    //��ȡģ�ͼ�¼���ļ���
    const int GetLogMasterFileName(char * mainFileName, unsigned int fileNameLen, int waitTime = -1) ;

    //��ȡ���յ�ģ�ͼ�¼��
    const int GetLogFinalFileName(char * finalFileName, unsigned int fileNameLen, int waitTime = -1) ;

    //ģ�ͼ�¼��ʼ��,���յ����������ļ���+ʱ���γ�
    const int InitLog(int waitTime = -1) ;

    //ģ�ͼ�¼�ر�
    const int CloseLog(int waitTime = -1) ;

    //ģ�ͼ�¼�������
    const int AddLogVar(const std::string & varStr, int waitTime = -1) ;

    //ģ�ͼ�¼����ɾ��
    const int DelLogVar(const std::string & varStr, int waitTime = -1) ;

    //ģ�ͼ�¼����ȫɾ��
    const int DelAllLogVar(int waitTime = -1) ;

    //ģ�ͼ�¼��ʼ
    const int StartLog(int waitTime = -1) ;

    //ģ�ͼ�¼ֹͣ
    const int StopLog(int waitTime = -1) ;

    //ģ�ͼ�¼����ʱ���趨,���ʱ��Ϊģ�ͻ�Ƶ�ı���ֵ
    const int SetLogIntervalTime(int intervalTime, int waitTime = -1) ;

    //��ȡģ�ͼ�¼���ʱ��
    const int GetLogIntervalTime(int & intervalTime, int waitTime = -1) ;

    //���ļ���Ӽ�¼�б�
    const int AddLogVarFromFile(const std::string & fileName, int waitTime = -1);

    //���ò�ʹ��ʱ��������ģ�ͼ�¼�ļ�����Ҳ��ֻ�������ļ�����Ϊ�ļ���
    const int SetLogFileNameUnTimeAppend(bool flag, int waitTime);

    //��ѯ��ʹ��ʱ��������ģ�ͼ�¼�ļ�����Ҳ��ֻ�������ļ�����Ϊ�ļ���
    const int IsLogFileNameTimeAppend(bool & flag, int waitTime);

    //��ȡ���еļ�¼�����б�
    const int GetLogVarList(std::vector<std::string> * & logVarsList, int waitTime = -1);

    //�ͷŻ�ȡ���еļ�¼�����б�
    void GetLogVarList_FreeRes(std::vector<std::string> * & logVarsList);

    //��ȡ��¼����Ϣ
    const int GetLogInfo(LogInfo_t & logInfo, int waitTime = -1);

    //////////////////////////////////////////////////////////////////////////
    //ģ��ӳ���¼                                                          //
    //�������ӳ��ģ���еı���                                              //
    //////////////////////////////////////////////////////////////////////////
    //ģ��ӳ��������
    const int AddMappingVar(
        const std::string & varStr, //ģ�ͱ������ַ�������
        unsigned int direction,     //ģ�ͱ���ӳ��ķ���,1ģ�͵������ڴ�,2�����ڴ浽ģ��
        unsigned int addr,          //ӳ��ĵ�ַ
        unsigned int mediumType,    //ӳ������ͣ�1��ʾ�����ڴ棬2��ʾUDP�����ڴ棬3��ʾ���ع����ڴ�
        unsigned int intervalTime,  //ӳ����ʱ�䣬����ÿ��������ӳ��Ƶ�ʶ���ͬ
        int waitTime = -1) ;

    //ģ��ӳ�����ɾ��
    const int DelMappingVar(const std::string & varStr, int waitTime = -1) ;

    //ģ��ӳ�����ȫ��ɾ��
    const int DelAllMappingVar(int waitTime = -1) ;

    //ģ��ӳ�俪ʼ
    const int StartMapping(int waitTime = -1) ;

    //ģ��ӳ��ֹͣ
    const int StopMapping(int waitTime = -1) ;

    //ģ���Ƿ�ʼ
    const int IsMappingStarted(bool & isEnable, int waitTime = -1) ;

    //���ָ��������ڴ棨�����䣩
    const int ClearShareMemoryByAddr(unsigned int shareMemType, unsigned int beginAddress, unsigned int endAddress, int waitTime = -1);

    //���ӳ��������ڴ棨�����䣩cleanFlag: SHARE_MEM_CLEAN_IN(1),SHARE_MEM_CLEAN_OUT(2) ���Ի�����
    const int ClearShareMemoryOfMappingVars(unsigned int cleanFlag, int waitTime = -1);

    //���ļ�����ӳ����Ϣ
    const int AddMappingVarFromFile(const std::string & fileName, int waitTime = -1);

    //��ȡ���е�ӳ���б���Ϣ
    const int GetMappingVarList(std::vector<MappingVarDes_t> * & varList, int waitTime = -1);

    //�ͷŶ�ȡ���յ�ģ�ͼ�¼��
    void GetMappingVarList_FreeRes(std::vector<MappingVarDes_t> * & varList);

    //////////////////////////////////////////////////////////////////////////
    //ģ�ͼ��                                                              //
    //�������ӳ��ģ���еı���                                              //
    //////////////////////////////////////////////////////////////////////////
    //ģ�ͼ�ر������
    const int AddGMonVar(const std::string & varStr, int waitTime = -1);

    //ģ�ͼ�ر���ɾ��
    const int DelGMonVar(const std::string & varStr, int waitTime = -1);

    //ģ�ͼ�ر���ȫ��ɾ��
    const int DelAllGMonVar(int waitTime = -1);

    //ģ��ȫ�ּ������
    const int EnableGMon(int waitTime = -1);

    //ģ��ȫ�ּ�ؽ�ֹ
    const int DisableGMon(int waitTime = -1);

    //ģ�ͼ������ʱ���趨,���ʱ��
    const int SetGMonIntervalTime(unsigned int intervalTime, int waitTime = -1);

    //��ѯģ��ȫ�ּ���Ƿ�����
    const int IsGMonEnable(bool & enable, int waitTime = -1);

    //��ѯģ��ȫ�ּ�ؼ��ʱ��
    const int GetGMonIntervalTime(unsigned int & intervalTime, int waitTime = -1);

    //����ȫ�ּ�ص��鲥��Ϣ��ip���˿�
    static void SetMCastIp(const std::string & localIp, const std::string & mcastIp = "255.5.5.5", unsigned short port = 22000);

    //��ʼȫ�ּ��
    static int StartGMonAllServer(void);

    //�ر�ȫ�ּ�ض���
    static int StopGMonAllServer(void);

    //////////////////////////////////////////////////////////////////////////
    //ר���ڼ���Ӷ��еĺ���

    //����һ������Ӷ��У�������Ѿ����ڣ���ô������
    const int OpenCMon(const std::string & monListName, int waitTime = -1);

    //�ر�һ����ض��У�������еı����ü���Ϊ0����ôɾ������
    const int CloseCMon(const std::string & monListName, int waitTime = -1);

    //ģ�ͼ�ر������
    const int AddCMonVar(const std::string & monListName,const std::string & varStr, int waitTime = -1);

    //ģ�ͼ�ر���ɾ��
    const int DelCMonVar(const std::string & monListName,const std::string & varStr, int waitTime = -1);

    //ģ�ͱ���ȫ��ɾ��
    const int DelAllCMonVar(const std::string & monListName, int waitTime = -1);

    //ģ�ͼ�ؿ�ʼ
    const int EnableCMon(const std::string & monListName, int waitTime = -1);

    //ģ�ͼ��ֹͣ
    const int DisableCMon(const std::string & monListName, int waitTime = -1);

    //ģ�ͼ������ʱ���趨
    const int SetCMonIntervalTime(const std::string & monListName, unsigned int intervalTime, int waitTime = -1);

    //��ѯģ���Ӽ���Ƿ�����
    const int IsCMonEnable(const std::string & monListName, bool & enable, int waitTime = -1);

    //��ѯģ���Ӽ�ؼ��ʱ��
    const int GetCMonIntervalTime(const std::string & monListName, unsigned int & intervalTime, int waitTime = -1);

    //ɾ������
    const int CloseAllCMon(int waitTime = -1);

    //��ȡ���м���б�����ƺͱ������б�
    const int GetAllCMonDes(std::vector<MonitorGroupDes_t> * & monGroupList, int waitTime = -1);

    //�ͷŻ�ȡ���м���б�����ƺͱ������б�
    void GetAllCMonDes_FreeRes(std::vector<MonitorGroupDes_t> * & monGroupList);

    //ǿ��ˢ�¼������
    const int ForceCMonRefreshOneFrameData(int waitTime = -1);

    //���÷����µķ������ص�����
    static void SetFindNewServerCallFunc(FindNewServer_f pfunc);

    //����ȫ�ּ�����ݻص�����
    static void SetAllServerMonitorReadDataFunc(AllServerMonitorGotNewData_f pfunc);

    //���ü�ض����ݺ���
    void SetMonitorReadDataFunc(MonitorGotNewData_f pfunc);

    //��ȡ������ݵĺ���
    static const int GetMonData(const std::string & varName, char & data);
    static const int GetMonData(const std::string & varName, short & data);
    static const int GetMonData(const std::string & varName, int & data);
    static const int GetMonData(const std::string & varName, unsigned char & data);
    static const int GetMonData(const std::string & varName, unsigned short & data);
    static const int GetMonData(const std::string & varName, unsigned int & data);
    static const int GetMonData(const std::string & varName, double & data);
    static const int GetMonData(const std::string & varName, float & data);
    static const int GetMonData(const std::string & varName, bool & data);
    static const int GetMonData(const std::string & varName, void * pdata, unsigned int len, unsigned int type);

    //////////////////////////////////////////////////////////////////////////
    //Ŀ�������״̬�趨                                                    //
    //��������IP�����������ļ���                                            //
    //////////////////////////////////////////////////////////////////////////

    //����Ŀ���
    const int RebootMachine(void);

    //�洢��¼ӳ����״̬
    const int SaveLogMapMonitorStatus(int waitTime = -1);

    //��ѯ��ǰ�������Ƿ�����Ч������
    const int IsServerValid(bool & valid);

    //�������Ƿ�����
    bool IsConnect(void);

    //��ȡ������ID
    long long int GetId(void);

    //�Ƿ�ͷ������İ汾һ��
    bool IsVersionSameOfServer(int waitTime = -1);

    //���÷��������㾫��
    const int SetFloatPrecision(double preErr, int waitTime = -1);

    //��ȡ���������㾫��
    const int GetFloatPrecision(double & preErr, int waitTime = -1);


    //////////////////////////////////////////////////////////////////////////
    //�ļ�����                                                              //
    //                                                                      //
    //////////////////////////////////////////////////////////////////////////
    //�ļ�������
    const int Ftp_Get(const std::string & filename, const std::string & toPath, int waitTime = -1);

    //�ļ�д����
    const int Ftp_Put(const std::string & filenamePath, int waitTime = -1);

    //��ѯ�ļ��Ƿ����
    const int Ftp_IsExist(const std::string & filename, bool & exist, int waitTime = -1);

    //��ѯ�ļ��Ƿ�·��
    const int Ftp_IsDir(const std::string & filename, bool & bDirectory, int waitTime = -1);

    //�ı�·��
    const int Ftp_CD(const std::string & pathname, bool isRelativeDirectory, int waitTime = -1);

    //����·��
    const int Ftp_MDIR(const std::string & newPathName, int waitTime = -1);

    //��ȡ��ǰ·��
    const int Ftp_PWD(char * currentPathName, unsigned int length, int waitTime = -1);

    //��ȡ��ǰ·���������ļ��б�����·��
    const int Ftp_GetList(std::vector<FileDes_t> * & fileList, int waitTime = -1);

    //�ͷŻ�ȡ���м���б�����ƺͱ������б�
    void Ftp_GetList_FreeRes(std::vector<FileDes_t> * & fileList);

    //ɾ���ļ�
    const int Ftp_Del(const std::string & fileName, int waitTime = -1);

    //ɾ��·�����������ļ�
    const int Ftp_RD(const std::string & pathName, int waitTime = -1);


    //////////////////////////////////////////////////////////////////////////
    //���ʽ������                                                        //
    //����ȫ�ֵ��¼�����                                                    //
    //////////////////////////////////////////////////////////////////////////
    //���ʽ����
    const int InsExp(const std::string & expStr, int & expId, int waitTime = -1);

    //ɾ�����ʽ
    const int DelExp(const std::string & expStr, int waitTime = -1);

    //ɾ�����ʽ
    const int DelExp(int expId, int waitTime = -1);

    //ɾ�����б��ʽ
    const int DelAllExp(int waitTime = -1);

    //��ȡ���еı��ʽ
    const int GetAllExp(std::vector<std::string> * & expStrList, std::vector<int> * & expIdList, int waitTime = -1);

    //�ͷŶ�ȡ���еı��ʽ
    void GetAllExp_FreeRes(std::vector<std::string> * & expStrList, std::vector<int> * & expIdList);

    //���ھ�̬���߳̿�����⣬���ܽ���ʹ���ڴ���䣬����޸��࣬ʹ���е��ڴ������ͷŶ���dll�ڲ�����������û������
    //��������
    static CRass * New(void);
    //ɾ������
    static void Delete(CRass * & p);
    //��������
    static CRass & NewRef(void);
    //ɾ������
    static void Delete(CRass & ref);

private:
    //������Dll����ֹ�ڿͻ��������ڴ����
    CRass();
    virtual ~CRass();

    //��ֹ�����͸���
    CRass(const CRass& rhs);
    const CRass& operator=(const CRass& rhs);

    void * p;
    void * pRet;
};

#endif
