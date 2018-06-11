//Rass控制库头文件
#ifndef RASS_H
#define RASS_H

#include <string>
#include <vector>

#define NONE_ENOUGH_MEMORY -200     //如果在返回字符串的时候，缓存空间不够，返回此值

//模型运行状态
enum SERVER_RUN_STATUS {
    SRS_UNKONW = 0,
    SRS_RUN,
    SRS_READY,
    SRS_STEP,
    SRS_STOP
};

//变量描述
#pragma pack(1)
struct ModelVarDes_t {
    unsigned char   varClass;       //变量类型，是否是信号
    unsigned char   dataType;       //模型变量类型(0:double, 1:float, 2:char, 3:uchar, 4:short, 5:ushort, 6:int, 7:uint, 8:bool, 200:fixpoint, 255:struct)
    unsigned short  dataSize;       //模型变量每项大小,总大小为维数的乘积和数据项大小的乘积
    unsigned int    nDims;          //几维
    unsigned int    dim[10];        //维数
    bool            isComplex;      //是否是复数
    bool            isPointer;      //是否需要指针访问
    bool            isFixPointData; //是否是定点数

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

//文件描述结构
struct FileDes_t
{
    long long int   fileSize;       //文件大小
    char            filename[512];  //文件名
    bool            isDirectory;    //是否路径
    unsigned int    year;           //年
    unsigned char   month;          //月
    unsigned char   day;            //日
    unsigned char   hour;           //时
    unsigned char   minute;         //分
    unsigned char   second;         //秒
};

//映射变量结构描述
struct MappingVarDes_t{
    std::string         name;               //映射的模型可操作变量名称
    unsigned int        direction;          //映射的方向，1:模型到内存 2:内存到模型
    unsigned int        shareMemType;       //共享内存类型，1表示反射内存，2表示UDP共享内存，3表示本地共享内存
    unsigned int        mapAddress;         //共享内存地址
    unsigned int        length;             //映射的长度
    unsigned int        intervalCount;      //变量映射间隔次数，即在次数的整数倍上才进行映射操作，提供用户低速率信号要求，但是过高的映射速度占用极大带宽，甚至模型运行超时

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

//映射队列信息描述
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

//记录信息结构
struct LogInfo_t {
    bool                bLogStarted;                    //记录是否已经开始，开始表示记录文件已经打开
    bool                bLogEnable;                     //记录是否使能，表示记录使能。
    char                logMasterFileName[1024];        //记录主文件名
    bool                bDoNotAutoGenTimeFileName;      //记录文件名是否使用时间，配合主文件名生成最终文件名
    char                logFileName[1024];              //记录最终文件名
    int                 logIntervalTime;                //间隔记录周期，为基频率的整数倍
    unsigned int        logVarsStatusCnt;               //记录队列的变更计数器，发现数据比本地更新，那么需要进行记录变量列表同步
};
#pragma pack()

typedef int (__stdcall * MonitorGotNewData_f)(const std::string & monListName, double time);
typedef int (__stdcall * AllServerMonitorGotNewData_f)(long long Id, const std::string & monListName, double time);
typedef int (__stdcall * FindNewServer_f)(const std::string & name, const std::string & addr, unsigned short port);

//////////////////////////////////////////////////////////////////////////
//模型控制类
//等待超时为
//  -1   :   时表示无限等待，
//  0    :   的时候表示立即返回，不管命令的执行结果
//  >0   :   的时候表示的等待时间，单位ms，
//  其他 :   为非法值
//加载、卸载模型耗时间比较长，所以需要等待时间较长，通常可以设为无限等待，或者大于100ms
//模型运行控制时间设为大于5ms即可，或者无限等待
//////////////////////////////////////////////////////////////////////////

class __declspec(dllexport) CRass
{
public:
    //////////////////////////////////////////////////////////////////////////
    //目标机工作状态设定                                                    //
    //设置诸如IP，启动配置文件等                                            //
    //////////////////////////////////////////////////////////////////////////

    //连接目标机
    const int Connect(const std::string & targetIP = "", int port = 0, std::string localif = "");

    //关闭目标机
    const int Close(void);

