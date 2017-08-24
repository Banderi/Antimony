#include "Warnings.h"
#include "FontRenderer.h"

#pragma comment (lib, "..\\ext\\FW1FontWrapper\\lib\\x86\\FW1FontWrapper.lib")

///

HRESULT FontWrapper::build(wchar_t *fname)
{
	m_fname = fname;
	return S_OK;
}
HRESULT FontWrapper::buildFW1(wchar_t *fname, IFW1Factory *fw1f)
{
	m_isfw1 = true;
	m_fname = fname;
	HRESULT hr;
	hr = fw1f->CreateFontWrapper(dev, fname, &m_FW1Wrapper);
	if (hr != S_OK)
		return hr;
	hr = m_FW1Wrapper->GetDWriteFactory(&m_FW1WriteFactory);
	if (hr != S_OK)
		return hr;
	hr = m_FW1WriteFactory->CreateTextFormat(
		fname,
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		16.0f,
		L"",
		&m_FW1TextFormat
	);
	if (hr != S_OK)
		return hr;
	return fw1f->CreateTextGeometry(&m_FW1TextGeometry);

	return hr;
}
float FontWrapper::getWidth(const wchar_t *str, float s, RECT r, UINT f)
{
	if (m_isfw1)
	{
		/*FW1_RECTF fw1r;
		fw1r.Top = r.top;
		fw1r.Bottom = r.bottom;
		fw1r.Left = r.left;
		fw1r.Right = r.right;*/
		FW1_RECTF rect = { r.left, r.top, r.right, r.bottom };
		return m_FW1Wrapper->MeasureString(str, m_fname, s, &rect, f | FW1_RESTORESTATE).Right;
	}
	else
	{
		// TODO: Sprite rendering
	}
	return 0.0f;
}
void FontWrapper::render(const wchar_t *str, float s, float x, float y, UINT32 c, UINT flags, ID3D11DeviceContext *con)
{
	if (m_isfw1)
	{
		RenderedText temp;
		temp.color = c;
		temp.fname = this->m_fname;
		temp.size = s;
		temp.text = str;
		temp.x = x;
		temp.y = y;
		temp.FW1Wrapper = m_FW1Wrapper;

		RenderedText *lastobj = nullptr;

		if (m_FW1LineIndex < m_FW1RenderedText.size())			// check string at index and see if it's the same as last time
		{
			lastobj = &m_FW1RenderedText.at(m_FW1LineIndex);

			if (lastobj->text == temp.text)			// the text matches!
			{
				temp.layout = lastobj->layout;
				temp.update(FCACHE_UPDATE);
			}
			else									// no text match, create new layout
			{
				m_FW1WriteFactory->CreateTextLayout(
					temp.text.c_str(),
					static_cast<UINT32>(temp.text.size()),
					m_FW1TextFormat,
					0.0f,
					0.0f,
					&temp.layout
				);
				temp.layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
				if (flags & FW1_CENTER)
				{
					temp.layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
					temp.layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				}
				DWRITE_TEXT_RANGE allText = { 0, static_cast<UINT32>(wcslen(str)) };
				temp.layout->SetFontSize(temp.size, allText);
				temp.layout->SetFontFamilyName(temp.fname.c_str(), allText);

				lastobj->layout->Release();
				*lastobj = temp;
			}

			if (1)		// analyze the selected layout and draw it
			{
				m_FW1Wrapper->AnalyzeTextLayout(NULL, temp.layout, temp.x, temp.y, temp.color, FW1_NOFLUSH | flags, m_FW1TextGeometry);
			}
			else if (0)
			{
				FW1_RECTF rect = { temp.x, temp.y, temp.x, temp.y };
				m_FW1Wrapper->AnalyzeString(NULL, temp.text.c_str(), temp.fname.c_str(), temp.size, &rect, temp.color, FW1_CENTER | FW1_VCENTER | FW1_NOWORDWRAP | FW1_NOFLUSH | flags, m_FW1TextGeometry);
			}
			else if (0)
			{
				m_FW1Wrapper->DrawTextLayout(con, temp.layout, temp.x, temp.y, temp.color, FW1_IMMEDIATECALL | flags);
			}
			else
			{
				m_FW1Wrapper->DrawString(con, str, s, x, y, c, FW1_RESTORESTATE | flags);
			}
		}
		else			// new object entirely past the index
		{
			m_FW1WriteFactory->CreateTextLayout(
				temp.text.c_str(),
				static_cast<UINT32>(temp.text.size()),
				m_FW1TextFormat,
				0.0f,
				0.0f,
				&temp.layout
			);
			temp.layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			if (flags & FW1_CENTER)
			{
				temp.layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				temp.layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			}
			DWRITE_TEXT_RANGE allText = { 0, static_cast<UINT32>(wcslen(str)) };
			temp.layout->SetFontSize(temp.size, allText);
			temp.layout->SetFontFamilyName(temp.fname.c_str(), allText);

			temp.update(FCACHE_UPDATE);
			m_FW1RenderedText.push_back(temp);
		}

		m_FW1LineIndex++;	// advance global line index
	}
	else
	{
		// TODO: Sprite rendering
	}
}
void FontWrapper::clearFW1(ID3D11DeviceContext *con)
{
	m_FW1Wrapper->DrawString(con, L"", 10.0f, 0.0f, 0.0f, 0xff000000, 0);
	m_FW1TextGeometry->Clear();
}
void FontWrapper::presentFW1(ID3D11DeviceContext *con)
{
	if (m_isfw1)
	{
		m_FW1Wrapper->Flush(con);
		m_FW1Wrapper->DrawGeometry(devcon, m_FW1TextGeometry, NULL, NULL, FW1_RESTORESTATE);

		m_FW1LineIndex = 0;

		for (unsigned int i = 0; i < m_FW1RenderedText.size(); i++)
		{
			if (m_FW1RenderedText.at(i).update(FCACHE_CLEANUP) == FCACHE_PAST_LIFETIME)
			{
				m_FW1RenderedText.at(i).layout->Release();
				m_FW1RenderedText.erase(m_FW1RenderedText.begin() + i);
				i--;
			}
		}
	}
	else
		return;
}

bool RenderedText::update(bool accessed)
{
	int ct = clock();

	if (accessed == FCACHE_UPDATE)
	{
		m_life = ct;
		return FCACHE_OPERATIONAL;
	}
	else if (ct > m_life + 120 * CLOCKS_PER_SEC)
	{
		return FCACHE_PAST_LIFETIME;
	}
	else
		return FCACHE_OPERATIONAL;
}

namespace Antimony
{
	FontWrapper Arial, Consolas;
	IFW1Factory* FW1Factory;
}