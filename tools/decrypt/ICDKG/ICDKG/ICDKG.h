
// ICDKG.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CICDKGApp:
// �йش����ʵ�֣������ ICDKG.cpp
//

class CICDKGApp : public CWinApp
{
public:
	CICDKGApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CICDKGApp theApp;