    //////////////////////////////////////////////////////////////////////////
    //模型控制                                                              //
    //负责目标机模型的加载和运行                                            //
    //////////////////////////////////////////////////////////////////////////
    //模型加载
    const int Load(
        const std::string & modelPath,                          //模型全路径
        const std::string & modelName,                          //模型名称
        int                 waitTime,                           //等待超时时间
        bool                bClearVarLogTable     = true,       //是否清除变量记录表
        bool                bClearVarMappingTable = true,       //是否清除变量映射表
        bool                bClearVarMonitorTable = true,       //是否清除变量监控表
        bool                bForeceClearLogMappingMonitorVars = false,  //是否强制执行用于请求的清除变量指令
        double              freqScale = 1,                      //模型频率的调整量
        bool                freeRun = false                     //是否自由运行
        );

    //读取模型路径和名称
    const int GetLoadedInfo(char * path, unsigned int pathLen, char * name, unsigned int nameLen, int waitTime = -1);

    //模型卸载
    const int Unload(bool reserveLogMappingMonitor, int waitTime = -1) ;

    //重新加载模型符号表
    const int ReloadSymbolTalbe(int waitTime = -1);

    //获取模型基础频率时间
    const int GetModelBaseFreq(double & freqTime, int waitTime = -1);

    //设置模型运行模式
    const int SetModelRunMode(double freqScale = 1, bool freeRun = false, int waitTime = -1);

    //获取模型运行模式信息
    const int GetModelRunMode(double & freqScale, bool & freeRun, int waitTime = -1);

    //同步服务器全部数据
    const int SyncServer(int waitTime = -1);

    //获取所有变量的列表
    const int GetAllVarsList(std::vector<std::string> * & varsList);

    //释放读取变量的列表
    void GetAllVarsList_FreeRes(std::vector<std::string> * & pStr);

    //模型初始化
    const int Init(int waitTime = -1) ;

    //模型运行
    const int Run(int waitTime = -1) ;

    //暂停模型运行
    const int Pause(int waitTime = -1) ;

    //步进模型，可以是单步和多步
    const int Step(const unsigned int nStep, int waitTime = -1) ;

    //步进模型，可以是单步和多步
    const int StepSync(const unsigned int nStep, int waitTime = -1) ;

    //停止模型
    const int Stop(int waitTime = -1) ;

    //查询模型运行状态
    const int GetModelRunStatus(bool & haveLoadModel, SERVER_RUN_STATUS & modelStatus, int waitTime = -1) ;

    //获取模型步进运行状态，是否还在步进之中
    const int GetStepRuningStatus(bool & isRunningStep, int waitTime = -1) ;

    //设置模型运行限制时间
    const int SetModelRunTimeLimit(double timeLimit, int waitTime = -1) ;

    //获取模型运行限制时间
    const int GetModelRunTimeLimit(double & timeLimit, int waitTime = -1);

    //获取模型当前运行时间
    const int GetCurrentModelRunTime(double & time, int waitTime = -1) ;

    //设置模型暂停检测表达式
    const int SetPauseCheckExpression(const std::string & expressionStr, int waitTime = -1);

    //获取模型暂停检测表达式
    const int GetPauseCheckExpression(char * expressionStr, unsigned int expStrLen, int waitTime = 1);

    //清除模型暂停检测表达式
    const int ClearPauseCheckExpression(int waitTime = -1);

    //设置使能模型暂停检测表达式
    const int SetEnablePauseCheck(bool enalbeFlag, int waitTime = -1);

    //查询使能模型暂停检测表达式
    const int IsEnablePauseCheck(bool & enalbeFlag, int waitTime = -1);

    //复位模型运行时间统计信息
    const int ResetModelRunTimeStatInfo(int waitTime = -1);

    //设置模型可以修改其自身的符号表
    const int SetModelCanModifyItselfSymbolTable(bool enableFlag, int waitTime = -1);

    //////////////////////////////////////////////////////////////////////////
    //模型变量修改查询                                                      //
    //负责如何修改和查询模型中的变量                                        //
    //////////////////////////////////////////////////////////////////////////

