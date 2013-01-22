#include "hch.h"

int hch_queryHchType(unsigned char *str,int stx)
{
unsigned char *pch = str;
HANCHAR hch;

#ifdef DEBUG
	assert(str!=NULL);
	assert(strlen(str)>=stx);
#endif

	if (!str) return -1;
	if (strlen(str)<=stx) return -1;

	while (hch = hch_getNextHch(&pch))
		{
		if (ISHCH(hch))
			{
			if (pch-2 == str+stx) return HCH_HAN_LEAD;
			if (pch-1 == str+stx) return HCH_HAN_TAIL;
			} else {
			if (pch-1 == str+stx) return HCH_ENG;
			}
		} // while
	return -1;
}

