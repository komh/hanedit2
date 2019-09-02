#define INCL_DOSMEMMGR
#define INCL_PM
#include <os2.h>

#include <stdio.h>

#include "../hanlib/han.h"
#include "hchlb.h"

#define CATEGORY_NUMBER     7

static PSCSELINFO pscselInfo = NULL;
static SCSELINFO  scselInfo = { 0, 0, 0 };

static PSZ apszCategory[ CATEGORY_NUMBER ] =
    { "도형문자", "전각영문,한글낱자(1)", "로마숫자,괘선조각",
      "단위기호,라틴(1)", "라틴(2),일본(1)", "일본(2),러시아",
      "한글낱자(2)" };
static LBOXINFO lbInfo = { LIT_END, CATEGORY_NUMBER, 0, 0 };

static MRESULT scselDlg_wmInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT scselDlg_wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT scselDlg_wmDrawItem( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT scselDlg_wmMeasureItem( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT scselDlg_wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );

HANCHAR scselDlg( HWND hwndParent, HWND hwndOwner, HMODULE hmod, PSCSELINFO pscselInfo )
{
    HWND    hwndDlg;
    LONG    rc;

    //rc = WinDlgBox( hwndParent, hwndOwner, &scselDlgProc, hmod, IDD_SPECIALCHARSEL, pscselInfo );
    hwndDlg = WinLoadDlg( hwndParent, hwndOwner, &scselDlgProc, hmod, IDD_SPECIALCHARSEL, pscselInfo );
    rc = WinProcessDlg( hwndDlg );
    WinDestroyWindow( hwndDlg );

    if( rc == DID_CANCEL )
        pscselInfo->hch = HCH_SINGLE_SPACE;

    return pscselInfo->hch;
}

MRESULT EXPENTRY scselDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
        case WM_INITDLG     : return scselDlg_wmInitDlg( hwnd, mp1, mp2 );
        case WM_CONTROL     : return scselDlg_wmControl( hwnd, mp1, mp2 );
        case WM_DRAWITEM    : return scselDlg_wmDrawItem( hwnd, mp1, mp2 );
        case WM_MEASUREITEM : return scselDlg_wmMeasureItem( hwnd, mp1, mp2 );
        case WM_COMMAND     : return scselDlg_wmCommand( hwnd, mp1, mp2 );

        default : return WinDefDlgProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT scselDlg_wmInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HWND    hwndLB = WinWindowFromID( hwnd, IDLB_SCCATEGORY );
    HWND    hwndHCHLB = WinWindowFromID( hwnd, IDHCHLB_SPECIALCHARSEL );
    LONG    cxScreen, cyScreen;
    BOOL    usePrev = FALSE;
    RECTL   rcl;

    pscselInfo = PVOIDFROMMP( mp2 );
    if( pscselInfo->category == -1 )
    {
        usePrev = TRUE;
        pscselInfo->category = scselInfo.category;
        pscselInfo->index = scselInfo.index;
    }

    WinSendMsg( hwndLB, LM_INSERTMULTITEMS, MPFROMP( &lbInfo ), MPFROMP( apszCategory ));
    WinSendMsg( hwndLB, LM_SELECTITEM, MPFROMLONG( pscselInfo->category ), MPFROMLONG( TRUE ));

    WinSendMsg( hwndHCHLB, HCHLM_SETHORZINT, MPFROMSHORT( 4 ), 0 );
    WinSendMsg( hwndHCHLB, HCHLM_SETVERTINT, MPFROMSHORT( 4 ), 0 );

    if( usePrev )
        WinSendMsg( hwndHCHLB, HCHLM_SELECTITEM, MPFROMSHORT( pscselInfo->index ),
                    MPFROMLONG( TRUE ));

    cxScreen = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
    cyScreen = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

    WinQueryWindowRect( hwnd, &rcl );

    WinSetWindowPos( hwnd, HWND_TOP,
                     ( cxScreen - ( rcl.xRight - rcl.xLeft )) / 2,
                     ( cyScreen - ( rcl.yTop - rcl.yBottom )) / 2,
                     0, 0, SWP_MOVE );

    return 0;
}

