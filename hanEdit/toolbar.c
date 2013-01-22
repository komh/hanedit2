#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
#include <assert.h>
#endif

#include "../hanlib/han.h"
#include "../HMLE/HMLE.h"
#include "toolbar.h"

#define WINWORD_INSTANCE				0

#define TOOLBAR_DEFAULTITEMMAXCOUNT		50

typedef struct tagToolbarItem {
	HWND hwnd;
	int x;
	int xsize;
	int ysize;
	int visible;
} ToolbarItem, *PToolbarItem;

typedef struct tagToolbar {
	int itemMaxCount;
	int itemCount;
	int stepsize;
	PToolbarItem items;
} Toolbar,*PToolbar;

MRESULT APIENTRY ToolbarWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT toolbar_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT toolbar_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT toolbar_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT toolbar_wmCommand(HWND hwnd,MPARAM mp1,MPARAM mp2);

MRESULT toolbar_usermAddItem(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT toolbar_usermShowItem(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT toolbar_usermIsItemShowing(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT toolbar_usermQueryItemHwnd(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT toolbar_usermUpdate(HWND hwnd,MPARAM mp1,MPARAM mp2);

void toolbar_alignItems(PToolbar toolbar);

BOOL EXPENTRY RegisterToolbarControl(HAB hab)
{
	BOOL rc;
	rc = WinRegisterClass( hab, WC_TOOLBAR, ToolbarWinProc,
				CS_SIZEREDRAW|CS_SYNCPAINT, 4 );
	return rc;
}

MRESULT APIENTRY ToolbarWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

	switch (msg) {
	case WM_COMMAND:	return toolbar_wmCommand(hwnd, mp1, mp2 );
	case WM_CREATE:		return toolbar_wmCreate( hwnd, mp1, mp2 );
	case WM_DESTROY:	return toolbar_wmDestroy( hwnd, mp1, mp2 );
	case WM_PAINT:		return toolbar_wmPaint(hwnd, mp1, mp2 );

	case TOOLBAR_USERM_ADDITEM:			return toolbar_usermAddItem(hwnd,mp1,mp2);
	case TOOLBAR_USERM_SHOWITEM:		return toolbar_usermShowItem(hwnd,mp1,mp2);
	case TOOLBAR_USERM_ISITEMSHOWING:	return toolbar_usermIsItemShowing(hwnd,mp1,mp2);
	case TOOLBAR_USERM_QUERYITEMHWND:	return toolbar_usermQueryItemHwnd(hwnd,mp1,mp2);
	case TOOLBAR_USERM_UPDATE:			return toolbar_usermUpdate(hwnd,mp1,mp2);

	default:	return WinDefWindowProc( hwnd, msg, mp1, mp2 );
	}
}

MRESULT toolbar_usermAddItem(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PToolbar toolbar = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
PToolbarItemSpec itemSpec = mp1;
PToolbarItem newItem,lastItem = NULL;
ULONG ysize;
RECTL rectl;

	if (toolbar->itemCount >= toolbar->itemMaxCount)
		return MRFROMLONG(FALSE);
	if (toolbar->itemCount==0)
		lastItem = NULL;
		else
		lastItem = &(toolbar->items[toolbar->itemCount-1]);

	WinQueryWindowRect(hwnd,&rectl);
	if (itemSpec->ysize==0)
		ysize = rectl.yTop - rectl.yBottom;
		else
		ysize = itemSpec->ysize;

	newItem = malloc(sizeof(ToolbarItem));
	if (lastItem==NULL)
		newItem->x = 0;
		else
		newItem->x = lastItem->x + lastItem->xsize + toolbar->stepsize;
	newItem->xsize = itemSpec->xsize;

	if (itemSpec->className)
		{
		newItem->hwnd = WinCreateWindow(
			hwnd,itemSpec->className,
			itemSpec->text,itemSpec->style|WS_VISIBLE, //|WS_SYNCPAINT,
			newItem->x,0,newItem->xsize,ysize,
			hwnd,HWND_TOP,
			itemSpec->id,
			NULL,NULL);
		if (newItem->hwnd==NULLHANDLE)
			{
			free(newItem);
			return MRFROMLONG(FALSE);
			}
		WinQueryWindowRect(newItem->hwnd,&rectl);
		newItem->xsize = rectl.xRight - rectl.xLeft;
		newItem->ysize = rectl.yTop - rectl.yBottom;
		} else {
		newItem->hwnd = NULLHANDLE;
		newItem->xsize = itemSpec->xsize;
		newItem->ysize = ysize;
		}
	newItem->visible = TRUE;
	toolbar->items[toolbar->itemCount] = *newItem;
	toolbar->itemCount++;
	free(newItem);

	toolbar_alignItems(toolbar);

	WinInvalidateRect(hwnd,NULL,TRUE);

	return MRFROMLONG(toolbar->items[toolbar->itemCount-1].hwnd);
}

MRESULT toolbar_usermShowItem(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PToolbar toolbar = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int idx = (int)mp1;
BOOL visible = (BOOL)mp2;

	if (idx>=toolbar->itemCount)
		return MRFROMLONG(FALSE);

	if (toolbar->items[idx].visible != visible)
		{
		toolbar->items[idx].visible = visible;
//		toolbar_alignItems(toolbar);
//		WinInvalidateRect(hwnd,NULLHANDLE,TRUE);
		}

	return MRFROMLONG(TRUE);
}

MRESULT toolbar_usermUpdate(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PToolbar toolbar = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

	toolbar_alignItems(toolbar);
	WinInvalidateRect(hwnd,NULLHANDLE,TRUE);

	return 0L;
}

void toolbar_alignItems(PToolbar toolbar)
{
int idx;
int x=0;

	for (idx=0;idx<toolbar->itemCount;idx++)
		if (toolbar->items[idx].visible)
			{
			toolbar->items[idx].x = x;
			if (toolbar->items[idx].hwnd!=NULLHANDLE)
				WinSetWindowPos(toolbar->items[idx].hwnd,NULLHANDLE,
					x,0,toolbar->items[idx].xsize,toolbar->items[idx].ysize,
					SWP_SIZE|SWP_MOVE|SWP_SHOW);
			x += toolbar->items[idx].xsize;
			x += toolbar->stepsize;
			} else {
			if (toolbar->items[idx].hwnd!=NULLHANDLE)
				WinSetWindowPos(toolbar->items[idx].hwnd,NULLHANDLE,
					0,0,0,0,SWP_HIDE);
			}
}

MRESULT toolbar_usermIsItemShowing(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PToolbar toolbar = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int idx = (int)mp1;
BOOL visible;

	if (idx>=toolbar->itemCount)
		return MRFROMLONG(FALSE);

	visible = toolbar->items[idx].visible;

	return MRFROMLONG(visible);
}

MRESULT toolbar_usermQueryItemHwnd(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PToolbar toolbar = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int idx = (int)mp1;
HWND hwndItem;

	if (idx>=toolbar->itemCount)
		return MRFROMLONG(FALSE);

	hwndItem = toolbar->items[idx].hwnd;

	return MRFROMLONG(hwndItem);
}

MRESULT toolbar_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PToolbar toolbar;
ToolbarCtlData *ctlData = mp1;
//	printf("TOOLBAR::WM_CREATE\n");

	toolbar = malloc(sizeof(Toolbar));
	if (toolbar==NULL) return MRFROMLONG(TRUE);

	toolbar->itemMaxCount = TOOLBAR_DEFAULTITEMMAXCOUNT;
	toolbar->stepsize = 0;
	if (ctlData!=NULL)
		if (ctlData->cb == sizeof(ToolbarCtlData))
			{
			toolbar->itemMaxCount = ctlData->itemMaxCount;
			toolbar->stepsize = ctlData->stepsize;
			}
	if (toolbar->itemMaxCount < 1) toolbar->itemMaxCount = 1;
	toolbar->itemCount = 0;
	toolbar->items = calloc(toolbar->itemMaxCount,sizeof(ToolbarItem));
	if (toolbar->items == NULL) return MRFROMLONG(TRUE);

	WinSetWindowPtr(hwnd,WINWORD_INSTANCE,toolbar);

	return FALSE;
}

MRESULT toolbar_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PToolbar toolbar = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

//	printf("TOOLBAR::WM_DESTROY\n");
	if (toolbar==NULL) return FALSE;
	free(toolbar->items);
	free(toolbar);

	return FALSE;
}

MRESULT toolbar_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PToolbar toolbar = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

HPS		hps;
RECTL	rectlEntire,rectlUpdate;
RECTL	rectl;
POINTL	p;
int idx;
//	printf("TOOLBAR::WM_PAINT\n");

	hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);

	WinQueryWindowRect(hwnd,&rectlEntire);
