#pragma once

class __CommonPrjExt CFontGuide
{
public:
	CFontGuide() {};

private:
	CFont m_font;

public:
	bool setFontSize(CWnd& wnd, UINT uFontSize)
	{
		if (0 == uFontSize)
		{
			return true;
		}

		if (NULL==m_font.GetSafeHandle())
		{
			CFont *pFont = wnd.GetFont();
			if (NULL == pFont)
			{
				return false;
			}
			
			LOGFONT logFont;
			::ZeroMemory(&logFont, sizeof(logFont));
			(void)pFont->GetLogFont(&logFont);
			
			logFont.lfHeight = (LONG)uFontSize;

			if (!m_font.CreateFontIndirect(&logFont))
			{
				return false;
			}
		}

		wnd.SetFont(&m_font);

		return true;
	}
};

