#pragma once

#include <gdiplus.h>
using namespace Gdiplus;

// ͼƬԤ���ؼ�

class CPicturePreviewCtrl : public CStatic
{
	DECLARE_DYNAMIC(CPicturePreviewCtrl)

public:
	CPicturePreviewCtrl();
	virtual ~CPicturePreviewCtrl();

	// ���ô��ڱ���ɫ
	void SetBkColor(COLORREF clrBk);

	// ��������˫����
	void EnableDoubleBuffer(BOOL bEnabled) { m_bDoubleBuffer = bEnabled; }
	BOOL IsDoubleBufferEnabled() { return m_bDoubleBuffer; }

	// ָ��ͼƬ
	void SetImage(LPCTSTR lpszFileName);

	// ������ʾ��bFit-��Ӧ����
	void CenterImage(BOOL bFit = FALSE);

	// ��������
	void Zoom(double dScale);

	// ��Ӧ���ڣ�0-��/1-��/2-�Զ�
	void ZoomFit(int nType);

	// ��ӡͼƬ��bFit-����ͼƬ����Ӧֽ�ţ�bUseDefaultPrinter-ʹ��Ĭ�ϴ�ӡ����������ʾѡ���ӡ��
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


