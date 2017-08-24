#pragma warning (disable : 4101)		// unreferenced local variable


#include <Shlwapi.h>	// required for StrToIntExW				(Console::send)
#include <regex>		// required for std::find_if			(Console::send)

#include "Console.h"
#include "Gameplay.h"
#include "Spawner.h"

///

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
				if (obj == L"none" || obj == L"nothing")
				{
					log(L"What are you trying to accomplish there?\n", CSL_ERR_GENERIC);
					return false;
				}
				else if (obj == L"everything")
				{
					log(L"You'll have to be more specific\n", CSL_ERR_GENERIC);
					return false;
				}
				else
				{
					bool spawn_success = false;
					int qu = 1;
					float3 pos = Antimony::getPlayer()->getPos();

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

					spawn_success = Antimony::spawn(keyw.at(1), qu, pos);

					wchar_t coords[64];
					swprintf_s(coords, L"%f %f %f", pos.x, pos.y, pos.z);

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