    //查询模型变量的信息
    const int IsVarOperable(const std::string & varStr, bool & isOperable, int waitTime = -1) ;

    //查询模型变量的信息
    const int GetVarDes(const std::string & varStr, ModelVarDes_t & varDes, int waitTime = -1);

    //恢复修改变量的原始值
    const int ResetVarValue(const std::string & varStr, int waitTime = -1) ;

    //恢复所有变量的原始值
    const int ResetAllVarValues(int waitTime = -1) ;

    //修改单个模型的变量，实数
    const int ModifyVar(const std::string & varStr, char            i8  , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, unsigned char   ui8 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, short           i16 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, unsigned short  ui16, int waitTime = -1);
    const int ModifyVar(const std::string & varStr, int             i32 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, unsigned int    ui32, int waitTime = -1);
    const int ModifyVar(const std::string & varStr, float           f32 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, double          f64 , int waitTime = -1);
    const int ModifyVar(const std::string & varStr, bool            b8  , int waitTime = -1);

    //修改单个模型的变量，复数
    const int ModifyVarComplex(const std::string & varStr, char            real_i8  , char            imn_i8  , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, unsigned char   real_ui8 , unsigned char   imn_ui8 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, short           real_i16 , short           imn_i16 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, unsigned short  real_ui16, unsigned short  imn_ui16, int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, int             real_i32 , int             imn_i32 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, unsigned int    real_ui32, unsigned int    imn_ui32, int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, float           real_f32 , float           imn_f32 , int waitTime = -1);
    const int ModifyVarComplex(const std::string & varStr, double          real_f64 , double          imn_f64 , int waitTime = -1);
                                                             
    //修改单个的定点数
    const int ModifyVarFixPoint(const std::string & varStr, double f64 , int waitTime = -1);
    const int ModifyVarComplexFixPoint(const std::string & varStr, double real_f64 , double imn_f64 , int waitTime = -1);

    //用内存填充模型变量区域，这里不能进行严格的类型检查，所以谨慎使用
    const int ModifyVar(const std::string & varStr, void * pNewVale, unsigned int size, int waitTime = -1);

    //读取单个模型的变量，实数
    const int ReadVar(const std::string & varStr, char            & i8  , int waitTime = -1);
    const int ReadVar(const std::string & varStr, unsigned char   & ui8 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, short           & i16 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, unsigned short  & ui16, int waitTime = -1);
    const int ReadVar(const std::string & varStr, int             & i32 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, unsigned int    & ui32, int waitTime = -1);
    const int ReadVar(const std::string & varStr, float           & f32 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, double          & f64 , int waitTime = -1);
    const int ReadVar(const std::string & varStr, bool            & b8  , int waitTime = -1);

    //读取单个模型的变量，复数
    const int ReadVarComplex(const std::string & varStr, char            & real_i8  , char            & imn_i8  , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, unsigned char   & real_ui8 , unsigned char   & imn_ui8 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, short           & real_i16 , short           & imn_i16 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, unsigned short  & real_ui16, unsigned short  & imn_ui16, int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, int             & real_i32 , int             & imn_i32 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, unsigned int    & real_ui32, unsigned int    & imn_ui32, int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, float           & real_f32 , float           & imn_f32 , int waitTime = -1);
    const int ReadVarComplex(const std::string & varStr, double          & real_f64 , double          & imn_f64 , int waitTime = -1);
                                      
    //读取单个的定点数
    const int ReadVarFixPoint(const std::string & varStr, double & f64 , int waitTime = -1);
    const int ReadVarComplexFixPoint(const std::string & varStr, double & real_f64 , double & imn_f64 , int waitTime = -1);

    //用内存填充模型变量区域，这里不能进行严格的类型检查，所以谨慎使用
    const int ReadVar(const std::string & varStr, void * pNewVale, unsigned int size, int waitTime = -1);

    //设置某个变量的所有维数，所有的实部，虚部，都为某一个常量
    const int SetVarEntireItem(const std::string & varStr, char           i8  , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, unsigned char  ui8 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, short          i16 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, unsigned short ui16, int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, int            i32 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, unsigned int   ui32, int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, float          f32 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, double         f64 , int waitTime = -1);
    const int SetVarEntireItem(const std::string & varStr, bool           b8  , int waitTime = -1);

