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
#include "addchardlg.h"
#include "mudgefontdlg.h"
#include "mudgefont.h"

// --> Static members <--------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




BEGIN_MESSAGE_MAP(CMudgeFontDlg, CDialog)
	//{{AFX_MSG_MAP(CMudgeFontDlg)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_H64, OnSizeChange)
	ON_BN_CLICKED(IDC_ITALIC, OnFontChange)
	ON_BN_CLICKED(IDC_RECT, OnRectChange)
	ON_EN_CHANGE(IDC_CBL_EDIT, OnCharChange)
	ON_CBN_SELCHANGE(IDC_CHAR, OnSelchangeChar)
	ON_COMMAND(ID_TOOLS_ADDCHAR, OnToolsAddchar)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(IDC_MUDGEFONT_HOME_PAGE, OnMudgefontHomePage)
	ON_BN_CLICKED(IDC_H512, OnSizeChange)
	ON_BN_CLICKED(IDC_H256, OnSizeChange)
	ON_BN_CLICKED(IDC_H128, OnSizeChange)
	ON_BN_CLICKED(IDC_W64, OnSizeChange)
	ON_BN_CLICKED(IDC_W512, OnSizeChange)
	ON_BN_CLICKED(IDC_W256, OnSizeChange)
	ON_BN_CLICKED(IDC_W128, OnSizeChange)
	ON_CBN_SELCHANGE(IDC_QUALITY, OnFontChange)
	ON_EN_CHANGE(IDC_SIZE_EDIT, OnFontChange)
	ON_CBN_SELCHANGE(IDC_WEIGHT, OnFontChange)
	ON_CBN_SELCHANGE(IDC_FONT, OnFontChange)
	ON_EN_CHANGE(IDC_BB_EDIT, OnFontChange)
	ON_EN_CHANGE(IDC_BL_EDIT, OnFontChange)
	ON_EN_CHANGE(IDC_BR_EDIT, OnFontChange)
	ON_EN_CHANGE(IDC_BT_EDIT, OnFontChange)
	ON_EN_CHANGE(IDC_CBR_EDIT, OnCharChange)
	ON_CBN_SELCHANGE(IDC_AA, OnFontChange)
	ON_BN_CLICKED(IDC_FORCE_MONOSPACED, OnFontChange)
	ON_BN_DOUBLECLICKED(IDC_FORCE_MONOSPACED, OnFontChange)
	ON_CBN_SELCHANGE(IDC_FORCE_MONOSPACED, OnFontChange)
	ON_EN_CHANGE(IDC_CBB_EDIT, OnCharChange)
	ON_EN_CHANGE(IDC_CBT_EDIT, OnCharChange)
	ON_COMMAND(IDC_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// --> Class implementation <--------------------------------------------------

CMudgeFontDlg::CMudgeFontDlg(CWnd* pParent /*=0*/): 
	CDialog(CMudgeFontDlg::IDD, pParent),
	m_Running(false)
{
	stx_memset(filename, 0, sizeof(filename));


	//{{AFX_DATA_INIT(CMudgeFontDlg)
	m_ZoomText = _T("");
	m_Height = -1;
	m_Width = -1;
	m_Italic = FALSE;
	m_ShowRect = FALSE;
	m_forceMonospaced = -1;
	//}}AFX_DATA_INIT
}


void CMudgeFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMudgeFontDlg)
	DDX_Control(pDX, IDC_AA, m_AA);
	DDX_Control(pDX, IDC_CBR_SPIN, m_cbrSpin);
	DDX_Control(pDX, IDC_CBR_EDIT, m_cbrEdit);
	DDX_Control(pDX, IDC_CBL_SPIN, m_cblSpin);
	DDX_Control(pDX, IDC_CBL_EDIT, m_cblEdit);
	DDX_Control(pDX, IDC_CBT_SPIN, m_cbtSpin);
	DDX_Control(pDX, IDC_CBT_EDIT, m_cbtEdit);
	DDX_Control(pDX, IDC_CBB_SPIN, m_cbbSpin);
	DDX_Control(pDX, IDC_CBB_EDIT, m_cbbEdit);
	DDX_Control(pDX, IDC_CHAR, m_CharSel);
	DDX_Control(pDX, IDC_BT_SPIN, m_btSpin);
	DDX_Control(pDX, IDC_BT_EDIT, m_btEdit);
	DDX_Control(pDX, IDC_BR_SPIN, m_brSpin);
	DDX_Control(pDX, IDC_BR_EDIT, m_brEdit);
	DDX_Control(pDX, IDC_BL_SPIN, m_blSpin);
	DDX_Control(pDX, IDC_BL_EDIT, m_blEdit);
	DDX_Control(pDX, IDC_BB_SPIN, m_bbSpin);
	DDX_Control(pDX, IDC_BB_EDIT, m_bbEdit);
	DDX_Control(pDX, IDC_WEIGHT, m_Weight);
	DDX_Control(pDX, IDC_SIZE_SPIN, m_SizeSpin);
	DDX_Control(pDX, IDC_SIZE_EDIT, m_SizeEdit);
	DDX_Control(pDX, IDC_QUALITY, m_Quality);
	DDX_Control(pDX, IDC_FONT, m_FontName);
	DDX_Control(pDX, IDC_ZOOM, m_Zoom);
	DDX_Control(pDX, IDC_UP, m_Up);
	DDX_Control(pDX, IDC_RIGHT, m_Right);
	DDX_Control(pDX, IDC_VIEW, m_View);
	DDX_Text(pDX, IDC_ZOOM_TEXT, m_ZoomText);
	DDX_Radio(pDX, IDC_H64, m_Height);
	DDX_Radio(pDX, IDC_W64, m_Width);
	DDX_Check(pDX, IDC_ITALIC, m_Italic);
	DDX_Check(pDX, IDC_RECT, m_ShowRect);
	DDX_CBIndex(pDX, IDC_FORCE_MONOSPACED, m_forceMonospaced);
	//}}AFX_DATA_MAP
}


