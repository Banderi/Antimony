#pragma once

template <typename T> void smartRelease(T com)
{
	if (com)
	{
		com->Release();
		com = nullptr;
	}
}