#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#include <os2.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include <assert.h>
#include "../etclib/debugutil.h"
#endif

#include "../hanlib/han.h"
#include "HEF.h"

#define APPCLASS	"App:HEF Test"
#define APPTITLE	"HEF Test"
#define ID_APP		0
#define ID_HIA		1
#define ID_HEF1		101
#define ID_HEF2		102

MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

static MRESULT mainwmChar(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT mainwmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT mainwmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT mainwmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2);

int winmain(void);
void morph(void);

static HWND		hwndHEF1 = NULLHANDLE;
static HWND		hwndHEF2 = NULLHANDLE;
static HWND		hwndHIA = NULLHANDLE;

int main(int argc, char *argv[], char *envp[])
{
	morph();
	winmain();
	return 0;
}

void morph(void)
{
PPIB ppib = NULL;
PTIB ptib = NULL;
APIRET rc;

	rc = DosGetInfoBlocks(&ptib,&ppib);
	if (rc != NO_ERROR)
		{
		printf("DosGetInfoBlocks Error : %d\n",rc);
		exit(-1);
		}
	ppib->pib_ultype = 3;
}


int winmain()
{
HAB   hab = NULLHANDLE;
HMQ   hmq = NULLHANDLE;
QMSG  qmsg;
HWND  hwndFrame = NULLHANDLE;
HWND  hwndClient = NULLHANDLE;
ULONG ctlData = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
		FCF_TASKLIST;

	do	{
		if ((hab = WinInitialize(0)) == NULLHANDLE) break;
		if ((hmq = WinCreateMsgQueue(hab,0)) == NULLHANDLE) break;
		if (!WinRegisterClass(hab, APPCLASS,
			ClientWndProc, CS_SIZEREDRAW, 4)) break;


		// Register the HanMLE and HanInputAutomata window class
		if (!RegisterHanAutomataClass(hab)) break;
		if (!RegisterHanEntryFieldControl(hab)) break;
		if ((hwndFrame = WinCreateStdWindow(
					HWND_DESKTOP, WS_VISIBLE,
					&ctlData,
					APPCLASS,
					APPTITLE,
					0, NULLHANDLE,
					ID_APP,
					&hwndClient )) == NULLHANDLE )
			{
			printf("errorcode: %x\n",WinGetLastError(hab));
			break;
			}

		WinSetWindowPos(hwndFrame,NULLHANDLE,
				300,300,300,200,
				SWP_MOVE|SWP_SIZE);

		houtInit(hab,256);

		while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
			{
			WinDispatchMsg(hab, &qmsg);
			}

		houtClose();

		} while (FALSE);

	if (hmq != NULLHANDLE) WinDestroyMsgQueue(hmq);
	if (hab != NULLHANDLE) WinTerminate(hab);

	return 0;
}


MRESULT APIENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch (msg)
	{
	case WM_ERASEBACKGROUND:	return MRFROMLONG(TRUE);
	case WM_CONTROL:		return mainwmControl(hwnd, mp1, mp2);
	case WM_CREATE:			return mainwmCreate(hwnd, mp1, mp2);
	case WM_CLOSE:			return mainwmClose(hwnd,mp1,mp2);
	case WM_CHAR:			return mainwmChar(hwnd,mp1,mp2);
	case WM_PAINT:			return mainwmPaint(hwnd,mp1,mp2);

	default:			return WinDefWindowProc(hwnd, msg, mp1, mp2);
	} /* endswitch */
}

