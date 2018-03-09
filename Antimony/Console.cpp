//#include <conio.h>
#include <string>		// required for std::stoi			(Console::parse)

#include "Warnings.h"
#include "Console.h"
#include "Geometry.h"

///

void Console::enable()
{
	m_enabled = true;
}
void Console::disable()
{
	m_enabled = false;
}
bool Console::isEnabled()
{
	return m_enabled;
}

void Console::open()
{
	m_open = true;
}
void Console::close()
{
	m_open = false;
}
bool Console::isOpen()
{
	return m_open;
}
bool Console::isClosed()
{
	return !m_open;
}

void Console::parse(UINT message, WPARAM wParam, LPARAM lParam, ControlParams *ctrls)
{
	if (message == WM_CHAR)
	{
		unsigned char scancode = ((unsigned char*)&lParam)[2];
		unsigned int virtualKey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK);

		if (virtualKey == ctrls->k_console)		// do not print console key
			return;
		else
		{
			wchar_t c = wParam;

			if ((wParam <= 0xff && !isprint(c)))
				return;

			m_cmdStr.insert(m_caretPos, 1, c);
			m_caretPos++;
			return;
		}
	}
	switch (wParam)
	{
		case 0x08:		// Backspace
		{
			if (m_caretPos > 0)
			{
				m_cmdStr.erase(m_caretPos - 1, 1);
				m_caretPos -= 1;
			}
			break;
		}
		case 0x2e:		// Delete
		{
			m_cmdStr.erase(m_caretPos, 1);
			break;
		}
		case 0x0D:		// Enter
		{
			if (!m_cmdStr.empty() && m_cmdStr.find_first_not_of(' ') != std::string::npos)
			{
				m_cmdHistory.push_back(m_cmdStr);
				log(L">> " + m_cmdStr + L"\n", CSL_COMMAND);
				send(m_cmdStr);
				clear();
			}
			break;
		}
		case 0x26:		// Up
		{
			if (m_cmdHistory.size() > 0)
			{
				if (m_currCmd > 0)
				{
					m_currCmd--;
				}
				else
				{
					m_currCmd = m_cmdHistory.size() - 1;
				}
				m_cmdStr = m_cmdHistory.at(m_currCmd);
				m_caretPos = m_cmdStr.length();
			}
			break;
		}
		case 0x28:		// Up arrow
		{
			if (m_cmdHistory.size() > 0)
			{
				if (m_currCmd < m_cmdHistory.size() - 1)
				{
					m_currCmd++;
				}
				else
				{
					m_currCmd = 0;
				}
				m_cmdStr = m_cmdHistory.at(m_currCmd);
				m_caretPos = m_cmdStr.length();
			}
			break;
		}
		case 0x25:		// Left
		{
			m_caretPos--;
			if (m_caretPos < 0)
				m_caretPos = 0;
			break;
		}
		case 0x27:		// Right
		{
			m_caretPos++;
			if (m_caretPos > m_cmdStr.length())
				m_caretPos = m_cmdStr.length();
			break;
		}
	}
}
void Console::clear()
{
	m_cmdStr = L"";
	m_caretPos = 0;
	m_currCmd = m_cmdHistory.size();
	m_caretTick = 0;
	m_showCaret = false;
}
void Console::purgeHistory()
{
	m_logHistory.clear();
	m_scroll = 0.0f;
}
void Console::log(std::wstring string, unsigned char col)
{
	std::wstring newl = L"\n";

	std::wstring col_esc = L"";
	switch (col)
	{
		case CSL_SYSTEM:
		{
			col_esc = L"&#00";
			break;
		}
		case CSL_SUCCESS:
		{
			col_esc = L"&#01";
			break;
		}
		case CSL_ERR_FATAL:
		{
			col_esc = L"&#02";
			break;
		}
		case CSL_ERR_GENERIC:
		{
			col_esc = L"&#03";
			break;
		}
		case CSL_INFO:
		{
			col_esc = L"&#04";
			break;
		}
		case CSL_COMMAND:
		{
			col_esc = L"&#05";
			break;
		}
		default:
		{
			break;
		}
	}

	size_t pos = 0;
	std::wstring line;
	while ((pos = string.find(newl)) != std::string::npos)
	{
		line = string.substr(0, pos);

		m_logBuffer += col_esc + line;
		m_logHistory.push_back(m_logBuffer);
		m_logBuffer = L"";

		string.erase(0, pos + newl.length());
	}
	m_logBuffer += col_esc + string;

	if (m_logHistory.size() > m_historyMaxLineCount)
		m_scroll++;
}