extern CMudgeFontApp theApp;

BOOL CMudgeFontDlg::OnInitDialog()
{
	int index;
	CDialog::OnInitDialog();

	HICON icon = theApp.LoadIcon(MAKEINTRESOURCE(IDI_MUDGE));
	if (icon != 0)
		SetIcon(icon, FALSE);

	// set initial values.
	m_Zoom.SetRange(100,1000, TRUE);
	m_Zoom.SetPos(100);
	m_Zoom.SetTicFreq(50);
	m_View.ShowWindow(SW_HIDE);
	m_ZoomText.Format("%d%%", m_Zoom.GetPos());
	m_Width = 2;
	m_Height = 2;
	m_Border = CRect(0,0,0,0);
	UpdateData(FALSE);

	// enumerate fonts.
	CDC* pDC = GetDC();
	EnumFonts(pDC->GetSafeHdc(), 0, (FONTENUMPROC)EnumFontsProc, (LPARAM)&m_FontName);
	m_FontName.SetCurSel(0);

	// add anti-aliasing.
	index = m_AA.AddString("(none)");
	m_AA.SetItemData(index, 1);
	index = m_AA.AddString("2x2");
	m_AA.SetItemData(index, 2);
	index = m_AA.AddString("3x3");
	m_AA.SetItemData(index, 3);
	index = m_AA.AddString("4x4");
	m_AA.SetItemData(index, 4);
	m_AA.SetCurSel(0);

	// add qualities.
	index = m_Quality.AddString("Antialiased");
	m_Quality.SetItemData(index, ANTIALIASED_QUALITY);
	index = m_Quality.AddString("non-Antialiased");
	m_Quality.SetItemData(index, NONANTIALIASED_QUALITY);
	m_Quality.SetCurSel(0);

#ifdef CLEARTYPE_QUALITY
	index = m_Quality.AddString("Cleartype");
	m_Quality.SetItemData(index, CLEARTYPE_QUALITY);
#endif

	// add weights.
	index = m_Weight.AddString("Normal");
	m_Weight.SetItemData(index, FW_NORMAL);
	index = m_Weight.AddString("Medium");
	m_Weight.SetItemData(index, FW_SEMIBOLD);
	index = m_Weight.AddString("Bold");
	m_Weight.SetItemData(index, FW_BOLD);
	m_Weight.SetCurSel(0);

	// setup size control.
	m_SizeSpin.SetBuddy(&m_SizeEdit);
	m_SizeSpin.SetRange(5, 100);
	m_SizeSpin.SetPos(11);

	// setup border controls.
	m_blSpin.SetBuddy(&m_blEdit);
	m_blSpin.SetRange32(-255, 255);
	m_blSpin.SetPos(0);
	m_brSpin.SetBuddy(&m_brEdit);
	m_brSpin.SetRange32(-255, 255);
	m_brSpin.SetPos(0);
	m_bbSpin.SetBuddy(&m_bbEdit);
	m_bbSpin.SetRange32(-255, 255);
	m_bbSpin.SetPos(0);
	m_btSpin.SetBuddy(&m_btEdit);
	m_btSpin.SetRange32(-255, 255);
	m_btSpin.SetPos(0);

	m_cblSpin.SetBuddy(&m_cblEdit);
	m_cblSpin.SetRange32(-255, 255);
	m_cblSpin.SetPos(0);
	m_cbrSpin.SetBuddy(&m_cbrEdit);
	m_cbrSpin.SetRange32(-255, 255);
	m_cbrSpin.SetPos(0);
	m_cbtSpin.SetBuddy(&m_cbtEdit);
	m_cbtSpin.SetRange32(-255, 255);
	m_cbtSpin.SetPos(0);
	m_cbbSpin.SetBuddy(&m_cbbEdit);
	m_cbbSpin.SetRange32(-255, 255);
	m_cbbSpin.SetPos(0);

	// setup initial font.
	SetupFont();

	// create the dibs for the font.
	CPaintDC dc(this);
	m_MemDC.CreateCompatibleDC(&dc);
	m_MemDib.Create(512, 512, m_MemDC.GetSafeHdc());
	m_Dib.Create(512, 512);
	
	// force one event.
	OnSizeChange();
	m_Running = true;

	m_forceMonospaced = 0;

	return TRUE;
}


