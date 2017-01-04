////////////////////////////////////////////////////////////////////////
//
//	RemoteArrow.h
//
//	Definice / deklarace pro RemoteArrow
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

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

//
//	class CRemoteArrowApp
//
//	trida aplikace
//

class CRemoteArrowApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

//
//	class CMainWnd
//
//	tool window
//

class CMainWnd : public CWnd
{
public:
	CMainWnd();

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

protected:
	afx_msg void OnPaint();
	afx_msg LRESULT OnMouseLeave(WPARAM wPar, LPARAM lPar);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	BOOL m_bShowing;				// rezim ukazovani ON/OFF
	CRect m_Rect;					// zobrazovaci obdelnik
	CMenu m_Menu;					// pop-up menu
	BOOL m_bMoving;					// pohyb oknem

									//	staticke prvky
protected:
	static LPCTSTR s_lpRegClass;	// registered class name

	DECLARE_MESSAGE_MAP()
};