    //保存变量状态到服务器文件
    const int SaveModelVarStateToServerFile(const std::string & fileName, int waitTime = -1);

    //从服务器文件载入变量状态
    const int LoadModelVarStateFromServerFile(const std::string & fileName, int waitTime = -1);

    //从本地文件载入变量状态
    //此函数必须在同步服务器之后才能使用（SyncServer）
    const int LoadModelVarStateFromLocalFile(const std::string & fileName, int & successItemCnt, int & failItemCnt, int itemModifyWaitTime = -1);

    //////////////////////////////////////////////////////////////////////////
    //模型变量记录                                                          //
    //负责如何记录模型中的变量                                              //
    //一个模型同一个时间只允许记录一个文件                                  //
    //////////////////////////////////////////////////////////////////////////

    //设置模型记录主文件名
    const int SetLogMasterFileName(const std::string & mainFileName, int waitTime = -1) ;

    //获取模型记录主文件名
    const int GetLogMasterFileName(char * mainFileName, unsigned int fileNameLen, int waitTime = -1) ;

    //获取最终的模型记录名
    const int GetLogFinalFileName(char * finalFileName, unsigned int fileNameLen, int waitTime = -1) ;

    //模型记录初始化,最终的名字有主文件名+时间形成
    const int InitLog(int waitTime = -1) ;

    //模型记录关闭
    const int CloseLog(int waitTime = -1) ;

    //模型记录变量添加
    const int AddLogVar(const std::string & varStr, int waitTime = -1) ;

    //模型记录变量删除
    const int DelLogVar(const std::string & varStr, int waitTime = -1) ;

    //模型记录变量全删除
    const int DelAllLogVar(int waitTime = -1) ;

    //模型记录开始
    const int StartLog(int waitTime = -1) ;

    //模型记录停止
    const int StopLog(int waitTime = -1) ;

    //模型记录周期时间设定,间隔时间为模型基频的倍数值
    const int SetLogIntervalTime(int intervalTime, int waitTime = -1) ;

    //获取模型记录间隔时间
    const int GetLogIntervalTime(int & intervalTime, int waitTime = -1) ;

    //从文件添加记录列表
    const int AddLogVarFromFile(const std::string & fileName, int waitTime = -1);

    //设置不使用时间来生成模型记录文件名，也即只是用主文件名作为文件名
    const int SetLogFileNameUnTimeAppend(bool flag, int waitTime);

    //查询不使用时间来生成模型记录文件名，也即只是用主文件名作为文件名
    const int IsLogFileNameTimeAppend(bool & flag, int waitTime);

    //获取所有的记录变量列表
    const int GetLogVarList(std::vector<std::string> * & logVarsList, int waitTime = -1);

    //释放获取所有的记录变量列表
    void GetLogVarList_FreeRes(std::vector<std::string> * & logVarsList);

    //获取记录的信息
    const int GetLogInfo(LogInfo_t & logInfo, int waitTime = -1);

    //////////////////////////////////////////////////////////////////////////
    //模型映射记录                                                          //
    //负责如何映射模型中的变量                                              //
    //////////////////////////////////////////////////////////////////////////
    //模型映射变量添加
    const int AddMappingVar(
        const std::string & varStr, //模型变量的字符串描述
        unsigned int direction,     //模型变量映射的方向,1模型到共享内存,2共享内存到模型
        unsigned int addr,          //映射的地址
        unsigned int mediumType,    //映射的类型，1表示反射内存，2表示UDP共享内存，3表示本地共享内存
        unsigned int intervalTime,  //映射间隔时间，允许每个变量的映射频率都不同
        int waitTime = -1) ;

    //模型映射变量删除
    const int DelMappingVar(const std::string & varStr, int waitTime = -1) ;

    //模型映射变量全部删除
    const int DelAllMappingVar(int waitTime = -1) ;

    //模型映射开始
    const int StartMapping(int waitTime = -1) ;

    //模型映射停止
    const int StopMapping(int waitTime = -1) ;

    //模型是否开始
    const int IsMappingStarted(bool & isEnable, int waitTime = -1) ;

