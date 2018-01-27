
// ICDKGDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <string>

// CICDKGDlg 对话框
class CICDKGDlg : public CDialogEx
{
// 构造
public:
	CICDKGDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ICDKG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnGenerate();
	afx_msg void OnBnClickedBtnCopy();
	afx_msg void OnBnClickedBtnExit();
	DECLARE_MESSAGE_MAP()

private:
	CString GenerateSN() const;
	static CString GenerateSN(const CString &input);
	static std::string CStringToString(const CString &str);

private:
	HICON m_hIcon;
	CEdit m_editSN;
	CString m_CPUID;
	CString m_HDID;
	CString m_MACID;
	CString m_SN;
};
