#define INCL_PM
#define INCL_DOSMEMMGR
#include <os2.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "../hanlib/han.h"
#include "HST.h"

typedef struct tagHStaticText {
	int fgColor;
	int bgColor;
	int allocsize;
	int len;
	char *str;
	HSTCTLDATA *pCtlData;
} HStaticText;


// --------------------------------------------------------------------------
// Definitions for the window instance data
// --------------------------------------------------------------------------
#define WINWORD_INSTANCE	0

// --------------------------------------------------------------------------
// Prototype for the main window procedure
// --------------------------------------------------------------------------
MRESULT APIENTRY HSTWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------
// Prototypes for the message processing worker functions
// --------------------------------------------------------------------------
MRESULT hst_wmCreate(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT hst_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT hst_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT hst_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT hst_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT hst_wmQueryWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT hst_wmSetWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2);

MRESULT hst_usermSetTextColor(HWND,MPARAM,MPARAM);
MRESULT hst_usermSetBackColor(HWND,MPARAM,MPARAM);
MRESULT hst_usermQueryTextColor(HWND,MPARAM,MPARAM);
MRESULT hst_usermQueryBackColor(HWND,MPARAM,MPARAM);

// Register the class.
BOOL EXPENTRY RegisterHanStaticTextControl(HAB hab)
{
BOOL rc;

	rc = WinRegisterClass( hab, WC_HST, HSTWinProc,
				CS_SIZEREDRAW, sizeof(PVOID) );
	return rc;
}

MRESULT APIENTRY HSTWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch (msg)
	{
	case HSTM_SETTEXTCOLOR:			return hst_usermSetTextColor(hwnd,mp1,mp2);
	case HSTM_SETBACKCOLOR:			return hst_usermSetBackColor(hwnd,mp1,mp2);
	case HSTM_QUERYTEXTCOLOR:		return hst_usermSetTextColor(hwnd,mp1,mp2);
	case HSTM_QUERYBACKCOLOR:		return hst_usermSetBackColor(hwnd,mp1,mp2);

	case WM_QUERYWINDOWPARAMS:		return hst_wmQueryWindowParams(hwnd,mp1,mp2);
	case WM_SETWINDOWPARAMS:		return hst_wmSetWindowParams(hwnd,mp1,mp2);
	case WM_CREATE:					return hst_wmCreate( hwnd, mp1, mp2 );
	case WM_DESTROY:				return hst_wmDestroy( hwnd, mp1, mp2 );
//	case WM_SIZE:					return hst_wmSize( hwnd, mp1, mp2 );
	case WM_PAINT:					return hst_wmPaint( hwnd, mp1, mp2 );
	default:						return WinDefWindowProc( hwnd, msg, mp1, mp2 );
	}
}

MRESULT hst_wmCreate(HWND hwnd, MPARAM mp1,MPARAM mp2)
{
PCREATESTRUCT pCreate = (PCREATESTRUCT)mp2;
//PHSTCTLDATA pHSTCD = (PHSTCTLDATA)mp1;
//PHSTCTLDATA pCtlData;
HStaticText *hst;

//	printf("HST::WM_CREATE...");
/*	if ((pHSTCD != NULL) && (pHSTCD->cb == sizeof(HSTCTLDATA)))
		{
		if (DosAllocMem((PPVOID)&pCtlData,sizeof(HSTCTLDATA),fALLOC))
			return MRFROMLONG(TRUE);
		pCtlData->cb = pHSTCD->cb;
*/
		if (DosAllocMem((PPVOID)&hst,sizeof(HStaticText),fALLOC))
			return MRFROMLONG(TRUE);

		WinSetWindowPtr(hwnd,WINWORD_INSTANCE,hst);
		
//		hst->pCtlData = pCtlData;
		hst->fgColor = SYSCLR_WINDOWSTATICTEXT;
		hst->bgColor = SYSCLR_DIALOGBACKGROUND;
		if (pCreate->pszText!=NULL)
			{
			hst->len = strlen(pCreate->pszText);
			hst->allocsize = hst->len+1;
			hst->str = malloc(hst->allocsize);
			strcpy(hst->str,pCreate->pszText);
			} else {
			hst->len = 0;
			hst->allocsize = 0;
			hst->str = NULL;
			}

//		} else return MRFROMLONG(TRUE);

//	printf("ok.\n");
	return FALSE;
}

MRESULT hst_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//PHSTCTLDATA  pCtlData;