    //清除指定区域的内存（闭区间）
    const int ClearShareMemoryByAddr(unsigned int shareMemType, unsigned int beginAddress, unsigned int endAddress, int waitTime = -1);

    //清除映射区域的内存（闭区间）cleanFlag: SHARE_MEM_CLEAN_IN(1),SHARE_MEM_CLEAN_OUT(2) 可以或起来
    const int ClearShareMemoryOfMappingVars(unsigned int cleanFlag, int waitTime = -1);

    //从文件加载映射信息
    const int AddMappingVarFromFile(const std::string & fileName, int waitTime = -1);

    //读取所有的映射列表信息
    const int GetMappingVarList(std::vector<MappingVarDes_t> * & varList, int waitTime = -1);

    //释放读取最终的模型记录名
    void GetMappingVarList_FreeRes(std::vector<MappingVarDes_t> * & varList);

    //////////////////////////////////////////////////////////////////////////
    //模型监控                                                              //
    //负责如何映射模型中的变量                                              //
    //////////////////////////////////////////////////////////////////////////
    //模型监控变量添加
    const int AddGMonVar(const std::string & varStr, int waitTime = -1);

    //模型监控变量删除
    const int DelGMonVar(const std::string & varStr, int waitTime = -1);

    //模型监控变量全部删除
    const int DelAllGMonVar(int waitTime = -1);

    //模型全局监控允许
    const int EnableGMon(int waitTime = -1);

    //模型全局监控禁止
    const int DisableGMon(int waitTime = -1);

    //模型监控周期时间设定,间隔时间
    const int SetGMonIntervalTime(unsigned int intervalTime, int waitTime = -1);

    //查询模型全局监控是否允许
    const int IsGMonEnable(bool & enable, int waitTime = -1);

    //查询模型全局监控间隔时间
    const int GetGMonIntervalTime(unsigned int & intervalTime, int waitTime = -1);

    //设置全局监控的组播信息，ip，端口
    static void SetMCastIp(const std::string & localIp, const std::string & mcastIp = "255.5.5.5", unsigned short port = 22000);

    //开始全局监控
    static int StartGMonAllServer(void);

    //关闭全局监控队列
    static int StopGMonAllServer(void);

    //////////////////////////////////////////////////////////////////////////
    //专用于监控子队列的函数

    //创建一个监控子队列，如队列已经存在，那么引用它
    const int OpenCMon(const std::string & monListName, int waitTime = -1);

    //关闭一个监控队列，如果队列的被引用计数为0，那么删除队列
    const int CloseCMon(const std::string & monListName, int waitTime = -1);

    //模型监控变量添加
    const int AddCMonVar(const std::string & monListName,const std::string & varStr, int waitTime = -1);

    //模型监控变量删除
    const int DelCMonVar(const std::string & monListName,const std::string & varStr, int waitTime = -1);

    //模型变量全部删除
    const int DelAllCMonVar(const std::string & monListName, int waitTime = -1);

    //模型监控开始
    const int EnableCMon(const std::string & monListName, int waitTime = -1);

    //模型监控停止
    const int DisableCMon(const std::string & monListName, int waitTime = -1);

    //模型监控周期时间设定
    const int SetCMonIntervalTime(const std::string & monListName, unsigned int intervalTime, int waitTime = -1);

    //查询模型子监控是否允许
    const int IsCMonEnable(const std::string & monListName, bool & enable, int waitTime = -1);

    //查询模型子监控间隔时间
    const int GetCMonIntervalTime(const std::string & monListName, unsigned int & intervalTime, int waitTime = -1);

    //删除所有
    const int CloseAllCMon(int waitTime = -1);

    //获取所有监控列表的名称和变量名列表
    const int GetAllCMonDes(std::vector<MonitorGroupDes_t> * & monGroupList, int waitTime = -1);

    //释放获取所有监控列表的名称和变量名列表
    void GetAllCMonDes_FreeRes(std::vector<MonitorGroupDes_t> * & monGroupList);

    //强行刷新监控数据
    const int ForceCMonRefreshOneFrameData(int waitTime = -1);

