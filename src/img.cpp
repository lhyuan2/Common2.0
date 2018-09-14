
#include "StdAfx.h"

#include <img.h>

#define __ColorBlack ((COLORREF)0)

BOOL CImg::StretchBltEx(CDC& dc, const CRect& rcPos, bool bHalfToneMode, LPCRECT prcMargin)
{
	CRect rcDst(rcPos);
	if (NULL != prcMargin)
	{
		dc.FillSolidRect(rcDst, __Color_White);

		rcDst.left = prcMargin->left;
		rcDst.top = prcMargin->top;
		rcDst.right -= prcMargin->right;
		rcDst.bottom -= prcMargin->bottom;
	}

	int iImgWidth = this->GetWidth();
	int iImgHeight = this->GetHeight();
	float fHWRate = (float)iImgHeight / iImgWidth;

	int iDstWidth = rcDst.Width();
	int iDstHeight = rcDst.Height();

	float fNeedHWRate = (float)rcDst.Height() / rcDst.Width();

	CRect rcSrc;
	if (fHWRate > fNeedHWRate)
	{
		rcSrc.left = 0;
		rcSrc.right = iImgWidth;

		rcSrc.top = LONG(iImgHeight - iImgWidth*fNeedHWRate) / 2;
		rcSrc.bottom = iImgHeight - rcSrc.top;
	}
	else
	{
		rcSrc.top = 0;
		rcSrc.bottom = iImgHeight;

		rcSrc.left = LONG(iImgWidth - iImgHeight / fNeedHWRate) / 2;
		rcSrc.right = iImgWidth - rcSrc.left;
	}

	(void)dc.SetStretchBltMode(bHalfToneMode ? STRETCH_HALFTONE : STRETCH_DELETESCANS);

	CDC *pdcThis = CDC::FromHandle(this->GetDC());

	BOOL bRet = dc.StretchBlt(rcDst.left, rcDst.top, iDstWidth, iDstHeight
		, pdcThis, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), SRCCOPY);

	this->ReleaseDC();

	return bRet;
}

BOOL CImg::InitInnerDC(bool bHalfToneMode, UINT cx, UINT cy, LPCRECT prcMargin)
{
	m_cx = cx;
	m_cy = cy;
	m_rcDst = { 0, 0, (int)cx, (int)cy };

	m_bHalfToneMode = bHalfToneMode;

	CDC *pDC = CDC::FromHandle(::GetDC(NULL));
	__AssertReturn(pDC, FALSE);

	__AssertReturn(m_CompDC.CreateCompatibleDC(pDC), FALSE);

	__AssertReturn(m_CompBitmap.CreateCompatibleBitmap(pDC, m_rcDst.Width(), m_rcDst.Height()), FALSE);

	if (NULL != prcMargin)
	{
		m_CompDC.FillSolidRect(m_rcDst, __Color_White);

		m_rcDst.left = prcMargin->left;
		m_rcDst.top = prcMargin->top;
		m_rcDst.right -= prcMargin->right;
		m_rcDst.bottom -= prcMargin->bottom;
	}

	return TRUE;
}

CBitmap* CImg::LoadEx(const wstring& strFile)
{
	Destroy();
	HRESULT hr = Load(strFile.c_str());
	__EnsureReturn(S_OK == hr, NULL);

	CBitmap *pbmpPrev = (CBitmap*)m_CompDC.SelectObject(&m_CompBitmap);

	BOOL bRet = StretchBltEx(m_CompDC, m_rcDst, m_bHalfToneMode);
	if (!bRet)
	{
		return NULL;
	}

	//(void)m_CompDC.SelectObject(pbmpPrev);

	return &m_CompBitmap;
}

BOOL CImg::StretchBltEx(CDC& dc, const CRect& rcPos)
{
	return dc.StretchBlt(rcPos.left, rcPos.top, rcPos.Width(), rcPos.Height(), &m_CompDC, 0, 0, m_cx, m_cy, SRCCOPY);
}

BOOL CImg::StretchBltEx(CImg& img)
{
	return StretchBltEx(img.m_CompDC, CRect(0, 0, m_cx, m_cy));
}

BOOL CImglst::Init(UINT cx, UINT cy)
{
	__AssertReturn(Create(cx, cy, ILC_COLOR32, 0, 0), FALSE);

	m_cx = cx;
	m_cy = cy;

	CDC *pDC = CDC::FromHandle(::GetDC(NULL));
	__AssertReturn(pDC, FALSE);

	__AssertReturn(m_CompDC.CreateCompatibleDC(pDC), FALSE);

	__AssertReturn(m_CompBitmap.CreateCompatibleBitmap(pDC, cx, cy), FALSE);
	
	return TRUE;
}

BOOL CImglst::Init(const CSize& size, const TD_IconVec& vecIcons)
{
	__AssertReturn(Init(size.cx, size.cy), FALSE);

	for (auto hIcon : vecIcons)
	{
		(void)Add(hIcon);
	}

	return TRUE;
}

BOOL CImglst::Init(CBitmap& bitmap)
{
	BITMAP bmp;
	(void)bitmap.GetBitmap(&bmp);
	__AssertReturn(Init(bmp.bmHeight, bmp.bmHeight), FALSE);

	(void)Add(&bitmap, __ColorBlack);

	return TRUE;
}

BOOL CImglst::SetFile(const wstring& strFile, bool bHalfToneMode, LPCRECT prcMargin, int iPosReplace)
{
	CImg img;
	HRESULT hr = img.Load(strFile.c_str());
	__EnsureReturn(S_OK == hr, FALSE);
	{
		return NULL;
	}

	SetImg(img, bHalfToneMode, prcMargin, iPosReplace);
	
	return TRUE;
}

void CImglst::SetImg(CImg& img, bool bHalfToneMode, LPCRECT prcMargin, int iPosReplace)
{
	CBitmap *pbmpPrev = (CBitmap*)m_CompDC.SelectObject(&m_CompBitmap);

	img.StretchBltEx(m_CompDC, CRect(0, 0, m_cx, m_cy), bHalfToneMode, prcMargin);

	(void)m_CompDC.SelectObject(pbmpPrev);

	SetBitmap(m_CompBitmap, iPosReplace);
}

void CImglst::SetBitmap(CBitmap& bitmap, int iPosReplace)
{
	if (iPosReplace >= 0)
	{
		(void)__super::Replace(iPosReplace, &bitmap, NULL);
	}
	else
	{
		(void)Add(&bitmap, __Color_Black);
	}
}

void CImglst::SetIcon(HICON hIcon, int iPosReplace)
{
	if (iPosReplace >= 0)
	{
		(void)Replace(iPosReplace, hIcon);
	}
	else
	{
		(void)Add(hIcon);
	}
}