//	pCtlData = hst->pCtlData;
	DosFreeMem(hst);
//	DosFreeMem(pCtlData);

	return 0L;
}

MRESULT hst_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
ULONG flStyle = WinQueryWindowULong( hwnd, QWL_STYLE );
USHORT ncx,ncy;

	ncx = SHORT1FROMMP(mp2);
	ncy = SHORT2FROMMP(mp2);
	flStyle = 0;

	return 0L;
}

MRESULT hst_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HWND hwndMp1 = (HWND)mp1;

	if (SHORT1FROMMP(mp2))
		WinSetFocus(HWND_DESKTOP,hwndMp1);

	return 0L;
}

MRESULT hst_usermSetTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

	oldColor = hst->fgColor;
	hst->fgColor = (LONG)mp1;
	return MRFROMLONG(oldColor);
}

MRESULT hst_usermSetBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

	oldColor = hst->bgColor;
	hst->bgColor = (LONG)mp1;
	return MRFROMLONG(oldColor);
}

MRESULT hst_usermQueryTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

	return MRFROMLONG(hst->fgColor);
}

MRESULT hst_usermQueryBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

	return MRFROMLONG(hst->bgColor);
}

MRESULT hst_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HPS hps;
char *str;
RECTL rectlEntire;
ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
int x=0,y=0;

//	printf("HST::WM_PAINT...");

	hps=WinBeginPaint(hwnd, NULLHANDLE, NULL);
//	assert(hps!=NULLHANDLE);
	WinQueryWindowRect(hwnd,&rectlEntire);
	WinFillRect(hps,&rectlEntire,hst->bgColor);

	if (hst->str!=NULL)
		{

		str = malloc(strlen(hst->str)+1);
		strcpy(str,hst->str);
		hch_ks2systr(str);
		GpiSetColor(hps,hst->fgColor);
		
		if (flStyle & DT_LEFT)
			x = 0;
		if (flStyle & DT_CENTER)
			x = (rectlEntire.xRight - strlen(hst->str)*8)/2;
		if (flStyle & DT_RIGHT)
			x = rectlEntire.xRight - strlen(hst->str)*8;
		if (flStyle & DT_TOP)
			y = rectlEntire.yTop - 16;
		if (flStyle & DT_VCENTER)
			y = (rectlEntire.yTop - 16) / 2;
		if (flStyle & DT_BOTTOM)
			y = 0;
		HanOut(hps,x,y,str);
		free(str);

		}

	WinEndPaint(hps);

//	printf("ok.\n");
	return 0L;
}

MRESULT hst_wmQueryWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PWNDPARAMS wndParams = (PWNDPARAMS)mp1;
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

	printf("HST::WM_QUERYWINDOWPARAMS...\n");

	if (wndParams->fsStatus & WPM_CCHTEXT)
		wndParams->cchText = hst->len;

	if (wndParams->fsStatus & WPM_TEXT)
		strcpy(wndParams->pszText,hst->str);

/*	if (wndParams->fsStatus & WPM_CBCTLDATA)
		wndParams->cbCtlData = sizeof(HSTCTLDATA);

	if (wndParams->fsStatus & WPM_CTLDATA)
		memcpy(wndParams->pCtlData,hst->pCtlData,sizeof(HSTCTLDATA));
*/
//	wndParams->pPresParams = NULL;
	wndParams->cbPresParams = 0;

	printf("ok\n");
	return MRFROMLONG(TRUE);
}

MRESULT hst_wmSetWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PWNDPARAMS wndParams = (PWNDPARAMS)mp1;
HStaticText *hst = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

//	printf("HST::WM_SETWINDOWPARAMS...");
	
	if (wndParams->fsStatus & WPM_TEXT)
		if (wndParams->pszText != NULL)
			{
			if (hst->allocsize!=0)
				free(hst->str);
			hst->len = strlen(wndParams->pszText);
			hst->allocsize = hst->len + 1;
			hst->str = malloc(hst->allocsize);
			strcpy(hst->str,wndParams->pszText);
			}

/*	if (wndParams->fsStatus & WPM_PRESPARAMS)
		{
		printf("WPM_PRESPARAMS:size = %d\n",wndParams->cbPresParams);
		hst->fgColor = CLR_RED;
		} */
//	wndParams->pPresParams = NULL;
//	wndParams->cbPresParams = 0;
	WinInvalidateRect(hwnd,NULLHANDLE,FALSE);

	printf("ok\n");
	return MRFROMLONG(TRUE);
}
