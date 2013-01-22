#ifndef __hin_internal_h_
#define __hin_internal_h_

#define INCL_PM
#include <os2.h>

#ifdef DEBUG
#include <stdio.h>
#include <assert.h>
#endif

#include "hch.h"

/* HIABUF STATES */
#define HIABUF_NONE	0
#define HIABUF_FST	1
#define HIABUF_DFST	2
#define HIABUF_MID	3
#define HIABUF_DMID	4
#define HIABUF_LST	5
#define HIABUF_DLST	6
#define HIABUF_END1	7
#define HIABUF_END2	8
#define HIABUF_FULL	9
#define HIABUF_SIZE	9


/* hch components */

#define HIAMASKFST		0x83FF
#define HIAMASKMID		0xFC1F
#define HIAMASKLST		0xFFE0
#define HBASECODE		0x8441

#define HFSTFILL	0x0400
#define HMIDFILL	0x0040
#define HLSTFILL	0x0001

#define LONGFROM2SHORT(h,l)		((((ULONG)((USHORT)h))<<16)|(USHORT)l)
#define HCHFROMLONG(u)			((USHORT)((ULONG)u & 0x0000FFFF))
#define KEYFROMLONG(u)			((USHORT)((ULONG)u >> 16))

typedef struct __HIABUFELEM__ HIABufElem;
struct __HIABUFELEM__ {
	HANCHAR hch;
	USHORT	key;
};

typedef struct __HIABUF__ HIABuf;
struct __HIABUF__ {
	ULONG newpos;
	ULONG item[HIABUF_SIZE];
};

typedef struct tagHIANotifWnd{
	HWND hwnd;
	USHORT id;
} HIANotifWnd;

typedef struct tagHIA HIA;
struct tagHIA {
	HWND hwndHIA;
	ULONG kbdtype;
	ULONG hanmode;
	ULONG insertmode;
	ULONG hcode;
	HIABuf *inbuf;
	ULONG notifListAllocSize;
	HIANotifWnd *notifList;
	HIANotifWnd *responseTo;
};

#define HIA_NOTIFLIST_DEFAULT_SIZE		256

void HIABufClear	(HIABuf *phiabuf);
void HIABufPush (HIABuf *phiabuf,HANCHAR hch,USHORT key);
ULONG HIABufPop(HIABuf *phiabuf);
ULONG HIABufPeek(HIABuf *phiabuf);

#define HIABufPopHch(buf)	(HCHFROMLONG(HIABufPop(buf)))
#define HIABufPeekHch(buf)	(HCHFROMLONG(HIABufPeek(buf)))
#define HIABufPopKey(buf)	(KEYFROMLONG(HIABufPop(buf)))
#define HIABufPeekKey(buf)	(KEYFROMLONG(HIABufPeek(buf)))

// ha2.c ha3.c

HANCHAR HIAAutomata2(HIA *phia,USHORT key);
HANCHAR HIAAutomata3(HIA *phia,USHORT key);


#endif // __hin_internal_h__

