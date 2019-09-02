#include "hin.h"
#include "hin_internal.h"

void HIABufClear	(HIABuf *phiabuf)
{
ULONG i;

#ifdef DEBUG
	assert(phiabuf!=NULL);
#endif

	for(i=0;i<HIABUF_SIZE;i++)
		phiabuf->item[i] = 0;

	phiabuf->newpos = 0;
}

void HIABufPush (HIABuf *phiabuf,HANCHAR hch,USHORT key)
{
#ifdef DEBUG
	assert(phiabuf!=NULL);
#endif

	if (phiabuf->newpos >= HIABUF_FULL)
		return;
	phiabuf->item[(phiabuf->newpos)++] = LONGFROM2SHORT(key,hch);
	return;
}

ULONG HIABufPop (HIABuf *phiabuf)
{
ULONG elem;

#ifdef DEBUG
	assert(phiabuf!=NULL);
#endif

	if (phiabuf->newpos == 0)
		return 0;

/*	  while ((phiabuf->newpos > 0) && (phiabuf->item[phiabuf->newpos-1] == 0))
		phiabuf->newpos --; */

	phiabuf->newpos--;
	elem = phiabuf->item[phiabuf->newpos];

	while ((phiabuf->newpos > 0) && (phiabuf->item[phiabuf->newpos-1] == 0))
		phiabuf->newpos --;

	return elem;
}

ULONG HIABufPeek (HIABuf *phiabuf)
{
ULONG elem;

#ifdef DEBUG
	assert(phiabuf!=NULL);
#endif

	if (phiabuf->newpos == 0)
		return 0;
	elem = phiabuf->item[phiabuf->newpos-1];
	return elem;
}

