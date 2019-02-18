#pragma once

#include <gdiplus.h>
using namespace Gdiplus;

// 图片预览控件

class CPicturePreviewCtrl : public CStatic
{
	DECLARE_DYNAMIC(CPicturePreviewCtrl)

public:
	CPicturePreviewCtrl();
	virtual ~CPicturePreviewCtrl();

	// 设置窗口背景色
	void SetBkColor(COLORREF clrBk);

	// 设置启用双缓冲
	void EnableDoubleBuffer(BOOL bEnabled) { m_bDoubleBuffer = bEnabled; }
	BOOL IsDoubleBufferEnabled() { return m_bDoubleBuffer; }

	// 指定图片
	void SetImage(LPCTSTR lpszFileName);

	// 居中显示：bFit-适应窗口
	void CenterImage(BOOL bFit = FALSE);

	// 居中缩放
	void Zoom(double dScale);

	// 适应窗口：0-宽/1-高/2-自动
	void ZoomFit(int nType);

	// 打印图片：bFit-缩放图片以适应纸张，bUseDefaultPrinter-使用默认打印机，否则提示选择打印机
	BOOL PrintImage(BOOL bFit = TRUE, BOOL bUseDefaultPrinter = TRUE);

protected:

	virtual void PreSubclassWindow();

	void Zoom(CPoint pt, short zDelta);

	void Move(int x, int y);

protected:

	Gdiplus::REAL m_dScale;
	Gdiplus::PointF m_ptBase;
	Gdiplus::Image* m_pImage;
	Gdiplus::Color m_clrBk;
	BOOL m_bDoubleBuffer;
	POINT m_ptCursor;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


