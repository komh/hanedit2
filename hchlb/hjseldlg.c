#define INCL_PM
#include <os2.h>

#include <stdio.h>

#include "../hanlib/han.h"
#include "hchlb.h"

static HANCHAR *hch = NULL;

static MRESULT hjselDlg_wmInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hjselDlg_wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hjselDlg_wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );

HANCHAR hjselDlg( HWND hwndParent, HWND hwndOwner, HMODULE hmod, HANCHAR hch )
{
    HWND    hwndDlg;
    LONG    rc;

    //rc = WinDlgBox( hwndParent, hwndOwner, &hjselDlgProc, hmod, IDD_HANJASEL, &hch );
    hwndDlg = WinLoadDlg( hwndParent, hwndOwner, &hjselDlgProc, hmod, IDD_HANJASEL, &hch );
    rc = WinProcessDlg( hwndDlg );
    WinDestroyWindow( hwndDlg );

    if( rc == DID_CANCEL )
        hch = HCH_SINGLE_SPACE;

    return hch;
}

MRESULT EXPENTRY hjselDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
        case WM_INITDLG : return hjselDlg_wmInitDlg( hwnd, mp1, mp2 );
        case WM_CONTROL : return hjselDlg_wmControl( hwnd, mp1, mp2 );
        case WM_COMMAND : return hjselDlg_wmCommand( hwnd, mp1, mp2 );

        default : return WinDefDlgProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT hjselDlg_wmInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HWND    hwndHCHLB = WinWindowFromID( hwnd, IDHCHLB_HANJASEL );
    LONG    cxScreen, cyScreen;
    RECTL   rcl;
    int     pos, count;

    hch = PVOIDFROMMP( mp2 );

    WinSendMsg( hwndHCHLB, HCHLM_SETHORZINT, MPFROMSHORT( 4 ), 0 );
    WinSendMsg( hwndHCHLB, HCHLM_SETVERTINT, MPFROMSHORT( 4 ), 0 );

    WinSendMsg( hwndHCHLB, HCHLM_INSERT,
                MPFROMSHORT( HCHLIT_END ), MPFROMSHORT( *hch ));

    if( hch_hg2hjpos( *hch, &pos, &count ) == 0 )
    {
        int i;

        for( i = 0; i < count; i ++, pos ++ )
            WinSendMsg( hwndHCHLB, HCHLM_INSERT,
                        MPFROMSHORT( HCHLIT_END ), MPFROMSHORT( hch_pos2hj( pos )));
    }

    cxScreen = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
    cyScreen = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

    WinQueryWindowRect( hwnd, &rcl );

    WinSetWindowPos( hwnd, HWND_TOP,
                     ( cxScreen - ( rcl.xRight - rcl.xLeft )) / 2,
                     ( cyScreen - ( rcl.yTop - rcl.yBottom )) / 2,
                     0, 0, SWP_MOVE );

    return 0;
}

MRESULT hjselDlg_wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    USHORT  id = SHORT1FROMMP( mp1 );
    USHORT  notifyCode = SHORT2FROMMP( mp1 );

    if( id == IDHCHLB_HANJASEL )
    {
        HWND hwndHCHLB = WinWindowFromID( hwnd, IDHCHLB_HANJASEL );
        SHORT index = SHORT1FROMMP( mp2 );

        if(( notifyCode == HCHLN_ENTER ) && ( index != HCHLIT_NONE ))
        {
            *hch = SHORT1FROMMR( WinSendMsg( hwndHCHLB, HCHLM_QUERYHCH,
                                             MPFROMSHORT( index ), 0 ));

            WinDismissDlg( hwnd, DID_OK );
        }
    }

    return 0;
}

MRESULT hjselDlg_wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HWND hwndHCHLB;
    SHORT index;

    if( SHORT1FROMMP( mp1 ) == DID_OK )
    {
        hwndHCHLB = WinWindowFromID( hwnd, IDHCHLB_HANJASEL );
        index = SHORT1FROMMR( WinSendMsg( hwndHCHLB, HCHLM_QUERYSELECTION, 0, 0 ));

        if( index != HCHLIT_NONE )
        {
            *hch = SHORT1FROMMR( WinSendMsg( hwndHCHLB, HCHLM_QUERYHCH,
                                             MPFROMSHORT( index ), 0 ));

            WinDismissDlg( hwnd, DID_OK );
        }
    }
    else
        return WinDefDlgProc( hwnd, WM_COMMAND, mp1, mp2 );

    return 0;
}

