#include "hch.h"

HANCHAR hch_deleteHch(unsigned char* str)
{
unsigned char *src,*dst;
HANCHAR hch;

#ifdef DEBUG
	assert(str!=NULL);
#endif

	src = str;
	dst = str;
	hch = hch_getNextHch(&src);
	
	while (*(dst++)=*(src++));
	
	return hch;
}

HANCHAR hch_deleteHchStx(unsigned char* str,int stx)
{

#ifdef DEBUG
	assert(str!=NULL);
	assert(strlen(str)>=stx);
#endif

	if (str==NULL) return 0;
	if (strlen(str)<=stx) return 0;
	
	return hch_deleteHch(str+stx);
}