//	WinFillRect(hps,&rectlEntire,SYSCLR_DIALOGBACKGROUND);
	GpiSetColor(hps,SYSCLR_BUTTONLIGHT);
	p.x = rectlEntire.xLeft;
	p.y = rectlEntire.yTop-1;
	GpiMove(hps,&p);
	p.x = rectlEntire.xRight;
//	GpiLine(hps,&p);

	rectl = rectlEntire;
	for (idx = 0; idx< toolbar->itemCount; idx++)
		if (toolbar->items[idx].visible)
		{
		rectl.xLeft = toolbar->items[idx].x;
		rectl.xRight = rectl.xLeft + toolbar->items[idx].xsize;
		if (toolbar->items[idx].hwnd!=NULLHANDLE)
			rectl.yBottom = toolbar->items[idx].ysize;
			else
			rectl.yBottom = 0;
		WinFillRect(hps,&rectl,SYSCLR_DIALOGBACKGROUND);
		}
	rectl.yBottom = 0;
	rectl.xLeft = rectl.xRight;
	rectl.xRight = rectlEntire.xRight;
	WinFillRect(hps,&rectl,SYSCLR_DIALOGBACKGROUND);

	WinEndPaint( hps );

	return MRFROMLONG(0L);
}

MRESULT toolbar_wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
HWND hwndOwner = WinQueryWindow(hwnd,QW_OWNER);

	WinPostMsg(hwndOwner,WM_COMMAND,mp1,mp2);
	return 0;
}
