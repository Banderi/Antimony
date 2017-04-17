//#include <conio.h>
#include <Shlwapi.h>	// required for StrToIntExW				(Console::send)
#include <regex>		// required for std::find_if			(Console::send)

#include "Console.h"
#include "Antimony.h"
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

void Console::parse(MSG msg)
{
	switch (msg.wParam)
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
		default:
		{
			if (msg.wParam == antimony.controls.k_console)
				break;
			else
			{
				wchar_t c;

				TranslateMessage(&msg);
				DispatchMessageW(&msg);

				bool valid = false;

				if (!valid)
				{
					if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
					{
						if (msg.message == WM_CHAR)
						{
							c = msg.wParam;
							valid = true;

							if (c <= 0xff)
							{
								if (!isprint(c))
									valid = false;
							}
						}
						TranslateMessage(&msg);
						DispatchMessageW(&msg);
					}
				}

				if (valid)
				{
					m_cmdStr.insert(m_caretPos, 1, c);
					m_caretPos++;
				}
				break;
			}
		}
	}
}
bool Console::send(std::wstring cmd)
{
	std::vector<std::wstring> keyw;

	// remove trailing spaces
	cmd.erase(cmd.begin(), std::find_if(cmd.begin(), cmd.end(), std::bind1st(std::not_equal_to<char>(), ' ')));

	// split the string into keywords
	size_t cp = 0;
	std::wstring space = L" ";
	while ((cp = cmd.find(space)) != std::wstring::npos)
	{
		if (!cmd.substr(0, cp).empty())
			keyw.push_back(cmd.substr(0, cp));
		cmd.erase(0, cp + space.length());
	}
	if (cmd != L"")
		keyw.push_back(cmd);

	// check commands
	if (keyw.size() > 0)
	{
		auto lit = keyw.at(0);
		if (lit == L"spawn")
		{
			if (keyw.size() > 1)
			{
				auto obj = keyw.at(1);
				if (obj == L"none")
				{
					log(L"What are you trying to accomplish there?\n", CSL_ERR_GENERIC);
					return false;
				}
				else
				{
					bool spawn_success = false;
					int qu = 1;
					float3 pos = antimony.getPlayer()->getPos();

					if (keyw.size() > 2)
					{
						try
						{
							qu = std::stoi(keyw.at(2));
						}
						catch (std::invalid_argument& e)
						{
							log(L"'" + keyw.at(2) + L"' is not a valid quantity\n", CSL_ERR_GENERIC);
							return false;
						}
						catch (std::out_of_range& e)
						{
							log(L"'" + keyw.at(2) + L"' is out of range\n", CSL_ERR_GENERIC);
							return false;
						}
						if (qu < 1)
						{
							log(L"Spawn quantity cannot be less then 1\n", CSL_ERR_GENERIC);
							return false;
						}

						if (keyw.size() > 5)
						{
							wchar_t *p1, *p2, *p3;
							pos.x = wcstod(keyw.at(3).c_str(), &p1);
							pos.y = wcstod(keyw.at(4).c_str(), &p2);
							pos.z = wcstod(keyw.at(5).c_str(), &p3);
							if (*p1 || *p2 || *p3)
							{
								log(L"Unrecognized syntax: '" + keyw.at(3) + L" " + keyw.at(4) + L" " + keyw.at(5) + L"' - format is 'X Y Z'\n", CSL_ERR_GENERIC);
								return false;
							}
						}
						else if (keyw.size() > 4)
						{
							log(L"Unrecognized syntax: '" + keyw.at(3) + L" " + keyw.at(4) + L"' - format is 'X Y Z'\n", CSL_ERR_GENERIC);
							return false;
						}
						else if (keyw.size() > 3)
						{
							log(L"Unrecognized syntax: '" + keyw.at(3) + L"' - format is 'X Y Z'\n", CSL_ERR_GENERIC);
							return false;
						}
					}

					spawn_success = antimony.spawn(keyw.at(1), qu, pos);

					wchar_t coords[64];
					swprintf(coords, L"%f %f %f", pos.x, pos.y, pos.z);

					if (!spawn_success)
					{
						//auto s = std::to_wstring(qu);
						//log(L"goddamit\n", CSL_ERR_GENERIC);
						//log(L"Could not spawn " + std::to_wstring(qu) + L" " + obj + L" at coordinates " + coords + L"\n", CSL_ERR_GENERIC);
						return false;
					}
					else
					{
						log(L"Spawned " + std::to_wstring(qu) + L" '" + obj + L"' at coordinates " + coords + L"\n", CSL_SUCCESS);
						return true;
					}
				}
			}
			else
			{
				log(L"Missing object ID after command 'spawn'\n", CSL_ERR_GENERIC);
				return false;
			}
		}
		else if (lit == L"help")
		{
			log(L"I haven't implemented this command yet...\n", CSL_SYSTEM);
		}
		else if (lit == L"give")
		{
			if (keyw.size() > 1)
			{
				auto obj = keyw.at(1);
				if (obj == L"")
				{
					//
				}
				else
				{
					log(L"Unrecognized object ID: '" + keyw.at(1) + L"'\n", CSL_ERR_GENERIC);
					return false;
				}
			}
			else
			{
				log(L"Missing object ID after command 'give'\n", CSL_ERR_GENERIC);
				return false;
			}
		}
		else if (lit == L"remove")
		{
			log(L"I haven't implemented this command yet...\n", CSL_SYSTEM);
		}
		else if (lit == L"loadmap")
		{
			log(L"I haven't implemented this command yet...\n", CSL_SYSTEM);
		}
		else if (lit == L"load")
		{
			log(L"I haven't implemented this command yet...\n", CSL_SYSTEM);
		}
		else if (lit == L"save")
		{
			log(L"I haven't implemented this command yet...\n", CSL_SYSTEM);
		}
		else if (lit == L"connect")
		{
			log(L"I haven't implemented this command yet...\n", CSL_SYSTEM);
		}
		else if (lit == L"quit" || lit == L"exit")
		{
			PostQuitMessage(0);
		}
		else if (lit == L"clearconsole" || lit == L"cls")
		{
			purgeHistory();
		}
		else
		{
			log(L"Unrecognized command: '" + keyw.at(0) + L"'\n", CSL_ERR_GENERIC);
		}
	}

	return false;
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

void Console::draw()
{
	float lheight = 20;
	float cpadding = 20;

	if (m_drawerTimout > 0)
	{
		devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &vertex_stride, &vertex_offset);
		setDepthBufferState(OFF);
		setShader(SHADERS_PLAIN);
		Draw2DFullRect(
			antimony.display.width - cpadding, lheight * m_historyMaxLineCount,
			antimony.display.left + cpadding * 0.5, antimony.display.top + (lheight * m_historyMaxLineCount) * m_drawerTimout,
			2, color(0, 0, 0, 0.75), color(0.3, 0.1, 0.1, 1));
		Draw2DFullRect(
			antimony.display.width - cpadding, 25,
			antimony.display.left + cpadding * 0.5, antimony.display.top + (lheight * m_historyMaxLineCount + 21) * m_drawerTimout,
			2, color(0, 0, 0, 0.75), color(0.3, 0.1, 0.1, 1));
	}

	if (isOpen())
	{
		if (m_drawerTimout < 1)
			m_drawerTimout += 10 * antimony.getDelta();
		if (m_drawerTimout > 1)
			m_drawerTimout = 1;

		///

		m_caretTick += 1.5 * antimony.getDelta();
		if (m_caretTick > 1)
		{
			m_caretTick = 0;
			m_showCaret = !m_showCaret;
		}

		float cmdx = 20;
		float cmdy = (lheight * m_historyMaxLineCount) * m_drawerTimout;

		float fsize = 10;
		float fclear = 7;

		antimony.fw1Courier->DrawString(devcon, L">", 12, cmdx, cmdy, 0xbbffffff, NULL);
		for (UINT i = 0; i < m_cmdStr.length(); i++)
		{
			auto c = m_cmdStr.c_str();
			wchar_t f[2] = L"";
			f[0] = c[i];
			antimony.fw1Courier->DrawString(devcon, f, 12, cmdx + 8 + i * fclear, cmdy, 0xffffffff, NULL);
		}
		//antimony.fw1Courier->DrawString(devcon, m_cmdStr.c_str(), fsize, cmdx + 8, cmdy, 0xffffffff, NULL);

		if (m_showCaret)
			antimony.fw1Courier->DrawString(devcon, L"|", fsize, cmdx + 7 + m_caretPos * fclear, cmdy + 1, 0xbbffffff, FW1_ALIASED);

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

				antimony.fw1Courier->DrawString(devcon, text.c_str(), 12, cmdx + ind, cmdy - (m_logHistory.size() - l + 1) * lheight + 15, ColorCode(col), NULL);

				FW1_RECTF r;
				r.Top = 0;
				r.Left = 0;
				r.Bottom = antimony.display.height;
				r.Right = antimony.display.width;
				ind += antimony.display.width - abs(antimony.fw1Courier->MeasureString(text.c_str(), L"Consolas", 12, &r, NULL).Right);

				colbuf = line.substr(pos + 2, 2);
				col = std::stoi(colbuf);
				line.erase(0, pos + 4);
			}
			antimony.fw1Courier->DrawString(devcon, line.c_str(), 12, cmdx + ind, cmdy - (m_logHistory.size() - l + 1) * lheight + 15, ColorCode(col), NULL);
		}
	}
	else
	{
		if (m_drawerTimout > 0)
			m_drawerTimout -= 10 * antimony.getDelta();
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