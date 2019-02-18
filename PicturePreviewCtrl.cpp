#include "stdafx.h"
#include "PicturePreviewCtrl.h"


IMPLEMENT_DYNAMIC(CPicturePreviewCtrl, CStatic)
CPicturePreviewCtrl::CPicturePreviewCtrl()
{
	m_dScale = 1.0;
	m_ptBase.X = 0.0f;
	m_ptBase.Y = 0.0f;
	m_pImage = NULL;
	m_clrBk = Gdiplus::Color::White;
	m_bDoubleBuffer = TRUE;
}

CPicturePreviewCtrl::~CPicturePreviewCtrl()
{
	delete m_pImage;
	m_pImage = NULL;
}


BEGIN_MESSAGE_MAP(CPicturePreviewCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CPicturePreviewCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);
	CStatic::PreSubclassWindow();
}

void CPicturePreviewCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);

	if (m_bDoubleBuffer)
	{
		CDC dcTemp;
		dcTemp.CreateCompatibleDC(&dc);

		CBitmap bitmapTemp;
		bitmapTemp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());

		CBitmap *pOldBit = dcTemp.SelectObject(&bitmapTemp);

		// 绘图
		Graphics graphics(dcTemp.m_hDC);
		graphics.ScaleTransform(m_dScale, m_dScale);
		graphics.Clear(m_clrBk);
		if (m_pImage)
		{
			graphics.DrawImage(m_pImage, m_ptBase.X / m_dScale, m_ptBase.Y / m_dScale, m_pImage->GetWidth(), m_pImage->GetHeight());
		}
		graphics.ResetTransform();

		// 显示   
		dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcTemp, 0, 0, SRCCOPY);

		// 清理     
		dcTemp.SelectObject(pOldBit);

		::ReleaseDC(this->m_hWnd, dcTemp);

		bitmapTemp.DeleteObject();
	}
	else
	{
		Graphics graphics(dc.m_hDC);
		graphics.ScaleTransform(m_dScale, m_dScale);
		graphics.Clear(m_clrBk);
		if (m_pImage)
		{
			graphics.DrawImage(m_pImage, m_ptBase.X / m_dScale, m_ptBase.Y / m_dScale, m_pImage->GetWidth(), m_pImage->GetHeight());
		}
		graphics.ResetTransform();
	}
}

BOOL CPicturePreviewCtrl::PrintImage(BOOL bFit, BOOL bUseDefaultPrinter)
{
	if (m_pImage == NULL)
	{
		return FALSE;
	}

	double dWidth = m_pImage->GetWidth(), dHeight = m_pImage->GetHeight();
	if (dWidth < 0.01 || dHeight < 0.01)
	{
		return FALSE;
	}

	// 图片横向
	BOOL bImageHorizon = dWidth > dHeight;

	CPrintDialog printDlg(FALSE);
	if (bUseDefaultPrinter)
	{
		if (!printDlg.GetDefaults())
		{
			return FALSE;
		}
	}
	else
	{
		if (!printDlg.DoModal())
		{
			return FALSE;
		}
	}

	HDC printDC = printDlg.GetPrinterDC();
	if (printDC == NULL)
	{
		return FALSE;
	}

	CDC dc;
	if (!dc.Attach(printDC))
	{
		DeleteDC(printDC);
		return FALSE;
	}

	// 分辨率
	int xDpi = dc.GetDeviceCaps(LOGPIXELSX);
	int yDpi = dc.GetDeviceCaps(LOGPIXELSY);
	// 页面大小
	double dPageWidth = dc.GetDeviceCaps(HORZRES);
	double dPageHeight = dc.GetDeviceCaps(VERTRES);

	// 纸张横向
	BOOL bPageHorizon = dPageWidth > dPageHeight;

	// 需要旋转90度
	BOOL bRotate = (bImageHorizon && !bPageHorizon) || (!bImageHorizon && bPageHorizon);

	// 不可打印边距
	double dMarginX = dc.GetDeviceCaps(PHYSICALOFFSETX) / dc.GetDeviceCaps(PHYSICALWIDTH) * dPageWidth;
	double dMarginY = dc.GetDeviceCaps(PHYSICALOFFSETY) / dc.GetDeviceCaps(PHYSICALHEIGHT) * dPageHeight;
	// 缩放比例
	double dScale = 1.0;
	if (bFit)
	{
		// 打印范围
		double dPrintWidth = dPageWidth - dMarginX * 2.0;
		double dPrintHeight = dPageHeight - dMarginY * 2.0;
		double dScaleX = 1.0, dScaleY = 1.0;
		if (bRotate)
		{
			dScaleX = dPrintWidth / dHeight;
			dScaleY = dPrintHeight / dWidth;
		}
		else
		{
			dScaleX = dPrintWidth / dWidth;
			dScaleY = dPrintHeight / dHeight;
			dMarginX += (dPrintWidth - dWidth) / 2.0;
			dMarginY += (dPrintHeight - dHeight) / 2.0;
		}
		dScale = min(dScaleX, dScaleY);
	}
	
	// 居中打印，重新计算边距
	if (bRotate)
	{
		dMarginX = (dPageHeight - dWidth * dScale) / 2.0;
		dMarginY = (dPageWidth - dHeight * dScale) / 2.0;
	}
	else
	{
		dMarginX = (dPageWidth - dWidth * dScale) / 2.0;
		dMarginY = (dPageHeight - dHeight * dScale) / 2.0;
	}

	DOCINFO docInfo;
	ZeroMemory(&docInfo, sizeof(docInfo));
	docInfo.cbSize = sizeof(docInfo);
	docInfo.lpszDocName = _T("打印图片");

	dc.m_bPrinting = TRUE;

	dc.StartDoc(&docInfo);

	dc.StartPage();

	// 打印图形
	Graphics graphics(printDC);
	graphics.SetPageUnit(UnitPixel);
	if (bRotate) // 绕中心旋转90度
	{
		graphics.TranslateTransform(dPageWidth / 2.0, dPageHeight / 2.0);
		graphics.RotateTransform((REAL)90.0);
		graphics.TranslateTransform(-dPageHeight / 2.0, -dPageWidth / 2.0);
	}
	graphics.ScaleTransform(dScale, dScale);
	graphics.DrawImage(m_pImage, (REAL)dMarginX / dScale, (REAL)dMarginY / dScale, (REAL)dWidth, (REAL)dHeight);
	graphics.ResetTransform();
	
	dc.EndPage();

	dc.EndDoc();

	dc.Detach();

	DeleteDC(printDC);

	return TRUE;
}

