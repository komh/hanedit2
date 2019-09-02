#include "hch.h"

HANCHAR hch_getNextHch(unsigned char** pstr)
{
HANCHAR hch = 0;

	if (*(*pstr)==0) return 0;
	if (*((*pstr)+1)==0) return (HANCHAR)(*((*pstr)++));
	
	if (!(*(*pstr)&0x80))
		{
		hch = (HANCHAR)(*((*pstr)++));
		} else {
		hch = ((HANCHAR)(*((*pstr)++))) << 8 ;
		hch |= (*((*pstr)++));
		}
	return hch;
}

