////////////////////////////////////////////////////////////////////////
//
//	RemoteArrow.cpp
//
//	Zobrazeni vzdalene sipky
//
//	160729 jl
//	170104 jl
//
////////////////////////////////////////////////////////////////////////
//
//	Historie
//	--------
//	160729 -jl- zacatek (prevzato z CCfocus)
//	170104 -jl- rozvijeni funkcnosti RemoteArrow
//
////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "RemoteArrow.h"

CRemoteArrowApp App;

////////////////////////////////////////////////////////////////////////
//	konstanty, makra

#define	IDM_MOVE	1001		// menu -> move
#define	IDM_EXIT	1002		//		-> exit

////////////////////////////////////////////////////////////////////////
//	trida CRemoteArrowApp

//
//	BOOL CRemoteArrowApp::InitInstance()
//

BOOL CRemoteArrowApp::InitInstance()
{
	m_pMainWnd = new CMainWnd;
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//	trida CMainWnd

//	mapa zprav pro hlavni okno

BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

LPCTSTR CMainWnd::s_lpRegClass = NULL;

//
//	CMainWnd::CMainWnd()
//
//	Konstruktor
//

CMainWnd::CMainWnd()
{
	if (NULL == s_lpRegClass)	// okenni trida zatim neni registrovana
	{
		s_lpRegClass = AfxRegisterWndClass
		(CS_VREDRAW | CS_HREDRAW,
			::LoadCursor(NULL, IDC_ARROW),
			//			(HBRUSH)::GetStockObject(NULL_BRUSH),
			(HBRUSH)0,
			//::LoadIcon(NULL, IDR_MAINFRAME)
			//::LoadIcon(NULL, IDR_REMOTEARROW)
			::LoadIcon(NULL, MAKEINTRESOURCE(128))
		);
	}

//	DWORD dwStyle = WS_POPUP | WS_BORDER;
	DWORD dwStyle = WS_POPUP;
	DWORD dwStyleEx = WS_EX_TOPMOST | WS_EX_LAYERED;

	RECT rcs = { 0,0,0,0 };	// pracovni oblast obrazovky
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcs, 0);

	int dx = 16;	// rozmery nastroje
	int dy = 16;

	int xpos = rcs.right - dx;	// poloha nastroje
	int ypos = rcs.bottom - dy;

	CRect rc(CPoint(xpos, ypos), CSize(dx, dy));

	CreateEx(dwStyleEx, s_lpRegClass, _T("Main Window"), dwStyle, rc, this, 0);

	//	m_pShow = new CShowWnd;		// zobrazovaci okno - zatim skryte

//	SetLayeredWindowAttributes(RGB(255, 255, 255), 255, LWA_COLORKEY|LWA_ALPHA);
	SetLayeredWindowAttributes(RGB(0,0,0), 0, LWA_COLORKEY);

	m_bShowing = FALSE;			// ukazovani - OFF

	m_bMoving = FALSE;			// pohybovani neni

								//	jeste vytvor pop-up menu

	m_Menu.CreatePopupMenu();
	m_Menu.AppendMenu(MF_STRING, IDM_MOVE, _T("&Move"));
	m_Menu.AppendMenu(MF_SEPARATOR);
	m_Menu.AppendMenu(MF_STRING, IDM_EXIT, _T("E&xit"));
}

//
//	void CMainWnd::OnPaint()
//
//	vykresleni okna
//