void CMudgeFontDlg::OnPaint() 
{
	CPaintDC dc(this);

	CRect destRect;
	CRect sourceRect;
	m_View.GetWindowRect(&destRect);
	::MapWindowPoints(HWND_DESKTOP, m_hWnd, reinterpret_cast<LPPOINT>(&destRect), 2);

	CRgn rgn;
	CPoint pnt1(destRect.left, destRect.top);
	CPoint pnt2(destRect.right, destRect.bottom);
	dc.LPtoDP(&pnt1, 1);
	dc.LPtoDP(&pnt2, 1);
	rgn.CreateRectRgn(pnt1.x, pnt1.y, pnt2.x, pnt2.y);
	dc.SelectClipRgn(&rgn);

	float zoom = (100.0f / m_Zoom.GetPos());
	int width = (int)(destRect.Width() * zoom);
	int height = (int)(destRect.Height() * zoom);

	sourceRect.left = m_Right.GetScrollPos();
	sourceRect.top = 512 - height - m_Up.GetScrollPos();
	sourceRect.right = sourceRect.left + width;
	sourceRect.bottom = sourceRect.top + height;
	m_Dib.Render(dc.GetSafeHdc(), destRect, sourceRect);

	m_Right.SetScrollRange(0, 512 - width);
	m_Up.SetScrollRange(0, 512 - height);

	// show rectangles in green!
	CPen pen(PS_DOT, 1, RGB(0, 192, 0));
	dc.SelectObject(pen);
	dc.SetBkColor(RGB(0, 255, 0));

	if (m_ShowRect)
	{
		int len = m_Chars.GetSize();
		for (int i=0; i<len; i++)
		{
			const cCharacter& ch = m_Chars[i];
			CSize size;
			size.cx = ((m_forceMonospacedInX) ? m_SizeMax.cx : ch.m_Size.cx);
			size.cy = ((m_forceMonospacedInY) ? m_SizeMax.cy : ch.m_Size.cy);

			CRect rect;
			rect.left	= ch.m_Pos.x - m_Right.GetScrollPos();
			rect.top	= ch.m_Pos.y - m_Up.GetScrollPos();
			rect.right	= rect.left + size.cx;
			rect.bottom = rect.top + size.cy;

			rect.left = (int)(rect.left / zoom);
			rect.top = (int)(rect.top / zoom);
			rect.right = (int)(rect.right / zoom);
			rect.bottom = (int)(rect.bottom / zoom);

			::MapWindowPoints(m_View.GetSafeHwnd(), m_hWnd, reinterpret_cast<LPPOINT>(&rect), 2);
			rect.OffsetRect(-1, -1);

			dc.MoveTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			dc.LineTo(rect.right, rect.bottom);
			dc.LineTo(rect.left, rect.bottom);
			dc.LineTo(rect.left, rect.top);
		}
	}
}


void CMudgeFontDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	if (pScrollBar != 0)
	{
		// Get the minimum and maximum scroll-bar positions.
		int minpos;
		int maxpos;
		pScrollBar->GetScrollRange(&minpos, &maxpos);
		maxpos = pScrollBar->GetScrollLimit();
		
		// Get the current position of scroll box.
		int curpos = pScrollBar->GetScrollPos();
		
		// Determine the new position of scroll box.
		switch (nSBCode)
		{
		case SB_TOP:
			curpos = minpos;
			break;
			
		case SB_BOTTOM:
			curpos = maxpos;
			break;
			
		case SB_ENDSCROLL:
			break;
			
		case SB_LINEUP:
			if (curpos > minpos)
				curpos--;
			break;
			
		case SB_LINEDOWN:
			if (curpos < maxpos)
				curpos++;
			break;
			
		case SB_PAGEUP:   // Scroll one page up.
			if (curpos > minpos)
			{
				curpos = max(minpos, curpos - 10);
			}
			break;
			
		case SB_PAGEDOWN:    // Scroll one page down.
			if (curpos < maxpos)
			{
				curpos = min(maxpos, curpos + 10);
			}
			break;
			
		case SB_THUMBPOSITION:
			curpos = nPos; 
			break;
			
		case SB_THUMBTRACK:
			curpos = nPos;
			break;
		}
		
		// Set the new position of the thumb (scroll box).
		pScrollBar->SetScrollPos(curpos);
		InvalidateView();
	}
}


void CMudgeFontDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	if (pScrollBar != 0)
	{
		if (pScrollBar->GetSafeHwnd() == m_Zoom.GetSafeHwnd())
		{
			int pos = m_Zoom.GetPos();
			m_ZoomText.Format("%d%%", pos);
			UpdateData(FALSE);	
			InvalidateView();
		} else
		{
			// Get the minimum and maximum scroll-bar positions.
			int minpos;
			int maxpos;
			pScrollBar->GetScrollRange(&minpos, &maxpos);
			maxpos = pScrollBar->GetScrollLimit();
			
			// Get the current position of scroll box.
			int curpos = pScrollBar->GetScrollPos();
			
			// Determine the new position of scroll box.
			switch (nSBCode)
			{
			case SB_LEFT:    // Scroll to far left.
				curpos = minpos;
				break;
				
			case SB_RIGHT:    // Scroll to far right.
				curpos = maxpos;
				break;
				
			case SB_ENDSCROLL:  // End scroll.
				break;
				
			case SB_LINELEFT:    // Scroll left.
				if (curpos > minpos)
					curpos--;
				break;
				
			case SB_LINERIGHT:  // Scroll right.
				if (curpos < maxpos)
					curpos++;
				break;
				
			case SB_PAGELEFT:   // Scroll one page left.
				if (curpos > minpos)
				{
					curpos = max(minpos, curpos - 10);
				}
				break;
				
			case SB_PAGERIGHT:    // Scroll one page right.
				if (curpos < maxpos)
				{
					curpos = min(maxpos, curpos + 10);
				}
				break;
				
			case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
				curpos = nPos;      // of the scroll box at the end of the drag
				// operation.
				break;
				
			case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is
				// the
				curpos = nPos;     // position that the scroll box has been dragged
				// to.
				break;
			}
			
			// Set the new position of the thumb (scroll box).
			pScrollBar->SetScrollPos(curpos);
			InvalidateView();
		}
	}
}


