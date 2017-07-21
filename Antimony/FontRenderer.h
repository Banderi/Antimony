#pragma once

#include <Windows.h>
#include <time.h>		// required for clock					(Console::send)

#include "FW1FontWrapper.h"

#include "Geometry.h"

#pragma comment (lib, "..\\ext\\FW1FontWrapper\\lib\\x86\\FW1FontWrapper.lib")

///

#define FCACHE_PAST_LIFETIME	true
#define FCACHE_OPERATIONAL		false

#define FCACHE_UPDATE			true
#define FCACHE_CLEANUP			false

///

class RenderedText
{
private:
	int m_life;

public:
	std::wstring text;
	std::wstring fname;
	IDWriteTextLayout *layout;
	IFW1FontWrapper *FW1Wrapper;
	float x, y;
	float size;
	UINT32 color;

	bool update(bool accessed = false);

	RenderedText()
	{
		m_life = clock();
	}
};

class FontWrapper
{
private:
	bool m_isfw1;
	IFW1FontWrapper *m_FW1Wrapper;
	wchar_t *m_fname;

	IDWriteFactory *m_FW1WriteFactory;
	IDWriteTextFormat *m_FW1TextFormat;
	IFW1TextGeometry *m_FW1TextGeometry;

	std::vector<RenderedText> m_FW1RenderedText;
	int m_FW1LineIndex;

public:
	HRESULT build(wchar_t *fname);
	HRESULT buildFW1(wchar_t *fname, IFW1Factory *fw1f);

	float getWidth(const wchar_t *str, float s, RECT r, UINT f);
	void render(const wchar_t *str, float s, float x, float y, UINT32 c, UINT flags, ID3D11DeviceContext *con = devcon);
	void clearFW1(ID3D11DeviceContext *con = devcon);
	void presentFW1(ID3D11DeviceContext *con = devcon);

	FontWrapper()
	{
		m_isfw1 = false;
	}
	~FontWrapper()
	{
		if (m_isfw1)
		{
			smartRelease(m_FW1Wrapper);
		}
	}
};