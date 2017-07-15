#pragma once

#include <vector>

#include "DirectX.h"
#include "Param.h"
#include "FontRenderer.h"

///

#define CSL_SYSTEM			0x00		//color(1,1,1,1)				// &#00
#define CSL_SUCCESS			0x01		//color(0.5,1,0.5,1)			// &#01
#define CSL_ERR_FATAL		0x02		//color(1,0,0,1)				// &#02
#define CSL_ERR_GENERIC		0x03		//color(0.8,0.2,0.2,1)			// &#03
#define CSL_INFO			0x04		//color(0.8,0.8,0.8,1)			// &#04
#define CSL_COMMAND			0x05		//color(0.8,0.8,0.8,1)			// &#05

///

class Console
{
private:
	bool m_enabled;
	bool m_open;

	bool m_historyDisplay;
	unsigned char m_historyMaxLineCount;
	std::vector<std::wstring> m_logHistory;
	std::wstring m_logBuffer;
	std::vector<std::wstring> m_cmdHistory;
	std::wstring m_cmdStr;
	float m_drawerTimout;
	float m_scroll;

	double m_caretTick;
	int m_caretPos, m_currCmd;
	bool m_showCaret;

public:
	void enable();
	void disable();
	bool isEnabled();

	void open();
	void close();
	bool isOpen();

	void parse(MSG msg, ControlParams *ctrls);
	bool send(std::wstring cmd);
	void clear();
	void purgeHistory();
	void log(std::wstring string, unsigned char col);

	void draw(DisplayParams *display, double delta, FontWrapper *font);

	Console()
	{
		m_scroll = 0;
		m_enabled = false;
		m_open = false;
		m_logBuffer = L"";
		m_historyMaxLineCount = 20;

		clear();
	}
};

UINT32 ColorCode(unsigned char c);