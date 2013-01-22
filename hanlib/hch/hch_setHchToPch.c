#include "hch.h"

unsigned char* hch_setHchToPch(HANCHAR hch,unsigned char* pch)
{
	if (!pch) return pch;
	
	if ISHCH(hch)
		{
		*(pch)   = (unsigned char)(hch>>8);
		*(pch+1) = (unsigned char)(hch&0xFF);
		return (pch+2);
		} else {
		*pch = (unsigned char)(hch&0xFF);
		return (pch+1);
		}
}
