// CData.cpp : implementation file
//

#include "stdafx.h"
//#include "pch.h"
#include "CData.h"
#include "afxdialogex.h"


// CData dialog

IMPLEMENT_DYNAMIC(CData, CDialog)

CData::CData(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG1, pParent)
{

}

CData::~CData()
{
}

void CData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SLOW, m_staticDrawSlow);
	DDX_Control(pDX, IDC_STATIC_NORMAL, m_staticDrawNormal);
	DDX_Control(pDX, IDC_STATIC_FAST, m_staticDrawFast);
	DDX_Control(pDX, IDC_STATIC_HOME, m_staticDrawHome);
	DDX_Control(pDX, IDC_SLIDER1, m_sliderUpDown);
	DDX_Control(pDX, IDC_SLIDER2, m_sliderRightLeft);
}


BEGIN_MESSAGE_MAP(CData, CDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CData message handlers


void CData::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	/*
	//slow = yellow = 255 255 0
	//normal = orange = 255 131 0
	//fast = green = 159 218 64
	//white = 255 255 255
	//indicatror = light blue = 52 204 255
	if(str1== str2) {
		m_staticDrawHome.red = 52;
		m_staticDrawHome.green = 218;
		m_staticDrawHome.green = 64;
	}
	else {
		m_staticDrawHome.red = point.x % 256;
		m_staticDrawHome.green = point.y % 256;
	}



	*/

	m_staticDrawSlow.red = point.x % 256;
	m_staticDrawSlow.green = point.y % 256;

	m_staticDrawNormal.red = point.x % 256;
	m_staticDrawNormal.green = point.y % 256;

	m_staticDrawFast.red = point.x % 256;
	m_staticDrawFast.green = point.y % 256;

	m_staticDrawHome.reDraw();
	m_staticDrawSlow.reDraw();
	m_staticDrawNormal.reDraw();
	m_staticDrawFast.reDraw();

	CDialog::OnMouseMove(nFlags, point);
}


HBRUSH CData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	return (HBRUSH)GetStockObject(LTGRAY_BRUSH);;
}