void CMudgeFontDlg::SetupFont()
{
	int		index;
	int		nHeight;
	int		nWeight;
	int		nQuality;
	CString Facename;

	UpdateData(TRUE);
	m_FontName.GetWindowText(Facename);

	nHeight = m_SizeSpin.GetPos();
	if (nHeight > 100)	nHeight = 100;
	if (nHeight < 5)	nHeight = 5;
	index = m_Weight.GetCurSel();
	nWeight = m_Weight.GetItemData(index);

	index = m_Quality.GetCurSel();
	nQuality = m_Quality.GetItemData(index);

	index = m_AA.GetCurSel();
	nHeight *= m_AA.GetItemData(index);

	// delete old font.
	if (m_DrawFont.GetSafeHandle() != 0)
	{
		m_DrawFont.DeleteObject();
	}

	// create new font.
	CPaintDC dc(this);
	int realHeight = -MulDiv(nHeight, GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY), 72);
	m_DrawFont.CreateFont(realHeight, 0, 0, 0, nWeight, m_Italic, FALSE,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		nQuality, DEFAULT_PITCH | FF_SWISS, Facename);
}


void CMudgeFontDlg::UpdateTextureSize()
{
	for (int y=0; y<512; y++)
	{
		for (int x=0; x<512; x++)
		{
			if (x < m_TextureWidth && y < m_TextureHeight)
			{
				m_Dib.SetPixel(x,y, 0);
			} else
			{
				if ((x % 10) == (y % 10) || ((512-x) % 10) == (y % 10))
				{
					m_Dib.SetPixel(x,y, RGB(224,  64, 224));
				} else
				{
					m_Dib.SetPixel(x,y, RGB(128,  48, 128));
				}
			}
		}
	}
}


void CMudgeFontDlg::UpdateTexture()
{
	// clear the texture.
	UpdateTextureSize();
	UpdateSizes();
	UpdatePositions();

	DrawChars();
	InvalidateView();
}


void CMudgeFontDlg::UpdateSizes()
{
	m_MemDC.SelectObject(m_DrawFont);

	int sel = m_CharSel.GetCurSel();
	m_CharSel.ResetContent();

	int index = m_AA.GetCurSel();
	int sampling = m_AA.GetItemData(index);

	m_SizeMax.cx = m_SizeMax.cy = 0;
	ABC abc;
	int len = m_Chars.GetSize();
	for (int i=0; i<len; i++)
	{
		cCharacter& ch = m_Chars[i];
		m_CharSel.AddString(CString(ch.m_Char));
		ch.m_Size = m_MemDC.GetTextExtent(&ch.m_Char, 1);
		m_MemDC.GetCharABCWidths(ch.m_Char, ch.m_Char, &abc);
		ch.m_Size.cx = abs(abc.abcB);
		
		ch.m_Size.cx /= sampling;
		ch.m_Size.cy /= sampling;
		ch.m_Size.cx += m_Border.left + m_Border.right + ch.m_bLeft + ch.m_bRight;
		ch.m_Size.cy += m_Border.top + m_Border.bottom + ch.m_bTop + ch.m_bBottom;
		if (m_SizeMax.cx < ch.m_Size.cx)
			m_SizeMax.cx = ch.m_Size.cx;
		if (m_SizeMax.cy < ch.m_Size.cy)
			m_SizeMax.cy = ch.m_Size.cy;
	}

	m_CharSel.SetCurSel(sel);
}


void CMudgeFontDlg::UpdatePositions()
{
	int cx = 0;
	int cy = 0;
	int height = 0;

	int len = m_Chars.GetSize();
	for (int i=0; i<len; i++)
	{
		cCharacter& ch = m_Chars[i];

		CSize size;
		size.cx = ((m_forceMonospacedInX) ? m_SizeMax.cx : ch.m_Size.cx);
		size.cy = ((m_forceMonospacedInY) ? m_SizeMax.cy : ch.m_Size.cy);

		if (size.cy > height)
		{
			height = size.cy;
		}

		if (cx + size.cx > m_TextureWidth)
		{
			cx = 0;
			cy += height + 1;
			// now reset the running height for the next line!
			height = 0;
		}

		ch.m_Pos.x = cx;
		ch.m_Pos.y = cy;

		cx += size.cx + 1;		
	}
}


void CMudgeFontDlg::InvalidateView()
{
	CRect rect;
	m_View.GetWindowRect(&rect);
	::MapWindowPoints(HWND_DESKTOP, m_hWnd, reinterpret_cast<LPPOINT>(&rect), 2);
	InvalidateRect(&rect, FALSE);
}



void CMudgeFontDlg::OnSizeChange() 
{
	UpdateData(TRUE);

	switch (m_Width)
	{
	case 0:	m_TextureWidth = 64;	break;
	case 1:	m_TextureWidth = 128;	break;
	case 2:	m_TextureWidth = 256;	break;
	case 3:	m_TextureWidth = 512;	break;
	}

	switch (m_Height)
	{
	case 0:	m_TextureHeight = 64;	break;
	case 1:	m_TextureHeight = 128;	break;
	case 2:	m_TextureHeight = 256;	break;
	case 3:	m_TextureHeight = 512;	break;
	}

	UpdateTexture();
}