    //设置发现新的服务器回调函数
    static void SetFindNewServerCallFunc(FindNewServer_f pfunc);

    //设置全局监控数据回调函数
    static void SetAllServerMonitorReadDataFunc(AllServerMonitorGotNewData_f pfunc);

    //设置监控读数据函数
    void SetMonitorReadDataFunc(MonitorGotNewData_f pfunc);

    //读取监控数据的函数
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
    //目标机工作状态设定                                                    //
    //设置诸如IP，启动配置文件等                                            //
    //////////////////////////////////////////////////////////////////////////

    //重启目标机
    const int RebootMachine(void);

    //存储记录映射监控状态
    const int SaveLogMapMonitorStatus(int waitTime = -1);

    //查询当前服务器是否是有效服务器
    const int IsServerValid(bool & valid);

    //服务器是否连接
    bool IsConnect(void);

    //获取服务器ID
    long long int GetId(void);

    //是否和服务器的版本一致
    bool IsVersionSameOfServer(int waitTime = -1);

    //设置服务器浮点精度
    const int SetFloatPrecision(double preErr, int waitTime = -1);

    //获取服务器浮点精度
    const int GetFloatPrecision(double & preErr, int waitTime = -1);


    //////////////////////////////////////////////////////////////////////////
    //文件函数                                                              //
    //                                                                      //
    //////////////////////////////////////////////////////////////////////////
    //文件读函数
    const int Ftp_Get(const std::string & filename, const std::string & toPath, int waitTime = -1);

    //文件写函数
    const int Ftp_Put(const std::string & filenamePath, int waitTime = -1);

    //查询文件是否存在
    const int Ftp_IsExist(const std::string & filename, bool & exist, int waitTime = -1);

    //查询文件是否路径
    const int Ftp_IsDir(const std::string & filename, bool & bDirectory, int waitTime = -1);

    //改变路径
    const int Ftp_CD(const std::string & pathname, bool isRelativeDirectory, int waitTime = -1);

    //创建路径
    const int Ftp_MDIR(const std::string & newPathName, int waitTime = -1);

    //获取当前路径
    const int Ftp_PWD(char * currentPathName, unsigned int length, int waitTime = -1);

    //获取当前路径下所有文件列表，包括路径
    const int Ftp_GetList(std::vector<FileDes_t> * & fileList, int waitTime = -1);

    //释放获取所有监控列表的名称和变量名列表
    void Ftp_GetList_FreeRes(std::vector<FileDes_t> * & fileList);

    //删除文件
    const int Ftp_Del(const std::string & fileName, int waitTime = -1);

    //删除路径及其所有文件
    const int Ftp_RD(const std::string & pathName, int waitTime = -1);


    //////////////////////////////////////////////////////////////////////////
    //表达式管理部分                                                        //
    //用于全局的事件服务                                                    //
    //////////////////////////////////////////////////////////////////////////
    //表达式管理
    const int InsExp(const std::string & expStr, int & expId, int waitTime = -1);

    //删除表达式
    const int DelExp(const std::string & expStr, int waitTime = -1);

    //删除表达式
    const int DelExp(int expId, int waitTime = -1);

    //删除所有表达式
    const int DelAllExp(int waitTime = -1);

    //读取所有的表达式
    const int GetAllExp(std::vector<std::string> * & expStrList, std::vector<int> * & expIdList, int waitTime = -1);

    //释放读取所有的表达式
    void GetAllExp_FreeRes(std::vector<std::string> * & expStrList, std::vector<int> * & expIdList);

    //由于静态多线程库的问题，不能交叉使用内存分配，因此修改类，使所有的内存分配和释放都在dll内部，这样才能没有问题
    //创建对象
    static CRass * New(void);
    //删除对象
    static void Delete(CRass * & p);
    //创建对象
    static CRass & NewRef(void);
    //删除对象
    static void Delete(CRass & ref);

private:
    //由于是Dll，防止在客户方进行内存分配
    CRass();
    virtual ~CRass();

    //禁止拷贝和复制
    CRass(const CRass& rhs);
    const CRass& operator=(const CRass& rhs);

    void * p;
    void * pRet;
};

#endif
