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
#include "HMLE.h"

#define APPCLASS	"App:HMLE Test"
#define APPTITLE	"HMLE Test"
#define ID_APP		0
#define ID_HIA		1
#define ID_HMLE		101

MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

static MRESULT mainwmChar(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT mainwmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT mainwmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT mainwmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2);

int winmain(void);
void morph(void);

static HWND		hwndHIA = NULLHANDLE;
static HWND		hwndHMLE = NULLHANDLE;

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
		if (!RegisterHanMLEControl(hab)) break;
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

	hwndHMLE = WinCreateWindow (
		hwnd,WC_HMLE,
		NULL,WS_VISIBLE|HMLS_BORDER|HMLS_NOAUTOCREATEHIA,
		4,4,200,100,
		hwnd,HWND_TOP,
		ID_HMLE,
		NULL,NULL);
	if (hwndHMLE==NULLHANDLE) printf("error! HEF\n");
	WinSendMsg(hwndHMLE,HMLM_SETTEXTCOLOR,MPFROMLONG(CLR_BLACK),0);
	WinSendMsg(hwndHMLE,HMLM_SETBACKCOLOR,MPFROMLONG(CLR_PALEGRAY),0);
	WinSendMsg(hwndHMLE,HMLM_SETSELTEXTCOLOR,MPFROMLONG(CLR_BLACK),0);
	WinSendMsg(hwndHMLE,HMLM_SETSELBACKCOLOR,MPFROMLONG(CLR_RED),0);
	WinSendMsg(hwndHMLE,HMLM_SETHIA,MPFROMLONG(hwndHIA),0);

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

	return 0L;
}

MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
//	printf("TEST:: WM_CLOSE\n");

	WinSendMsg(hwndHMLE,WM_CLOSE,0L,0L);
	WinPostMsg(hwnd,WM_QUIT,0L,0L);
	return 0L;
}

MRESULT mainwmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
	switch (SHORT1FROMMP(mp1))
	{
	case ID_HMLE:
		switch (SHORT2FROMMP(mp1))
		{
		case HMLN_KILLFOCUS:
			break;
		case HMLN_SETFOCUS:
			break;
		case HMLN_CHANGED:
			WinInvalidateRect(hwnd,NULL,FALSE);
			break;
		case HMLN_OVERFLOW:
			printf("Overflow!\n");
			break;
		default:
			break;
		} // switch Notification
		break;
	} // switch Control

	return MRFROMLONG(0L);
}

MRESULT mainwmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HPS hps;
RECTL rectlUpdate;
//char str[203];

	hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);
#ifdef DEBUG
	assert(hps!=NULLHANDLE);
#endif
	if (hps==NULLHANDLE) return 0;

//	WinQueryWindowRect(hwnd,&rectlEntire);

//	GpiSetBackMix(hps,BM_OVERPAINT);

	WinEndPaint(hps);
	return 0L;
}

