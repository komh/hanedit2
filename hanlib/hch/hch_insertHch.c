#include "hch.h"

unsigned char *hch_insertHch (unsigned char *str,HANCHAR hch)
{
unsigned char *dst,*src;

#ifdef DEBUG
	assert(str!=NULL);
#endif

	if (!str) return NULL;

	src = str+strlen(str);
	if (ISHCH(hch))
		dst = src+2;
		else
		dst = src+1;

	while (src >= str)
		{
		*(dst--) = *(src--);
		}

	if (ISHCH(hch))
		{
		*(str  )=(unsigned char)(hch>>8);
		*(str+1)=(unsigned char)(hch);
		return (str+2);
		} else {
		*(str  )=(unsigned char)(hch);
		return (str+1);
		}
}

int hch_insertHchStx(unsigned char *str,HANCHAR hch,int stx)
{
unsigned char *pch = str;

#ifdef DEBUG
	assert(str!=NULL);
	assert(strlen(str)>=stx);
#endif

	if (str==NULL) return 0;
	if (strlen(str)<stx) return 0;
	
	pch = hch_insertHch(str+stx,hch);
	if (pch==NULL) return 0;
	
	return pch-(str+stx);
}