MRESULT scselDlg_wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    USHORT  id = SHORT1FROMMP( mp1 );
    USHORT  notifyCode = SHORT2FROMMP( mp1 );

    switch( id )
    {
        case IDHCHLB_SPECIALCHARSEL :
        {
            HWND hwndHCHLB = WinWindowFromID( hwnd, IDHCHLB_SPECIALCHARSEL );
            SHORT index = SHORT1FROMMP( mp2 );

            if(( notifyCode == HCHLN_ENTER ) && ( index != HCHLIT_NONE ))
            {
                pscselInfo->index = scselInfo.index = index;
                pscselInfo->hch = scselInfo.hch =
                    SHORT1FROMMR( WinSendMsg( hwndHCHLB, HCHLM_QUERYHCH,
                                                MPFROMSHORT( index ), 0 ));

                WinDismissDlg( hwnd, DID_OK );
            }
            break;
        }

        case IDLB_SCCATEGORY :
        {
            HWND hwndLB = HWNDFROMMP( mp2 );

            if(( notifyCode == LN_ENTER ) || ( notifyCode == LN_SELECT ))
            {
                HWND hwndHCHLB = WinWindowFromID( hwnd, IDHCHLB_SPECIALCHARSEL );
                int  index;
                int  i;

                index = SHORT1FROMMR( WinSendMsg( hwndLB, LM_QUERYSELECTION, 0, 0 ));
                if( index == LIT_NONE )
                    break;

                pscselInfo->category = scselInfo.category = index;

                WinEnableWindowUpdate( hwndHCHLB, FALSE );

                WinSendMsg( hwndHCHLB, HCHLM_DELETEALL, 0, 0 );

                for( i = 0x31; i <= 0x7E; i ++ )
                    WinSendMsg( hwndHCHLB, HCHLM_INSERT,
                        MPFROMSHORT( HCHLIT_END ),
                        MPFROMSHORT( HCHFROM2CH( 0xD9 + pscselInfo->category, i )));

                for( i = 0x91; i <= 0xFE; i ++ )
                    WinSendMsg( hwndHCHLB, HCHLM_INSERT,
                        MPFROMSHORT( HCHLIT_END ),
                        MPFROMSHORT( HCHFROM2CH( 0xD9 + pscselInfo->category, i )));

                WinEnableWindowUpdate( hwndHCHLB, TRUE );
            }
        }
        break;
    }

    return 0;
}

MRESULT scselDlg_wmDrawItem( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    //USHORT      id = SHORT1FROMMP( mp1 );
    POWNERITEM  poi = ( POWNERITEM )mp2;
    LONG        fgColor, bgColor;
    int         maxLen = SHORT1FROMMR( WinSendMsg( poi->hwnd, LM_QUERYITEMTEXTLENGTH,
                                        MPFROMSHORT( poi->idItem ), 0 ));
    char        *str;

    DosAllocMem(( PPVOID )&str, maxLen + 1, fALLOC );
    if( str == NULL )
        return MRFROMLONG( FALSE );

    WinQueryLboxItemText( poi->hwnd, poi->idItem, str, maxLen + 1 );
    hch_ks2systr( str );

    fgColor = GpiQueryColor( poi->hps );
    bgColor = GpiQueryBackColor( poi->hps );

    if( poi->fsState )
    {
        GpiSetColor( poi->hps, bgColor );
        GpiSetBackColor( poi->hps, fgColor );
    }
    else
    {
        GpiSetColor( poi->hps, fgColor );
        GpiSetBackColor( poi->hps, bgColor );
    }

    WinFillRect( poi->hps, &poi->rclItem, poi->fsState ? fgColor : bgColor );
    HanOut( poi->hps, poi->rclItem.xLeft, poi->rclItem.yBottom, str );
    poi->fsState = poi->fsStateOld = FALSE;

    DosFreeMem( str );

    return MRFROMLONG( TRUE );
}


MRESULT scselDlg_wmMeasureItem( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    return MRFROM2SHORT( 16, 0 );
}

MRESULT scselDlg_wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HWND hwndHCHLB;
    SHORT index;

    if(( SHORT1FROMMP( mp1 ) == DID_OK ) || ( SHORT1FROMMP( mp1 ) == DID_CANCEL ))
    {
        hwndHCHLB = WinWindowFromID( hwnd, IDHCHLB_SPECIALCHARSEL );
        index = SHORT1FROMMR( WinSendMsg( hwndHCHLB, HCHLM_QUERYSELECTION, 0, 0 ));

        if( index != HCHLIT_NONE )
        {
            pscselInfo->index = index;
            pscselInfo->hch = scselInfo.hch =
                SHORT1FROMMR( WinSendMsg( hwndHCHLB, HCHLM_QUERYHCH,
                                          MPFROMSHORT( index ), 0 ));

            if( SHORT1FROMMP( mp1 ) == DID_OK )
                WinDismissDlg( hwnd, DID_OK );
        }

        if( SHORT1FROMMP( mp1 ) == DID_CANCEL )
            WinDismissDlg( hwnd, DID_CANCEL );
    }
    else
        return WinDefDlgProc( hwnd, WM_COMMAND, mp1, mp2 );

    return 0;
}