void Console::draw(DisplayParams *display, double delta, FontWrapper *font)
{
	float lheight = 20;
	float cpadding = 20;

	if (m_drawerTimout > 0)
	{
		//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));
		Antimony::setDepthBufferState(OFF);
		Antimony::setShader(SHADERS_PLAIN);
		Antimony::Draw2DFullRect(
			display->width - cpadding, lheight * m_historyMaxLineCount,
			display->left + cpadding * 0.5, display->top + (lheight * m_historyMaxLineCount) * m_drawerTimout,
			2, color(0, 0, 0, 0.75), color(0.3, 0.1, 0.1, 1));
		Antimony::Draw2DFullRect(
			display->width - cpadding, 25,
			display->left + cpadding * 0.5, display->top + (lheight * m_historyMaxLineCount + 21) * m_drawerTimout,
			2, color(0, 0, 0, 0.75), color(0.3, 0.1, 0.1, 1));
	}

	if (isOpen())
	{
		if (m_drawerTimout < 1)
			m_drawerTimout += 10 * delta;
		if (m_drawerTimout > 1)
			m_drawerTimout = 1;

		///

		m_caretTick += 1.5 * delta;
		if (m_caretTick > 1)
		{
			m_caretTick = 0;
			m_showCaret = !m_showCaret;
		}

		float cmdx = 20;
		float cmdy = (lheight * m_historyMaxLineCount) * m_drawerTimout;

		float fsize = 10;
		float fclear = 7;

		font->render(L">", 12, cmdx, cmdy, 0xbbffffff, NULL);
		for (UINT i = 0; i < m_cmdStr.length(); i++)
		{
			auto c = m_cmdStr.c_str();
			wchar_t f[2] = L"";
			f[0] = c[i];
			font->render(f, 12, cmdx + 8 + i * fclear, cmdy, 0xffffffff, NULL);
		}
		//Antimony::fw1Courier->DrawString(devcon, m_cmdStr.c_str(), fsize, cmdx + 8, cmdy, 0xffffffff, NULL);

		if (m_showCaret)
			font->render(L"|", fsize, cmdx + 7 + m_caretPos * fclear, cmdy + 1, 0xbbffffff, FW1_ALIASED);

		/// History

		for (UINT i = 0; i < min(m_historyMaxLineCount, m_logHistory.size()); i++)
		{
			UINT l = i + (UINT)floor(m_scroll);
			std::wstring line = m_logHistory.at(l);
			std::wstring text;
			std::wstring colbuf;
			size_t pos = 0;
			float ind = 0.0f;
			unsigned char col = 0;
			if ((pos = line.find(L"&#")) != std::string::npos)
			{
				colbuf = line.substr(pos + 2, 2);
				col = std::stoi(colbuf);
				line.erase(0, pos + 4);
			}
			while ((pos = line.find(L"&#")) != std::string::npos)
			{
				text = line.substr(0, pos);

				font->render(text.c_str(), 12, cmdx + ind, cmdy - (m_logHistory.size() - l + 1) * lheight + 15, ColorCode(col), NULL);

				RECT r;
				r.top = 0;
				r.left = 0;
				r.bottom = display->height;
				r.right = display->width;
				ind += display->width - abs(font->getWidth(text.c_str(), 12, r, NULL));

				colbuf = line.substr(pos + 2, 2);
				col = std::stoi(colbuf);
				line.erase(0, pos + 4);
			}
			font->render(line.c_str(), 12, cmdx + ind, cmdy - (m_logHistory.size() - l + 1) * lheight + 15, ColorCode(col), NULL);
		}
	}
	else
	{
		if (m_drawerTimout > 0)
			m_drawerTimout -= 10 * delta;
		if (m_drawerTimout < 0)
			m_drawerTimout = 0;

		clear();
	}
}

UINT32 ColorCode(unsigned char c)
{
	switch (c)
	{
		case CSL_SYSTEM:
		{
			return 0xffffffff;
		}
		case CSL_SUCCESS:
		{
			return 0xff88ff88;
		}
		case CSL_ERR_FATAL:
		{
			return 0xff3333bb;
		}
		case CSL_ERR_GENERIC:
		{
			return 0xff7777aa;
		}
		case CSL_INFO:
		{
			return 0xffffcece;
		}
		case CSL_COMMAND:
		{
			return 0xff9a9a9a;
		}
		default:
		{
			return 0xffffffff;
		}
	}
}

namespace Antimony
{
	Console devConsole;
}