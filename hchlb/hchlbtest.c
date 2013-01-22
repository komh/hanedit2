#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../hanlib/han.h"
#include "hchlb.h"

#define APPCLASS    "App:HanInDlg Test"
#define APPTITLE    "HanInDlg Test"
#define ID_APP      100
#define ID_HCHLB    101

char hStr[ 3 ] = { 0x20, 0x20, 0 };

MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

static MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT mainwmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );

void morph(void);
int winmain(void);

int main(int argc, char *argv[], char *envp[])
{
    morph();
    winmain();
    return 0;
}

static char external_hanjafontbuf[HOUT_HANJAFONTBUFSIZE];

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

    do  {
        if ((hab = WinInitialize(0)) == NULLHANDLE) break;
        if ((hmq = WinCreateMsgQueue(hab,0)) == NULLHANDLE) break;
        if (!WinRegisterClass(hab, APPCLASS,
            ClientWndProc, CS_SIZEREDRAW, 4)) break;

        if( !RegisterHanCharListBoxControl( hab )) break;


        houtInit(hab,256);
        houtReadHanjaFont("f:/apps/fool/font/hanja.fnt",external_hanjafontbuf);
        houtSetHanjaFont(external_hanjafontbuf);


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

        WinSetWindowPos(hwndFrame,HWND_TOP,
                300,300,300,200,
                SWP_MOVE|SWP_SIZE);

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
    case WM_CREATE:             return mainwmCreate(hwnd, mp1, mp2);
    case WM_CLOSE:              return mainwmClose(hwnd,mp1,mp2);
    case WM_SIZE:
    {
        SHORT cx = SHORT1FROMMP( mp2 ), cy = SHORT2FROMMP( mp2 );

        WinSetWindowPos( WinWindowFromID( hwnd, ID_HCHLB ), HWND_TOP,
                         0, 0, cx - 20, cy,
                         SWP_MOVE | SWP_SIZE );

        return 0;
    }

    case WM_PAINT:
    {
        HPS     hps;
        RECTL   rcl;

        hps = WinBeginPaint( hwnd, NULLHANDLE, &rcl );
        WinFillRect( hps, &rcl, SYSCLR_BUTTONMIDDLE );
        HanOut( hps, rcl.xRight - 20, rcl.yTop - 20, hStr );

        WinEndPaint( hps );

        return 0;
    }

    case WM_CONTROL :
    {
        SHORT id = SHORT1FROMMP( mp1 );

        if( id == ID_HCHLB )
        {
            switch( SHORT2FROMMP( mp1 ))
            {
                case HCHLN_SELECT :
                {
                    SHORT index = SHORT1FROMMP( mp2 );
                    SHORT selection = SHORT2FROMMP( mp2 );
                    HANCHAR hch = 0x2020;
                    RECTL   rcl;

                    if( selection )
                        hch = LONGFROMMR( WinSendMsg( WinWindowFromID( hwnd, id ),
                                    HCHLM_QUERYHCH, MPFROMSHORT( index ), 0 ));

                    hStr[ 1 ] = hch & 0xFF;
                    hStr[ 0 ] = ( hch & 0xFF00) >> 8;

                    WinQueryWindowRect( hwnd, &rcl );
                    rcl.xLeft = rcl.xRight - 20;
                    WinInvalidateRect( hwnd, &rcl, FALSE );
                    break;
                }

                case HCHLN_ENTER :
                {
                    SHORT index = SHORT1FROMMP( mp2 );
                    HANCHAR hch = 0x2020;
                    RECTL   rcl;

                    WinAlarm( HWND_DESKTOP, WA_NOTE );

                    hch = LONGFROMMR( WinSendMsg( WinWindowFromID( hwnd, id ),
                                HCHLM_QUERYHCH, MPFROMSHORT( index ), 0 ));

                    hStr[ 1 ] = hch & 0xFF;
                    hStr[ 0 ] = ( hch & 0xFF00) >> 8;

                    WinQueryWindowRect( hwnd, &rcl );
                    rcl.xLeft = rcl.xRight - 20;
                    WinInvalidateRect( hwnd, &rcl, FALSE );
                    break;
                }
            }
        }

        return 0;
    }

    case WM_BUTTON1CLICK :
    {
        RECTL rcl;
        HANCHAR hch = 49889;
        SCSELINFO scselInfo = { -1, -1, -1 };

        hch = scselDlg( HWND_DESKTOP, hwnd, NULLHANDLE, &scselInfo );

        hStr[ 1 ] = hch & 0xFF;
        hStr[ 0 ] = ( hch & 0xFF00) >> 8;

        WinQueryWindowRect( hwnd, &rcl );
        rcl.xLeft = rcl.xRight - 20;
        WinInvalidateRect( hwnd, &rcl, FALSE );

        return MRFROMLONG( TRUE );
    }

    case WM_BUTTON2CLICK :
    {
        static int horzInt = 0;

        horzInt = 4 - horzInt;

        WinSendMsg( WinWindowFromID( hwnd, ID_HCHLB ), HCHLM_SETHORZINT, MPFROMLONG( horzInt ), 0 );

        return MRFROMLONG( TRUE );
    }

    default:            return WinDefWindowProc(hwnd, msg, mp1, mp2);
    } /* endswitch */
}

MRESULT mainwmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PCREATESTRUCT pcs = ( PCREATESTRUCT )mp2;
    HWND hwndHCHLB;
    int pos, count;

    hwndHCHLB = WinCreateWindow( hwnd, WC_HCHLB, NULL, WS_VISIBLE | WS_SYNCPAINT,
                     0, 0, pcs->cx, pcs->cy,
                     hwnd, HWND_TOP, ID_HCHLB, NULL, NULL );

    WinSetFocus( HWND_DESKTOP, hwndHCHLB );

    if( hch_hg2hjpos( 49889, &pos, &count ) == 0 )
    {
        int i;

        for( i = 0; i < count; i ++, pos ++ )
            WinSendMsg( hwndHCHLB, HCHLM_INSERT,
                        MPFROMSHORT( HCHLIT_END ), MPFROMSHORT( hch_pos2hj( pos )));
    }

    return MRFROMLONG(FALSE);
}

MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    WinDestroyWindow( WinWindowFromID( hwnd, ID_HCHLB ));
    WinPostMsg(hwnd,WM_QUIT,0L,0L);
    return 0L;
}


