#pragma once
#include "ColorStaticCtrl.h"

// CData dialog

class CData : public CDialog
{
	DECLARE_DYNAMIC(CData)

public:
	CData(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CData();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	ColorStaticCtrl m_staticDrawSlow;
	ColorStaticCtrl m_staticDrawNormal;
	ColorStaticCtrl m_staticDrawFast;
	ColorStaticCtrl m_staticDrawHome;
	CSliderCtrl m_sliderUpDown;
	CSliderCtrl m_sliderRightLeft;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