void CMudgeFontDlg::DrawChars()
{
	CBrush brush(RGB(0,0,0));
	CPen   pen(PS_SOLID, 1, RGB(0,0,0));
	m_MemDC.SelectObject(brush);
	m_MemDC.SelectObject(pen);
	m_MemDC.SetTextColor(RGB(255,255,255));
	m_MemDC.SetBkMode(TRANSPARENT);
	m_MemDC.SelectObject(m_DrawFont);

	int index = m_AA.GetCurSel();
	int sampling = m_AA.GetItemData(index);

	ABC abc;
	int len = m_Chars.GetSize();
	for (int i=0; i<len; i++)
	{
		const cCharacter& ch = m_Chars[i];
		CSize size;
		size.cx = ((m_forceMonospacedInX) ? m_SizeMax.cx : ch.m_Size.cx);
		size.cy = ((m_forceMonospacedInY) ? m_SizeMax.cy : ch.m_Size.cy);

		int width  = size.cx;
		int height = size.cy;
		int posx   = (m_Border.left + ch.m_bLeft) * sampling;
		int posy   = (m_Border.top + ch.m_bTop) * sampling;
		int value, x, y;

		m_MemDC.GetCharABCWidths(ch.m_Char, ch.m_Char, &abc);

		// draw character.
		CRect rect(0, 0, width * sampling, height * sampling);
		m_MemDC.Rectangle(rect);
		m_MemDC.TextOut(posx - abc.abcA, posy, ch.m_Char);

		CSize position = ch.m_Pos;
		// if monospaced, center the character horizontally (but not vertically!)
		if (m_forceMonospacedInX)
			position.cx += (m_SizeMax.cx - ch.m_Size.cx) / 2;

		// quite a quick hack this way...
		// I guess we can think of a more generic way...
		switch (sampling)
		{
		case 1:
			for (y=0; y<height; y++)
			{
				for (x=0; x<width; x++)
				{
					COLORREF col = m_MemDib.GetPixel(x, y);
					if (
						((y + position.cy) < m_TextureHeight)
						&&
						((x + position.cx) < m_TextureWidth)
						)
					{
						m_Dib.SetPixel(x + position.cx, y + position.cy, col);
					}
				}
			}
			break;
		case 2:	
			for (y=0; y<height; y++)
			{
				for (x=0; x<width; x++)
				{
					if (y + position.cy < m_TextureHeight && x + position.cx < m_TextureWidth)
					{
						int xb = x * 2;
						int yb = y * 2;
						value = m_MemDib.GetPixel(xb+0, yb+0) & 0xff;
						value += m_MemDib.GetPixel(xb+1, yb+0) & 0xff;
						value += m_MemDib.GetPixel(xb+1, yb+1) & 0xff;
						value += m_MemDib.GetPixel(xb+0, yb+1) & 0xff;
						value /= 4;

						m_Dib.SetPixel(x + position.cx, y + position.cy, RGB(value, value, value));
					}
				}
			}
			break;

		case 3:	
			for (y=0; y<height; y++)
			{
				for (x=0; x<width; x++)
				{
					if (y + position.cy < m_TextureHeight && x + position.cx < m_TextureWidth)
					{
						int xb = x * 3;
						int yb = y * 3;
						value  = (m_MemDib.GetPixel(xb+0, yb+0) & 0xff) * 1;
						value += (m_MemDib.GetPixel(xb+1, yb+0) & 0xff) * 3;
						value += (m_MemDib.GetPixel(xb+2, yb+0) & 0xff) * 1;
						value += (m_MemDib.GetPixel(xb+0, yb+1) & 0xff) * 3;
						value += (m_MemDib.GetPixel(xb+1, yb+1) & 0xff) * 5;
						value += (m_MemDib.GetPixel(xb+2, yb+1) & 0xff) * 3;
						value += (m_MemDib.GetPixel(xb+0, yb+2) & 0xff) * 1;
						value += (m_MemDib.GetPixel(xb+1, yb+2) & 0xff) * 3;
						value += (m_MemDib.GetPixel(xb+2, yb+2) & 0xff) * 1;
						value /= 21;

						m_Dib.SetPixel(x + position.cx, y + position.cy, RGB(value, value, value));
					}
				}
			}
			break;

		case 4:	
			for (y=0; y<height; y++)
			{
				for (x=0; x<width; x++)
				{
					if (y + position.cy < m_TextureHeight && x + position.cx < m_TextureWidth)
					{
						int xb = x * 4;
						int yb = y * 4;
						value  = (m_MemDib.GetPixel(xb+0, yb+0) & 0xff) * 1;
						value += (m_MemDib.GetPixel(xb+1, yb+0) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+2, yb+0) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+3, yb+0) & 0xff) * 1;
						value += (m_MemDib.GetPixel(xb+0, yb+1) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+1, yb+1) & 0xff) * 4;
						value += (m_MemDib.GetPixel(xb+2, yb+1) & 0xff) * 4;
						value += (m_MemDib.GetPixel(xb+3, yb+1) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+0, yb+2) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+1, yb+2) & 0xff) * 4;
						value += (m_MemDib.GetPixel(xb+2, yb+2) & 0xff) * 4;
						value += (m_MemDib.GetPixel(xb+3, yb+2) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+0, yb+3) & 0xff) * 1;
						value += (m_MemDib.GetPixel(xb+1, yb+3) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+2, yb+3) & 0xff) * 2;
						value += (m_MemDib.GetPixel(xb+3, yb+3) & 0xff) * 1;
						value /= 36;

						m_Dib.SetPixel(x + position.cx, y + position.cy, RGB(value, value, value));
					}
				}
			}
			break;
		}

	}
}


void CMudgeFontDlg::AddCharacter(char a_Char)
{
	int len = m_Chars.GetSize();
	for (int i=0; i<len; i++)
	{
		if (m_Chars[i].m_Char == a_Char)
			return;
	}

	cCharacter ch;
	ch.m_Char = a_Char;
	ch.m_bLeft = 0;
	ch.m_bRight = 0;
	ch.m_bTop = 0;
	ch.m_bBottom = 0;

	m_Chars.Add(ch);
}


int CALLBACK CMudgeFontDlg::EnumFontsProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, __DWORD__ dwType, LPARAM lpData)
{
	CComboBox* fonts = (CComboBox*)lpData;
	fonts->AddString(lplf->lfFaceName);
    return 1;
}


