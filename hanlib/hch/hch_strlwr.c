#include "hch.h"

unsigned char* hch_strlwr(unsigned char *str)
{
unsigned char *pch = str;
HANCHAR hch;

#ifdef DEBUG
	assert(str!=NULL);
#endif

	if (!str) return NULL;

	while (hch = hch_getNextHch(&pch))
		if (!ISHCH(hch))
			{
			char *pch2 = pch-1;
			if ((*pch2>='A')&&(*pch2<='Z'))
				{
				*pch2 -= 'A';
				*pch2 += 'a';
				}
			}
	return str;
}