void CPicturePreviewCtrl::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	Invalidate();
}

void CPicturePreviewCtrl::SetBkColor(COLORREF clrBk)
{
	m_clrBk.SetFromCOLORREF(clrBk);
}

void CPicturePreviewCtrl::SetImage(LPCTSTR lpszFileName)
{
	m_dScale = 1.0;
	m_ptBase.X = 0.0f;
	m_ptBase.Y = 0.0f;
	if (m_pImage != NULL)
	{
		delete m_pImage;
	}
	m_pImage = NULL;
	if (lpszFileName != NULL && ::GetFileAttributes(lpszFileName) != INVALID_FILE_ATTRIBUTES)
	{
#ifdef _UNICODE
		m_pImage = Gdiplus::Image::FromFile(lpszFileName);
#else
		LPWSTR lpszFileName2 = CUnicode::MultiByteToWideChar(lpszFileName);
		m_pImage = Gdiplus::Image::FromFile(lpszFileName2);
		delete[] lpszFileName2;
#endif
	}

	CenterImage();
}

void CPicturePreviewCtrl::CenterImage(BOOL bFit)
{
	// 居中
	if (m_pImage != NULL)
	{
		double dWidth = m_pImage->GetWidth(), dHeight = m_pImage->GetHeight();
		if (dWidth > 0.0 && dHeight > 0.0)
		{
			CRect rect;
			GetClientRect(&rect);
			if (bFit)
			{
				double dScaleX = rect.Width() / dWidth, dScaleY = rect.Height() / dHeight;
				m_dScale = min(dScaleX, dScaleY);
			}
			else
			{
				m_dScale = 1.0;
			}
			m_ptBase.X = (rect.Width() - dWidth * m_dScale) / 2.0;
			m_ptBase.Y = (rect.Height() - dHeight * m_dScale) / 2.0;
		}
	}

	Invalidate(TRUE);
}

void CPicturePreviewCtrl::Zoom(double dScale)
{
	CRect rect;
	GetClientRect(&rect);	

	double x = rect.Width() / 2.0, y = rect.Height() / 2.0;

	m_ptBase.X = x - (x - m_ptBase.X) * dScale;
	m_ptBase.Y = y - (y - m_ptBase.Y) * dScale;
	m_dScale *= dScale;

	Invalidate();
}

void CPicturePreviewCtrl::ZoomFit(int nType)
{
	// 居中
	if (m_pImage != NULL)
	{
		double dWidth = m_pImage->GetWidth(), dHeight = m_pImage->GetHeight();
		if (dWidth > 0.0 && dHeight > 0.0)
		{
			CRect rect;
			GetClientRect(&rect);
			double dScaleX = rect.Width() / dWidth, dScaleY = rect.Height() / dHeight;
			if (nType == 0)
			{
				m_dScale = dScaleX;
			}
			else if (nType == 1)
			{
				m_dScale = dScaleY;
			}
			else
			{
				m_dScale = min(dScaleX, dScaleY);
			}
			m_ptBase.X = (rect.Width() - dWidth * m_dScale) / 2.0;
			m_ptBase.Y = (rect.Height() - dHeight * m_dScale) / 2.0;
		}
	}

	Invalidate(TRUE);
}

//移动
void CPicturePreviewCtrl::Move(int x, int y)
{
	m_ptBase.X += x;
	m_ptBase.Y += y;

	Invalidate(TRUE);
}

//缩放
void CPicturePreviewCtrl::Zoom(CPoint pt, short zDelta)
{
	ScreenToClient(&pt);

  	Gdiplus::REAL dScale = zDelta > 0 ? 1.1f : 1.0f/1.1f;
	m_ptBase.X = pt.x - (pt.x - m_ptBase.X) * dScale;
	m_ptBase.Y = pt.y - (pt.y - m_ptBase.Y) * dScale;
	m_dScale *= dScale;

	if (fabs(m_dScale - 1.0) < 0.01) // 接近1置1
	{
		m_dScale = 1.0;
	}
	else if (m_dScale < 0.0001)
	{
		m_dScale = 0.0001;
	}

	Invalidate();
}

void CPicturePreviewCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (::GetCursorPos(&m_ptCursor))
	{
		SetCapture();
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CPicturePreviewCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	POINT ptCursor;
	if (::GetCursorPos(&ptCursor))
	{
		int x = ptCursor.x - m_ptCursor.x;
		int y = ptCursor.y - m_ptCursor.y;
		if (abs(x) > 1 || abs(y) > 1)
		{
			Move(x, y);
		}

		Invalidate();
	}

	ReleaseCapture();

	CStatic::OnLButtonUp(nFlags, point);
}

BOOL CPicturePreviewCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	this->Zoom(pt, zDelta);
	Invalidate();
	return CStatic::OnMouseWheel(nFlags,zDelta,pt);
}

void CPicturePreviewCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	SetFocus();

	CStatic::OnMouseMove(nFlags, point);
}