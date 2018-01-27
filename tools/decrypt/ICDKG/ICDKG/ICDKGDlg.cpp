
// ICDKGDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ICDKG.h"
#include "ICDKGDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "jencrypt/jencrypt_inc.h"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CICDKGDlg 对话框




CICDKGDlg::CICDKGDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CICDKGDlg::IDD, pParent)
	, m_CPUID(_T(""))
	, m_HDID(_T(""))
	, m_MACID(_T(""))
	, m_SN(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CICDKGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SN, m_editSN);
	DDX_Text(pDX, IDC_EDIT_CPUID, m_CPUID);
	DDX_Text(pDX, IDC_EDIT_HDID, m_HDID);
	DDX_Text(pDX, IDC_EDIT_MACID, m_MACID);
	DDX_Text(pDX, IDC_EDIT_SN, m_SN);
}

BEGIN_MESSAGE_MAP(CICDKGDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BTN_GENERATE, &CICDKGDlg::OnBnClickedBtnGenerate)
	ON_BN_CLICKED(ID_BTN_COPY, &CICDKGDlg::OnBnClickedBtnCopy)
	ON_BN_CLICKED(ID_BTN_EXIT, &CICDKGDlg::OnBnClickedBtnExit)
END_MESSAGE_MAP()


// CICDKGDlg 消息处理程序

BOOL CICDKGDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CICDKGDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CICDKGDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CICDKGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CICDKGDlg::OnBnClickedBtnGenerate()
{
	UpdateData(TRUE);

	m_SN = GenerateSN();

	UpdateData(FALSE);
}

void CICDKGDlg::OnBnClickedBtnCopy()
{
	m_editSN.Copy();
}

void CICDKGDlg::OnBnClickedBtnExit()
{
	EndDialog(IDCANCEL);
}

CString CICDKGDlg::GenerateSN() const
{
	return GenerateSN(m_CPUID + m_HDID + m_MACID);
}

CString CICDKGDlg::GenerateSN(const CString &input)
{
	if (input.IsEmpty()) {
		return CString();
	}

	jencrypt::JMd5 md5;
	md5.update(CStringToString(input));

	std::string _output = md5.toString();
	if (_output.empty()) {
		return CString();
	}

	// reverse
	std::reverse(_output.begin(), _output.end());

	// e
	std::string output;
	for (size_t i = 0; i < _output.size() / 2; ++i) {
		output.append(1, _output[i * 2 + 1]);
	}

	//
	for (size_t i = 4; i < output.size(); i += 5) {
		output.insert(i, 1, '-');
	}

	return CString(output.c_str());
}

std::string CICDKGDlg::CStringToString(const CString &str)
{
	return std::string(CStringA(str));
}
