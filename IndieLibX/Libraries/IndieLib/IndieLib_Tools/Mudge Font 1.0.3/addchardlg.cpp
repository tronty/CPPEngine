/**
 * Copyright (c) 2003 mudGE Entertainment
 * 
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising 
 * from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute 
 * it freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; 
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment 
 *    in the product documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must 
 *    not be misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 */
/*
** Changes Copyright 2004 Larry Hastings
** 
** All my changes are released with the same license as the above.
** 
*/
// --> Include files <---------------------------------------------------------

#include "stdafx.h"
#include "mudgefont.h"
#include "AddCharDlg.h"

// --> Static members <--------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CAddCharDlg, CDialog)
	//{{AFX_MSG_MAP(CAddCharDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// --> Class implementation <--------------------------------------------------

HFONT makeFont(const char *name, __DWORD__ height, __DWORD__ weight)
	{
	LOGFONT logfont;
	stx_memset(&logfont, 0, sizeof(logfont));
	logfont.lfHeight = height;
	logfont.lfWidth = 0;
	logfont.lfWeight = weight;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = ANTIALIASED_QUALITY;
	logfont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strcpy(logfont.lfFaceName, name);
	return CreateFontIndirect(&logfont);
	}


static HFONT  hFont = 0;
static CFont *cFont = 0;


CAddCharDlg::CAddCharDlg(CWnd* pParent /*=0*/)
	: CDialog(CAddCharDlg::IDD, pParent)
{
	if (hFont == 0)
		hFont = makeFont("Courier New", 28, FW_BOLD);
	if (cFont == 0)
		cFont = CFont::FromHandle(hFont);
	//{{AFX_DATA_INIT(CAddCharDlg)
	m_Chars = _T("");
	//}}AFX_DATA_INIT
}


void CAddCharDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddCharDlg)
	DDX_Control(pDX, IDC_EDIT2, m_String);
	DDX_Text(pDX, IDC_EDIT2, m_Chars);
	//}}AFX_DATA_MAP
}


void CAddCharDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	UpdateData(TRUE);
}


BOOL CAddCharDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_String.SetFont(cFont, TRUE);
	m_String.SetFocus();
	return FALSE;
}