MRESULT mainwmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
	hwndHIA = HIACreateHanAutomata(hwnd,ID_HIA);
	assert(hwndHIA!=NULLHANDLE);
	if (hwndHIA==NULLHANDLE) return MRFROMLONG(TRUE);

	hwndHEF1 = WinCreateWindow (
		hwnd,WC_HEF,
		NULL,WS_VISIBLE|HES_MARGIN|HES_NOAUTOCREATEHIA|HES_ACCEPTFILEDROP,
		10,30,200,18,
		hwnd,HWND_TOP,
		ID_HEF1,
		NULL,NULL);
	if (hwndHEF1==NULLHANDLE) printf("error! HEF\n");
	WinSendMsg(hwndHEF1,HEM_SETTEXTCOLOR,MPFROMLONG(CLR_BLACK),0);
	WinSendMsg(hwndHEF1,HEM_SETBACKCOLOR,MPFROMLONG(CLR_PALEGRAY),0);
	WinSendMsg(hwndHEF1,HEM_SETSELTEXTCOLOR,MPFROMLONG(CLR_BLACK),0);
	WinSendMsg(hwndHEF1,HEM_SETSELBACKCOLOR,MPFROMLONG(CLR_RED),0);
	WinSendMsg(hwndHEF1,HEM_SETHIA,MPFROMLONG(hwndHIA),0);

	hwndHEF2 = WinCreateWindow (
		hwnd,WC_HEF,
		NULL,WS_VISIBLE|HES_MARGIN|HES_UNREADABLE|HES_NOAUTOCREATEHIA,
		10,10,200,18,
		hwnd,HWND_TOP,
		ID_HEF2,
		NULL,NULL);
	if (hwndHEF2==NULLHANDLE) printf("error! HEF\n");
	WinSendMsg(hwndHEF2,HEM_SETTEXTCOLOR,MPFROMLONG(CLR_GREEN),0);
	WinSendMsg(hwndHEF2,HEM_SETBACKCOLOR,MPFROMLONG(CLR_BLACK),0);
	WinSendMsg(hwndHEF2,HEM_SETSELTEXTCOLOR,MPFROMLONG(CLR_BLACK),0);
	WinSendMsg(hwndHEF2,HEM_SETSELBACKCOLOR,MPFROMLONG(CLR_WHITE),0);
	WinSendMsg(hwndHEF2,HEM_SETHIA,MPFROMLONG(hwndHIA),0);

	WinSetWindowText(hwndHEF1,"kwisatz");

	return MRFROMLONG(FALSE);
}

MRESULT mainwmChar(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
//USHORT	fsFlags = SHORT1FROMMP(mp1);
//UCHAR	ucVkey	= CHAR3FROMMP(mp2);
//UCHAR	ucChar	= CHAR1FROMMP(mp2);
//UCHAR	ucScancode = CHAR4FROMMP(mp1);
//char str[100];
//	printf("TEST:: WM_CHAR\n");

//	ucChar = toupper(ucChar);
	return 0L;
}

MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
//	printf("TEST:: WM_CLOSE\n");

	WinSendMsg(hwndHEF1,WM_CLOSE,0L,0L);
	WinSendMsg(hwndHEF2,WM_CLOSE,0L,0L);
	WinPostMsg(hwnd,WM_QUIT,0L,0L);
	return 0L;
}

MRESULT mainwmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
	switch (SHORT1FROMMP(mp1))
	{
	case ID_HEF1:
		switch (SHORT2FROMMP(mp1))
		{
		case HEN_KILLFOCUS:
			break;
		case HEN_SETFOCUS:
			break;
		case HEN_CHANGE:
			WinInvalidateRect(hwnd,NULL,FALSE);
			break;
		default:
			break;
		} // switch Notification
		break;
	case ID_HEF2:
		switch (SHORT2FROMMP(mp1))
		{
		case HEN_CHANGE:
			WinInvalidateRect(hwnd,NULL,FALSE);
			break;
		}
	} // switch Control

	return MRFROMLONG(0L);
}

MRESULT mainwmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HPS hps;
RECTL rectlEntire,rectlUpdate;
char str[203];

	hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);
#ifdef DEBUG
	assert(hps!=NULLHANDLE);
#endif
	if (hps==NULLHANDLE) return 0;

	WinQueryWindowRect(hwnd,&rectlEntire);

	GpiSetBackMix(hps,BM_OVERPAINT);

	WinQueryWindowText(hwndHEF1,201,str);
//	printf("%d\n",strlen(str));
	strcat(str,"  ");
	HanOut(hps,0,100,str);
	WinQueryWindowText(hwndHEF2,201,str);
	strcat(str,"  ");
	HanOut(hps,0,80,str);

	WinEndPaint(hps);
	return 0L;
}

