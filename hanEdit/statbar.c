#define INCL_PM
#include <os2.h>
#include <stdio.h>
#ifdef DEBUG
#include <assert.h>
#endif

#include "../hanlib/han.h"
#include "../HMLE/HMLE.h"
#include "statbar.h"

#define WINWORD_INSTANCE    0

typedef struct {
    ULONG inputmode;
    ULONG insertmode;
    ULONG kbdmode;
    ULONG modified;
    ULONG eol_type;
    ULONG han_type;
    ULONG ln;
    ULONG col;
} STATBARINSTANCE,*PSTATBARINSTANCE;

MRESULT APIENTRY StatbarWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT statbar_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT statbar_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT statbar_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT statbar_usermSetHanMode(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT statbar_usermSetModified(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT statbar_usermSetKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT statbar_usermSetEolType(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT statbar_usermSetHanType(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT statbar_usermSetLineCol(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT statbar_usermSetInsertMode( HWND hwnd, MPARAM mp1, MPARAM mp2 );


BOOL EXPENTRY RegisterStatbarControl(HAB hab)
{
    BOOL rc;
    rc = WinRegisterClass( hab, WC_STATBAR, StatbarWinProc,
                CS_SIZEREDRAW|CS_SYNCPAINT, 4 );
    return rc;
}

MRESULT APIENTRY StatbarWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

    switch (msg) {
    case WM_CREATE:     return statbar_wmCreate( hwnd, mp1, mp2 );
    case WM_DESTROY:    return statbar_wmDestroy( hwnd, mp1, mp2 );
    case WM_PAINT:      return statbar_wmPaint(hwnd, mp1, mp2 );

    case STATBAR_USERM_SETHANMODE:      return statbar_usermSetHanMode(hwnd,mp1,mp2);
    case STATBAR_USERM_SETMODIFIED:     return statbar_usermSetModified(hwnd,mp1,mp2);
    case STATBAR_USERM_SETKBDTYPE:      return statbar_usermSetKbdType(hwnd,mp1,mp2);
    case STATBAR_USERM_SETEOLTYPE:      return statbar_usermSetEolType(hwnd,mp1,mp2);
    case STATBAR_USERM_SETHANTYPE:      return statbar_usermSetHanType(hwnd,mp1,mp2);
    case STATBAR_USERM_SETLINECOL:      return statbar_usermSetLineCol(hwnd,mp1,mp2);
    case STATBAR_USERM_SETINSERTMODE:   return statbar_usermSetInsertMode( hwnd, mp1, mp2 );

    default:    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT statbar_usermSetHanMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG hanmode = (ULONG) mp1;

    if ((hanmode!=HCH_ENG) && (hanmode!=HCH_HAN)) return 0L;

    pInstance->inputmode = hanmode;

    WinInvalidateRect(hwnd,NULL,FALSE);

    return 0L;
}

MRESULT statbar_usermSetModified(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG modified = (ULONG) mp1;

    if (pInstance->modified != modified)
        {
        pInstance->modified = modified;

        WinInvalidateRect(hwnd,NULL,FALSE);
        }

    return 0L;
}

MRESULT statbar_usermSetKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG kbdmode = (ULONG) mp1;

    pInstance->kbdmode = kbdmode;

    WinInvalidateRect(hwnd,NULL,FALSE);

    return 0L;
}

MRESULT statbar_usermSetEolType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG eol_type = (ULONG) mp1;

    pInstance->eol_type = eol_type;

    WinInvalidateRect(hwnd,NULL,FALSE);

    return 0L;
}

MRESULT statbar_usermSetHanType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG han_type = (ULONG) mp1;

    pInstance->han_type = han_type;

    WinInvalidateRect(hwnd,NULL,FALSE);

    return 0L;
}

MRESULT statbar_usermSetLineCol(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG ln = (ULONG) SHORT1FROMMP(mp1);
ULONG col = (ULONG) SHORT2FROMMP(mp1);

    pInstance->ln = ln + 1;
    pInstance->col = col + 1;

    WinInvalidateRect(hwnd,NULL,FALSE);

    return 0L;
}

MRESULT statbar_usermSetInsertMode( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PSTATBARINSTANCE pInstance = WinQueryWindowPtr( hwnd, WINWORD_INSTANCE );
    ULONG insertmode = ( ULONG ) mp1;

    pInstance->insertmode = insertmode;

    WinInvalidateRect( hwnd, NULL, FALSE );

    return 0L;
}

MRESULT statbar_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PSTATBARINSTANCE pInstance;

//  printf("STATBAR::WM_CREATE\n");

    if (DosAllocMem((PPVOID)&pInstance,sizeof(STATBARINSTANCE),fALLOC))
        return MRFROMLONG(TRUE);

    pInstance->inputmode = HCH_ENG;
    pInstance->insertmode = HAN_INSERT;
    pInstance->kbdmode = HAN_KBD_2;
    pInstance->modified = FALSE;
    pInstance->eol_type = HMLE_EOL_CRLF;
    pInstance->han_type = HMLE_HAN_KS;
    pInstance->ln = 1;
    pInstance->col = 1;

    WinSetWindowPtr(hwnd,WINWORD_INSTANCE,pInstance);

    return FALSE;
}

MRESULT statbar_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

//  printf("STATBAR::WM_DESTROY\n");
    DosFreeMem(pInstance);

    return FALSE;
}