void CMudgeFontDlg::OnFontChange() 
{
	UpdateData(TRUE);
	switch (m_forceMonospaced)
		{
		case 0:
			m_forceMonospacedInX = m_forceMonospacedInY = 0;
			break;
		case 1:
			m_forceMonospacedInX = m_forceMonospacedInY = 1;
			break;
		case 2:
			m_forceMonospacedInX = 1;
			m_forceMonospacedInY = 0;
			break;
		case 3:
			m_forceMonospacedInX = 0;
			m_forceMonospacedInY = 1;
			break;
		}

	m_Border.left	= (int)m_blSpin.SendMessage(UDM_GETPOS32);
	m_Border.right	= (int)m_brSpin.SendMessage(UDM_GETPOS32);
	m_Border.top	= (int)m_btSpin.SendMessage(UDM_GETPOS32);
	m_Border.bottom = (int)m_bbSpin.SendMessage(UDM_GETPOS32);

	SetupFont();
	if (m_Running)
	{
		UpdateTexture();
	}
}


void CMudgeFontDlg::OnRectChange() 
{
	UpdateData(TRUE);
	InvalidateView();	
}


void CMudgeFontDlg::OnCharChange() 
{
	int index = m_CharSel.GetCurSel();
	if (index >= 0)
	{
		m_Chars[index].m_bLeft  = m_cblSpin.GetPos();
		m_Chars[index].m_bRight = m_cbrSpin.GetPos();
		m_Chars[index].m_bTop   = m_cbtSpin.GetPos();
		m_Chars[index].m_bBottom= m_cbbSpin.GetPos();

		if (m_Running)
		{
			UpdateTexture();
		}		
	}
}


void CMudgeFontDlg::OnSelchangeChar() 
{
	int index = m_CharSel.GetCurSel();
	if (index >= 0)
	{
		m_cblSpin.SetPos(m_Chars[index].m_bLeft);
		m_cbrSpin.SetPos(m_Chars[index].m_bRight);	
		m_cbtSpin.SetPos(m_Chars[index].m_bTop);
		m_cbbSpin.SetPos(m_Chars[index].m_bBottom);	
	}
}


void CMudgeFontDlg::OnToolsAddchar() 
{
	CAddCharDlg dlg;

	// write in existing characters
	char buffer[1024];
	char *trace = buffer;
	int numChars = m_Chars.GetSize();
	for (int i=0; i<numChars; i++)
		*trace++ = m_Chars[i].m_Char;
	*trace = 0;
	dlg.SetString(buffer);

	if (dlg.DoModal())
	{
		// copy existing sizes into temporary "newArray"
		CCharArray newArray;
		CString chars = dlg.GetString();
		int len = chars.GetLength();
		for (int i=0; i<len; i++)
		{
			char c = chars[i];
			trace = strchr(buffer, c);
			cCharacter *ch, chStorage;
			if (trace != 0)
				ch = &(m_Chars[trace - buffer]);
			else
				{
				ch = &chStorage;
				chStorage.clear();
				chStorage.m_Char = c;
				}
			newArray.Add(*ch);
		}

		// now copy contents of "newArray" back into m_chars
		m_Chars.RemoveAll();
		m_Chars.Copy(newArray);

		UpdateTexture();
	}	
}


void CMudgeFontDlg::OnFileExit() 
{
	EndDialog(0);	
}


void CMudgeFontDlg::getBasename(const char *buffer)
	{
	strcpy(buffer, filename);
	char *trace = strrchr(buffer, '.');
	if (trace != 0)
		*trace = 0;
	}

void CMudgeFontDlg::OnFileExport() 
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	getBasename(szFile);

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Targa image (*.png)\0*.png\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = 0;
	ofn.lpstrDefExt = "tga";
	ofn.Flags = OFN_PATHMUSTEXIST;

	// Display the Open dialog box. 
	if (GetSaveFileName(&ofn) == TRUE)
	{
		ExportTGA(szFile);
		ExportXML(szFile);
	}
}


void CMudgeFontDlg::OnFileNew() 
{
	m_Chars.RemoveAll();
	UpdateTexture();	
}


