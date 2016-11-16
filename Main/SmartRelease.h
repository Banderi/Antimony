#ifndef SMARTR_H
#define SMARTR_H

template <typename T> void smartRelease(T com)
{
	if (com)
	{
		com->Release();
		com = nullptr;
	}
}

#endif