void CMainWnd::OnPaint()
{
	COLORREF bgCol = RGB(255, 128, 128);
	COLORREF penCol = RGB(255, 0, 0);
	CBrush bgBr;
	bgBr.CreateSolidBrush(bgCol);
	CPen pen;
	pen.CreatePen(PS_SOLID, 0, penCol);

	CPaintDC dc(this);

	CRect rc;
	GetClientRect(&rc);

	POINT pts[5];

	//pts[0] = rc.TopLeft();
	//pts[1].x = rc.left;
	//pts[1].y = rc.bottom;
	//pts[2].x = rc.CenterPoint().x - rc.Width() / 4;
	//pts[2].y = rc.CenterPoint().y - rc.Height() / 4;
	//pts[3].x = rc.right;
	//pts[3].y = rc.top;
	//pts[4] = rc.TopLeft();

	pts[0] = rc.TopLeft();
	pts[1].x = rc.left + rc.Width() / 4;
	pts[1].y = rc.bottom;
	pts[2].x = rc.CenterPoint().x;
	pts[2].y = rc.CenterPoint().y;
	pts[3].x = rc.right;
	pts[3].y = rc.top + rc.Height() / 4;
	pts[4] = rc.TopLeft();

	CBrush* oldBrush = dc.SelectObject(&bgBr);
	CPen* oldPen = dc.SelectObject(&pen);

	dc.Polygon(pts, sizeof(pts) / sizeof(pts[0]));

	dc.SelectObject(oldPen);
	dc.SelectObject(oldBrush);

//	dc.FillSolidRect(&rc, bgCol);
}

//
//	LRESULT CMainWnd::OnMouseLeave(WPARAM wPar, LPARAM lPar)
//
//	Udalost - mys opousti tool window
//

LRESULT CMainWnd::OnMouseLeave(WPARAM wPar, LPARAM lPar)
{
	m_bShowing = FALSE;

	return 0;
}

//
//	void CMainWnd::OnMouseMove(UINT nFlags, CPoint point)
//
//	Akce - kdyz je kurzor nad
//

void CMainWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bMoving)	// pohyb oknem
	{
		CRect rc;
		GetWindowRect(&rc);
		ClientToScreen(&point);
		rc.MoveToXY(point);
		MoveWindow(&rc);
		return;
	}

	if (!m_bShowing)
	{
		GUITHREADINFO gti = { sizeof(gti), };
		if (::GetGUIThreadInfo(NULL, &gti))
		{
			//if (NULL != gti.hwndFocus)
			//{	// aktualne fokusovane okno
			//	CWnd* pFocusedWnd = CWnd::FromHandle(gti.hwndFocus);
			//	CRect rcfoc;
			//	pFocusedWnd->GetWindowRect(rcfoc);
			//	ShowScreenRect(&rcfoc);

			//	CString tx;
			//	tx.Format(_T("Focused(0x%08x)(%d,%d,%d,%d)\n"),
			//		pFocusedWnd->m_hWnd, rcfoc.left, rcfoc.top, rcfoc.right, rcfoc.bottom);
			//	OutputDebugString(tx);
			//}
		}

		TRACKMOUSEEVENT tm = { sizeof(tm),TME_LEAVE,m_hWnd,0, };
		::TrackMouseEvent(&tm);

		m_bShowing = TRUE;
	}

	CWnd::OnMouseMove(nFlags, point);
}

//
//	void CMainWnd::OnRButtonUp(UINT nFlags, CPoint point)
//
//	Prave tlacitko - kontextove menu
//

void CMainWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	BOOL bRet = m_Menu.TrackPopupMenu(TPM_RIGHTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		point.x, point.y,
		this);
}

//
//	BOOL CMainWnd::OnCommand(WPARAM wParam, LPARAM lParam)
//
//	Interakce z kontextoveho menu
//

BOOL CMainWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDM_MOVE:
		return TRUE;

	case IDM_EXIT:
		DestroyWindow();
		PostQuitMessage(0);
		return TRUE;
	}

	return CWnd::OnCommand(wParam, lParam);
}

//
//	void CMainWnd::OnLButtonDown(UINT nFlags, CPoint point)
//
//	Leve tlacitko dolu - zahajeni pohybu okna
//

void CMainWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMoving = TRUE;
	SetCapture();
}

//
//	void CMainWnd::OnLButtonUp(UINT nFlags, CPoint point)
//
//	Leve tlacitko nahoru - ukonceni pohybu okna
//

void CMainWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bMoving)
	{
		m_bMoving = FALSE;
		ReleaseCapture();
	}
}