void CMudgeFontDlg::OnFileOpen() 
{
	OPENFILENAME ofn;       // common dialog box structure
	FILE *file = 0;

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = "MudgeFont file (*.mudge)\0*.mudge\0Old MudgeFont file (*.fnt)\0*.fnt\0All (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrDefExt = "fnt";
	ofn.lpstrInitialDir = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn) == TRUE)
	{
		file = fopen(filename, "rb");
		if (file != 0)
		{
			int		nHeight;
			int		nWeight;
			int		nQuality;
			int		nSampling;
			CString Facename;
			int fileFormat = 0;
			int textureWidth;

			fread(&textureWidth, sizeof(int), 1, file);
			// future direction for mudgefont: store
			// font data in a text file instead of a binary
			// file.  if textureWidth doesn't match any of
			// the five legal values for this version (1.0.3),
			// don't bother reading it, and warn the user.
			switch (textureWidth)
				{
				case 0:
				case 64:
				case 128:
				case 256:
				case 512:
					break;
				default:
					MessageBox("I can't read this MudgeFont data file!  If this really is a valid MudgeFont data file, you need a new version.  Go to the MudgeFont home page and download one!  (For free!)", "Can't Read New Format MudgeFont File", MB_ICONERROR);
					goto EXIT;
				}

			// okay, it's a format we understand.
			m_Chars.RemoveAll();
			if (textureWidth != 0)
				m_TextureWidth = textureWidth;
				{
				fread(&fileFormat, sizeof(int), 1, file);
				fread(&m_TextureWidth, sizeof(int), 1, file);
				}
			fread(&m_TextureHeight, sizeof(int), 1, file);

			switch (m_TextureWidth)
				{
				case 64:	m_Width = 0;	break;
				case 128:	m_Width = 1;	break;
				case 256:	m_Width = 2;	break;
				case 512:	m_Width = 3;	break;
				}
			
			switch (m_TextureHeight)
				{
				case 64:	m_Height = 0;	break;
				case 128:	m_Height = 1;	break;
				case 256:	m_Height = 2;	break;
				case 512:	m_Height = 3;	break;
				}
			UpdateData(FALSE);

			fread(&m_Border.left, sizeof(int), 1, file);
			m_blSpin.SetPos(m_Border.left);
			fread(&m_Border.top, sizeof(int), 1, file);
			m_btSpin.SetPos(m_Border.top);
			fread(&m_Border.right, sizeof(int), 1, file);
			m_brSpin.SetPos(m_Border.right);
			fread(&m_Border.bottom, sizeof(int), 1, file);
			m_bbSpin.SetPos(m_Border.bottom);

			fread(&nHeight, sizeof(int), 1, file);
			fread(&nWeight, sizeof(int), 1, file);
			fread(&nQuality, sizeof(int), 1, file);
			fread(&nSampling, sizeof(int), 1, file);
			fread(&m_Italic, sizeof(BOOL), 1, file);

			int len;
			char buf[256];
			fread(&len, sizeof(int), 1, file);
			fread(buf, len+1, 1, file);
			Facename = buf;

			UpdateUI(nHeight, nWeight, nQuality, nSampling, Facename);
						
			int numChars;
			fread(&numChars, sizeof(int), 1, file);
			for (int i=0; i<numChars; i++)
			{
				char ch;
				int bl, br, bt, bb;

				fread(&ch, sizeof(char), 1, file);
				fread(&bl, sizeof(int), 1, file);
				fread(&br, sizeof(int), 1, file);
				if (fileFormat > 0)
					{
					fread(&bt, sizeof(int), 1, file);
					fread(&bb, sizeof(int), 1, file);
					}
				else
					bt = bb = 0;

				cCharacter kar;
				kar.m_Char = ch;
				kar.m_bLeft = bl;
				kar.m_bRight = br;
				kar.m_bTop = bt;
				kar.m_bBottom = bb;
				m_Chars.Add(kar);
			}
			m_forceMonospaced = 0;
			fread(&m_forceMonospaced, sizeof(m_forceMonospaced), 1, file);

			UpdateData(FALSE);

			OnFontChange();
		}
	}
EXIT:
	if (file != 0)
		fclose(file);
}


void CMudgeFontDlg::OnFileSave() 
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	getBasename(szFile);

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "MudgeFont file (*.mudge)\0*.mudge\0All (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = 0;
	ofn.lpstrDefExt = "fnt";
	ofn.Flags = OFN_PATHMUSTEXIST;

	// Display the Open dialog box. 
	if (GetSaveFileName(&ofn) == TRUE)
	{
		strcpy(filename, szFile);

		int		index;
		int		nHeight;
		int		nWeight;
		int		nQuality;
		int		nSampling;
		CString Facename;

		UpdateData(TRUE);
		m_FontName.GetWindowText(Facename);

		nHeight = m_SizeSpin.GetPos();
		if (nHeight > 100)	nHeight = 100;
		if (nHeight < 5)	nHeight = 5;
		index = m_Weight.GetCurSel();
		nWeight = m_Weight.GetItemData(index);

		index = m_Quality.GetCurSel();
		nQuality = m_Quality.GetItemData(index);

		index = m_AA.GetCurSel();
		nSampling = m_AA.GetItemData(index);

		FILE* file = fopen(szFile, "w+b");
		if (file != 0)
		{
			int i;
			i = 0; fwrite(&i, sizeof(int), 1, file); // alerts MudgeFont that this has a version number
			i = 1; fwrite(&i, sizeof(int), 1, file); // says "version 1"
			fwrite(&m_TextureWidth, sizeof(int), 1, file);
			fwrite(&m_TextureHeight, sizeof(int), 1, file);
			fwrite(&m_Border.left, sizeof(int), 1, file);
			fwrite(&m_Border.top, sizeof(int), 1, file);
			fwrite(&m_Border.right, sizeof(int), 1, file);
			fwrite(&m_Border.bottom, sizeof(int), 1, file);

			fwrite(&nHeight, sizeof(int), 1, file);
			fwrite(&nWeight, sizeof(int), 1, file);
			fwrite(&nQuality, sizeof(int), 1, file);
			fwrite(&nSampling, sizeof(int), 1, file);
			fwrite(&m_Italic, sizeof(BOOL), 1, file);

			int len = Facename.GetLength();
			fwrite(&len, sizeof(int), 1, file);
			fwrite((LPCTSTR)Facename, len+1, 1, file);
						
			int numChars = m_Chars.GetSize();
			fwrite(&numChars, sizeof(int), 1, file);
			for (i=0; i<numChars; i++)
			{
				fwrite(&m_Chars[i].m_Char, sizeof(char), 1, file);
				fwrite(&m_Chars[i].m_bLeft, sizeof(int), 1, file);
				fwrite(&m_Chars[i].m_bRight, sizeof(int), 1, file);
				fwrite(&m_Chars[i].m_bTop, sizeof(int), 1, file);
				fwrite(&m_Chars[i].m_bBottom, sizeof(int), 1, file);
			}
			fwrite(&m_forceMonospaced, sizeof(m_forceMonospaced), 1, file);

			fclose(file);
		}
	}	
	__DWORD__ error = CommDlgExtendedError();
	
}