MRESULT statbar_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PSTATBARINSTANCE pInstance = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

HPS     hps;
RECTL   rectlEntire,rectlUpdate;

char    str[100];

POINTL p;
//  printf("STATBAR::WM_PAINT\n");

    hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);
#ifdef DEBUG
    assert(hps!=NULLHANDLE);
#endif

//    WinFillRect(hps,&rectlUpdate,SYSCLR_DIALOGBACKGROUND);

    WinQueryWindowRect(hwnd,&rectlEntire);

    GpiSetColor(hps,SYSCLR_BUTTONDARK);
    p.x = rectlEntire.xRight - 1;
    p.y = rectlEntire.yTop-1;
    GpiMove(hps,&p);
    p.x = rectlEntire.xLeft;
    GpiLine(hps,&p);
    p.y --;
    GpiLine(hps, &p);

    GpiSetColor(hps,SYSCLR_BUTTONLIGHT);
    p.x ++;
    GpiMove(hps,&p);
    p.x = rectlEntire.xRight - 1;
    GpiLine(hps,&p);
    p.y ++;
    GpiLine( hps, &p );

    GpiSetBackMix( hps, BM_OVERPAINT );

    GpiSetColor(hps,CLR_BLACK);
    GpiSetBackColor( hps, SYSCLR_DIALOGBACKGROUND );

    if (pInstance->inputmode == HCH_ENG)
        {
        sprintf(str,"영문   ");
        } else {
        switch (pInstance->kbdmode) {
        case HAN_KBD_2:
            sprintf(str,"한글2  ");
            break;
        case HAN_KBD_390:
            sprintf(str,"한글390");
            break;
        case HAN_KBD_3FINAL:
            sprintf(str,"한글3F ");
            break;
        } // switch
        }

    hch_ks2systr(str);
    HanOut(hps,0,0,str);

    rectlUpdate.yTop -= 2;
    rectlUpdate.xLeft = 8 * 7;
    rectlUpdate.xRight = 60;
    WinFillRect( hps, &rectlUpdate, SYSCLR_DIALOGBACKGROUND );

    if (pInstance->modified)
        HanOut(hps,60,0,"*");
        else
        HanOut(hps,60,0," ");

    rectlUpdate.xLeft = 60 + 8 * 1;
    rectlUpdate.xRight = 120;
    WinFillRect( hps, &rectlUpdate, SYSCLR_DIALOGBACKGROUND );

    sprintf(str,"L %-5d  C %-3d    ",pInstance->ln,pInstance->col);
    HanOut(hps,120,0,str);

    rectlUpdate.xLeft = 120 + 18 * 8;
    rectlUpdate.xRight = rectlEntire.xRight - 136;
    WinFillRect( hps, &rectlUpdate, SYSCLR_DIALOGBACKGROUND );

    sprintf( str, ( pInstance->insertmode == HAN_INSERT ) ? "끼움" : "겹침" );
    hch_ks2systr( str );
    HanOut( hps, rectlEntire.xRight - 136, 0, str );

    rectlUpdate.xLeft = rectlUpdate.xRight + 8 * 4;
    rectlUpdate.xRight = rectlEntire.xRight - 96;
    WinFillRect( hps, &rectlUpdate, SYSCLR_DIALOGBACKGROUND );

    if (pInstance->han_type == HMLE_HAN_KS)
        sprintf(str,"완성");
        else
        sprintf(str,"조합");
    hch_ks2systr(str);
    HanOut(hps,rectlEntire.xRight-96,0,str);

    rectlUpdate.xLeft = rectlUpdate.xRight + 8 * 4;
    rectlUpdate.xRight = rectlEntire.xRight - 56;
    WinFillRect( hps, &rectlUpdate, SYSCLR_DIALOGBACKGROUND );

    if (pInstance->eol_type == HMLE_EOL_CRLF)
        HanOut(hps,rectlEntire.xRight-56,0,"CR/LF");
        else
        HanOut(hps,rectlEntire.xRight-56,0,"LF   ");

    rectlUpdate.xLeft = rectlUpdate.xRight + 8 * 5;
    rectlUpdate.xRight = rectlEntire.xRight;
    WinFillRect( hps, &rectlUpdate, SYSCLR_DIALOGBACKGROUND );

    WinEndPaint( hps );

    return MRFROMLONG(0L);
}

