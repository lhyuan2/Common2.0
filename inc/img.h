
#pragma once

#include <def.h>

#include <atlimage.h>

using TD_IconVec = vector<HICON>;

class __CommonPrjExt CImg : public CImage
{
public:
	CImg()
	{
	}

private:
	bool m_bHalfToneMode = false;

	UINT m_cx = 0;
	UINT m_cy = 0;
	CRect m_rcDst;

	CDC m_CompDC;
	CBitmap m_CompBitmap;
	
public:
	BOOL StretchBltEx(CDC& dc, const CRect& rcPos, bool bHalfToneMode, LPCRECT prcMargin=NULL);

	BOOL InitInnerDC(bool bHalfToneMode, UINT cx, UINT cy, LPCRECT prcMargin=NULL);
	CBitmap* LoadEx(const wstring& strFile);
	BOOL StretchBltEx(CDC& dc, const CRect& rcPos);

	BOOL StretchBltEx(CImg& img);
};

enum class E_ImglstType
{
	ILT_Both = -1
	, ILT_Normal = 0
	, ILT_Small = LVSIL_SMALL
};

class __CommonPrjExt CImglst : public CImageList
{
public:
	CImglst()
	{
	}

private:
	UINT m_cx = 0;
	UINT m_cy = 0;

	CDC m_CompDC;
	CBitmap m_CompBitmap;

public:
	BOOL Init(UINT cx, UINT cy);

	BOOL Init(const CSize& size, const TD_IconVec& vecIcons = {});

	BOOL Init(CBitmap& bitmap);

	BOOL SetFile(const wstring& strFile, bool bHalfToneMode, LPCRECT prcMargin = NULL, int iPosReplace = -1);

	void SetImg(CImg& img, bool bHalfToneMode, LPCRECT prcMargin, int iPosReplace);
	
	void SetBitmap(CBitmap& bitmap, int iPosReplace = -1);

	void SetIcon(HICON hIcon, int iPosReplace = -1);

	void SetToListCtrl(CListCtrl &wndListCtrl, E_ImglstType eImglstType)
	{
		if (E_ImglstType::ILT_Both == eImglstType)
		{
			(void)wndListCtrl.SetImageList(this, (int)E_ImglstType::ILT_Normal);
			(void)wndListCtrl.SetImageList(this, (int)E_ImglstType::ILT_Small);
		}
		else
		{
			(void)wndListCtrl.SetImageList(this, (int)eImglstType);
		}
	}

	void SetToTreeCtrl(CTreeCtrl &wndTreeCtrl)
	{
		(void)wndTreeCtrl.SetImageList(this, TVSIL_NORMAL);
	}
};