void CMudgeFontDlg::UpdateUI(__DWORD__ nHeight, __DWORD__ nWeight, __DWORD__ nQuality, __DWORD__ nSampling, const CString& Facename)
{
	int i, len;

	UpdateData(TRUE);
	m_SizeSpin.SetPos(nHeight);

	len = m_Weight.GetCount();
	for (i=0; i<len; i++)
	{
		if (m_Weight.GetItemData(i) == nWeight)
		{
			m_Weight.SetCurSel(i);
			break;
		}
	}

	len = m_Quality.GetCount();
	for (i=0; i<len; i++)
	{
		if (m_Quality.GetItemData(i) == nQuality)
		{
			m_Quality.SetCurSel(i);
			break;
		}
	}

	len = m_AA.GetCount();
	for (i=0; i<len; i++)
	{
		if (m_AA.GetItemData(i) == nSampling)
		{
			m_AA.SetCurSel(i);
			break;
		}
	}

	len = m_FontName.GetCount();
	for (i=0; i<len; i++)
	{
		CString str;
		m_FontName.GetLBText(i, str);
		if (str.CompareNoCase(Facename) == 0)
		{
			m_FontName.SetCurSel(i);
			break;
		}
	}
}


typedef unsigned char uint8;
typedef unsigned short uint16;


#pragma pack(push, 1)
struct tgaHeader
	{
	uint8	Id_Size;
	uint8	ColorMap_Type;
	uint8	Image_Type;
	uint16	ColorMap_Low;
	uint16	ColorMap_Length;
	uint8	ColorMap_Bits;
	uint16	XOrigin;
	uint16	YOrigin;
	uint16	Width;
	uint16	Height;
	uint8	BPP;
	uint8	AttrBits;
	};

#pragma pack(pop)

void CMudgeFontDlg::ExportTGA(const char* a_Filename)
{
	char path[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	_splitpath(a_Filename, drive, dir, fname, 0);
	_makepath(path, drive, dir, fname, ".png");

	FILE* file = fopen(path, "w+b");
	if (file == 0)
		return;

	tgaHeader header;
	stx_memset(&header, 0, sizeof(header));
	// 2 = uncompressed raw image data. (RGBA)
	header.Image_Type	= 2;
	header.Width		= m_TextureWidth;
	header.Height		= m_TextureHeight;
	// 32bits RGBA.
	header.BPP			= 32;
	// 0x20 = first pixel at top-left,
	// bottom 4 bits indicate how many bits of alpha per pixel
	header.AttrBits		= 0x20 | 8;

	fwrite(&header, sizeof(header), 1, file);

	for (int y=0; y<m_TextureHeight; y++)
	{
		for (int x=0; x<m_TextureWidth; x++)
		{
			// get intensity.
			int intensity = m_Dib.GetPixel(x,y) & 0xff;

			// write rgba color;
			unsigned int color;
			if (intensity > 0)
				color = intensity << 24 | 0x00ffffff;
			else
				color = 0;
			fwrite(&color, sizeof(unsigned int), 1, file);
		}
	}

	fclose(file);
}


void CMudgeFontDlg::ExportXML(const char* a_Filename)
{
	char path[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	_splitpath(a_Filename, drive, dir, fname, 0);
	_makepath(path, drive, dir, fname, ".xml");

	FILE* file = fopen(path, "w+t");
	if (file == 0) return;

	fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fprintf(file, "<font>\n");

	int index = m_AA.GetCurSel();
	int sampling = m_AA.GetItemData(index);

	ABC abc;
	m_MemDC.SelectObject(m_DrawFont);
	int numChars = m_Chars.GetSize();
	for (int i=0; i<numChars; i++)
	{
		const cCharacter& ch = m_Chars[i];
		CSize size = (m_forceMonospaced ? m_SizeMax : ch.m_Size);
		fprintf(file, "\t<char id=\"%d\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" />\n",
			ch.m_Char, ch.m_Pos.x, ch.m_Pos.y, size.cx, size.cy);

		m_MemDC.GetCharABCWidths(ch.m_Char, ch.m_Char, &abc);
		abc.abcA /= sampling;
		abc.abcB /= sampling;
		abc.abcC /= sampling;

		abc.abcA -= (m_Border.left + ch.m_bLeft);
		abc.abcB += (m_Border.left + ch.m_bLeft) + (m_Border.right + ch.m_bRight);
		abc.abcC -= (m_Border.right + ch.m_bRight);

		fprintf(file, "\t<spacing id=\"%d\" a=\"%d\" b=\"%d\" c=\"%d\" />\n",
			ch.m_Char, abc.abcA, abc.abcB, abc.abcC);
	}

	fprintf(file, "</font>\n");
	fclose(file);
}


void CMudgeFontDlg::OnMudgefontHomePage() 
{
	ShellExecute(m_hWnd, "open", "http://www.midwinter.com/~lch/programming/mudgefont/", 0, 0, SW_SHOWNORMAL);
}


void CMudgeFontDlg::OnAbout() 
	{
	MessageBox(
		"MudgeFont 1.0.3\n"
		"Copyright (c) 2003 mudGE Entertainment\n"
		"Changes Copyright 2004 by Larry Hastings\n"
		"\n"
		"This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.\n"
		"\n"
		"You can see the full license, or download the MudgeFont source code, at the semi-official home page for MudgeFont:\n"
		"\thttp://www.midwinter.com/~lch/programming/mudgefont/\n"
		, "About MudgeFont", MB_OK);
	}

