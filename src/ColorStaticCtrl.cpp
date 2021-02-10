// ColorStaticCtrl.cpp : implementation file
//

//#include "pch.h"
#include "stdafx.h"
#include "ColorStaticCtrl.h"


// ColorStaticCtrl

IMPLEMENT_DYNAMIC(ColorStaticCtrl, CStatic)

ColorStaticCtrl::ColorStaticCtrl()
{

}

ColorStaticCtrl::~ColorStaticCtrl()
{
}


BEGIN_MESSAGE_MAP(ColorStaticCtrl, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// ColorStaticCtrl message handlers




void ColorStaticCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CStatic::OnPaint() for painting messages

	CRect rectWindow;
	GetClientRect(&rectWindow);

	// this function keeps things inside a particular static window
	CRgn ClipRgn;
	if (ClipRgn.CreateRectRgnIndirect(&rectWindow))
		dc.SelectClipRgn(&ClipRgn);

	auto color{ RGB(red, green, blue) };

	CBrush brush(color);

	dc.FillRect(&rectWindow, &brush);

	dc.SelectClipRgn(NULL);
}
