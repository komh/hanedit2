#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

#include "hchlb_internal.h"

static MRESULT hchlb_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_wmVscroll( HWND hwnd, MPARAM mp1, MPARAM mp2 );

static MRESULT hchlb_umQueryCount( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umDeleteAll( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umDelete( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umInsert( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umQueryHch( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umQuerySelection( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umQueryTopIndex( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umSetTopIndex( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umSearchHch( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umSelectItem( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umQueryVertInt( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umSetVertInt( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umQueryHorzInt( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umSetHorzInt( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlb_umRefresh( HWND hwnd, MPARAM mp1, MPARAM mp2 );

static void refreshClient( HWND hwnd );
static void refreshScrollBar( HWND hwnd );
static void notify( HWND hwnd, USHORT notifyCode, MPARAM mp2 );

BOOL RegisterHanCharListBoxControl( HAB hab )
{
    if( !WinRegisterClass( hab, WC_HCHLB, HCHLBWndProc,
                           CS_SYNCPAINT | CS_SIZEREDRAW, sizeof( PVOID )))
        return FALSE;

    if( !WinRegisterClass( hab, WC_HCHLB_CLIENT, HCHLBClientWndProc,
                           CS_SYNCPAINT | CS_SIZEREDRAW, sizeof( PVOID )))
        return FALSE;

    return TRUE;
}

MRESULT EXPENTRY HCHLBWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
        case WM_CREATE   : return hchlb_wmCreate( hwnd, mp1, mp2 );
        case WM_DESTROY  : return hchlb_wmDestroy( hwnd, mp1, mp2 );
        case WM_SIZE     : return hchlb_wmSize( hwnd, mp1, mp2 );
        case WM_SETFOCUS : return hchlb_wmSetFocus( hwnd, mp1, mp2 );
        case WM_PAINT    : return hchlb_wmPaint( hwnd, mp1, mp2 );
        case WM_VSCROLL  : return hchlb_wmVscroll( hwnd, mp1, mp2 );

        case HCHLM_QUERYCOUNT     : return hchlb_umQueryCount( hwnd, mp1, mp2 );
        case HCHLM_DELETEALL      : return hchlb_umDeleteAll( hwnd, mp1, mp2 );
        case HCHLM_DELETE         : return hchlb_umDelete( hwnd, mp1, mp2 );
        case HCHLM_INSERT         : return hchlb_umInsert( hwnd, mp1, mp2 );
        case HCHLM_QUERYHCH       : return hchlb_umQueryHch( hwnd, mp1, mp2 );
        case HCHLM_QUERYSELECTION : return hchlb_umQuerySelection( hwnd, mp1, mp2 );
        case HCHLM_QUERYTOPINDEX  : return hchlb_umQueryTopIndex( hwnd, mp1, mp2 );
        case HCHLM_SETTOPINDEX    : return hchlb_umSetTopIndex( hwnd, mp1, mp2 );
        case HCHLM_SEARCHHCH      : return hchlb_umSearchHch( hwnd, mp1, mp2 );
        case HCHLM_SELECTITEM     : return hchlb_umSelectItem( hwnd, mp1, mp2 );
        case HCHLM_QUERYVERTINT   : return hchlb_umQueryVertInt( hwnd, mp1, mp2 );
        case HCHLM_SETVERTINT     : return hchlb_umSetVertInt( hwnd, mp1, mp2 );
        case HCHLM_QUERYHORZINT   : return hchlb_umQueryHorzInt( hwnd, mp1, mp2 );
        case HCHLM_SETHORZINT     : return hchlb_umSetHorzInt( hwnd, mp1, mp2 );
        case HCHLM_REFRESH        : return hchlb_umRefresh( hwnd, mp1, mp2 );

        default         : return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT hchlb_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PCREATESTRUCT pcs = ( PCREATESTRUCT )mp2;
    PHCHLB hchlb;
    int x, y;
    int cx, cy;
    LONG sv_cxVscroll = WinQuerySysValue( HWND_DESKTOP, SV_CXVSCROLL );

    WinSetWindowPtr( hwnd, 0, NULL );

    hchlb = malloc( sizeof( HCHLB ));
    if( hchlb == NULL )
        return MRFROMLONG( TRUE );

    hchlb->hchCol = HCHColCreate();
    hchlb->topIndex = 0;
    hchlb->bottomIndex = 0;
    hchlb->curIndex = 0;
    hchlb->horzInt = 0;
    hchlb->vertInt = 0;
    hchlb->fgColor = SYSCLR_WINDOWTEXT;
    hchlb->bgColor = SYSCLR_ENTRYFIELD;
    hchlb->eraseBg = TRUE;
    WinSetWindowPtr( hwnd, 0, hchlb );

    x = HCH_BORDER;
    y = HCH_BORDER;
    cx = pcs->cx - ( HCH_BORDER * 2 ) - sv_cxVscroll;
    cy = pcs->cy - ( HCH_BORDER * 2 );

    hchlb->hwndHCHLB = hwnd;
    hchlb->hwndClient = WinCreateWindow(
                            hwnd, WC_HCHLB_CLIENT, NULL,
                            WS_VISIBLE,
                            x, y, cx, cy,
                            hwnd, HWND_TOP,
                            HCHLBID_CLIENT, hchlb, NULL );

    WinCreateWindow( hwnd, WC_SCROLLBAR, NULL,
                     WS_VISIBLE | SBS_VERT,
                     x + cx, y, sv_cxVscroll, cy,
                     hwnd, HWND_TOP,
                     HCHLBID_VSCROLL, NULL, NULL );

    return 0;
}

MRESULT hchlb_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    if( hchlb == NULL ) return 0;

    HCHColDestroy( hchlb->hchCol );
    free( hchlb );

    return 0;
}

MRESULT hchlb_wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    int x, y;
    SHORT cx = SHORT1FROMMP( mp2 ), cy = SHORT2FROMMP( mp2 );
    LONG sv_cxVscroll = WinQuerySysValue( HWND_DESKTOP, SV_CXVSCROLL );

    x = HCH_BORDER;
    y = HCH_BORDER;
    cx -= ( HCH_BORDER * 2 ) + sv_cxVscroll;
    cy -= ( HCH_BORDER * 2 );

    WinSetWindowPos( WinWindowFromID( hwnd, HCHLBID_CLIENT ), HWND_TOP,
                     x, y, cx, cy,
                     SWP_MOVE | SWP_SIZE );

    WinSetWindowPos( WinWindowFromID( hwnd, HCHLBID_VSCROLL ), HWND_TOP,
                     x + cx, y, sv_cxVscroll, cy,
                     SWP_MOVE | SWP_SIZE );

    return 0;
}

MRESULT hchlb_wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    if( SHORT1FROMMP( mp2 ))
        WinPostMsg( hchlb->hwndClient, HCHLMC_SETFOCUS, 0, 0 );

    return 0;
}

MRESULT hchlb_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HPS     hps;
    RECTL   rcl;
    POINTL  ptl;

    hps = WinBeginPaint( hwnd, NULLHANDLE, &rcl );
    WinQueryWindowRect( hwnd, &rcl );

    GpiSetColor( hps, SYSCLR_BUTTONDARK);

    ptl.x = rcl.xRight - 1;
    ptl.y = rcl.yTop - 1;
    GpiMove( hps, &ptl );

    ptl.x = rcl.xLeft;
    GpiLine( hps, &ptl );

    ptl.y = rcl.yBottom;
    GpiLine( hps, &ptl );

    GpiSetColor( hps, SYSCLR_BUTTONLIGHT );

    ptl.x = rcl.xLeft + 1;
    GpiMove( hps, &ptl );

    ptl.x = rcl.xRight - 1;
    GpiLine( hps, &ptl );

    ptl.y = rcl.yTop - 1;
    GpiLine( hps, &ptl );

    WinEndPaint( hps );

    return 0;
}

MRESULT hchlb_wmVscroll( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB  hchlb = WinQueryWindowPtr( hwnd, 0 );
    USHORT  cmd = SHORT2FROMMP( mp2 );
    USHORT  pos = SHORT1FROMMP( mp2 );

    switch (cmd)
    {
        case SB_LINEUP:
            HCHLBLineUp( hchlb );
            break;

        case SB_LINEDOWN:
            HCHLBLineDown( hchlb );
            break;

        case SB_PAGEUP:
            HCHLBPageUp( hchlb );
            break;

        case SB_PAGEDOWN:
            HCHLBPageDown( hchlb );
            break;

        case SB_SLIDERTRACK:
            HCHLBSetLine( hchlb, pos );
            break;

        case SB_SLIDERPOSITION:
            HCHLBSetLine( hchlb, pos );
            break;
    }

    //if( cmd != SB_SLIDERTRACK )
        //WinPostMsg( hchlb->hwndClient, HCHLMC_SETFOCUS, 0, 0 );

    return 0L;
}

MRESULT hchlb_umQueryCount( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    if( hchlb == NULL )
        return 0;

    return MRFROMLONG( HCHColQueryCount( hchlb->hchCol ));
}

MRESULT hchlb_umDeleteAll( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    LONG   rc;

    if( hchlb == NULL )
        return 0;

    if(( rc = HCHColDeleteAll( hchlb->hchCol )) == 0 )
    {
        hchlb->topIndex = hchlb->bottomIndex = hchlb->curIndex = 0;
        notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( HCHLIT_NONE, FALSE ));
    }

    WinSendMsg( hwnd, HCHLM_REFRESH, 0, 0 );

    return MRFROMLONG( rc == 0 );
}

MRESULT hchlb_umDelete( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT  index = SHORT1FROMMP( mp1 );
    LONG   rc;

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR );

    if(( rc = HCHColDelete( hchlb->hchCol, index )) >= 0 )
    {
        if( hchlb->topIndex >= hchlb->hchCol->count )
            hchlb->topIndex = hchlb->hchCol->count - 1;

        if( hchlb->curIndex >= hchlb->hchCol->count )
        {
            notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
            hchlb->curIndex = hchlb->hchCol->count - 1;
            notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));
        }

        WinSendMsg( hchlb->hwndHCHLB, HCHLM_SETTOPINDEX, MPFROMSHORT( hchlb->topIndex ), 0 );
    }

//    WinSendMsg( hwnd, HCHLM_REFRESH, MPFROMLONG( TRUE ), 0 );

    return MRFROMLONG( rc );
}

MRESULT hchlb_umInsert( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB  hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT   index = SHORT1FROMMP( mp1 );
    HANCHAR hch = SHORT1FROMMP( mp2 );
    LONG    rc;

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR);

    rc = HCHColInsert( hchlb->hchCol, index, hch );
    WinSendMsg( hchlb->hwndHCHLB, HCHLM_SETTOPINDEX, MPFROMSHORT( hchlb->topIndex ), 0 );

    return MRFROMLONG( rc );

}

MRESULT hchlb_umQueryHch( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT  index = SHORT1FROMMP( mp1 );

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR);

    return MRFROMLONG( HCHColQueryHch( hchlb->hchCol, index ));
}

MRESULT hchlb_umQuerySelection( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
//    SHORT  index = SHORT1FROMMP( mp1 );

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR);

    return MRFROMSHORT( hchlb->curIndex );
}

MRESULT hchlb_umQueryTopIndex( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR);

    return MRFROMSHORT( hchlb->topIndex );
}

MRESULT hchlb_umSetTopIndex( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT  index = SHORT1FROMMP( mp1 );
    int    topLineN, bottomLineN;

    if( hchlb == NULL )
        return 0;

    if(( index < 0 ) || ( index >= HCHColQueryCount( hchlb->hchCol )))
        return MRFROMLONG( HCHLIT_INDEXERROR );

    index = ( index / hchlb->xChars ) * hchlb->xChars;

    if( hchlb->hchCol->count - index < hchlb->xChars * hchlb->yChars )
    {
        bottomLineN = ( hchlb->hchCol->count - 1 ) / hchlb->xChars;
        topLineN = bottomLineN - hchlb->yChars + 1;
        if( topLineN < 0 )
            topLineN = 0;

        index = topLineN * hchlb->xChars;
    }

    hchlb->topIndex = index;
    hchlb->bottomIndex = hchlb->topIndex + hchlb->xChars * hchlb->yChars - 1;
    if( hchlb->bottomIndex >= hchlb->hchCol->count )
        hchlb->bottomIndex = hchlb->hchCol->count - 1;

    WinSendMsg( hwnd, HCHLM_REFRESH, 0, 0 );

    return MRFROMLONG( TRUE );
}

MRESULT hchlb_umSearchHch( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB  hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT   cmd = SHORT1FROMMP( mp1 );
    SHORT   index = SHORT2FROMMP( mp1 );
    HANCHAR hch = SHORT1FROMMP( mp2 );

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR);

    return MRFROMLONG( HCHColSearchHch( hchlb->hchCol, cmd, index, hch ));
}

MRESULT hchlb_umSelectItem( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT  index = SHORT1FROMMP( mp1 );
    BOOL   selectFlag = SHORT1FROMMP( mp2 );

    if( hchlb == NULL )
        return 0;

    if((( index < 0 ) || ( index >= HCHColQueryCount( hchlb->hchCol ))) &&
       ( index != HCHLIT_NONE ))
        return 0;

    notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));

    if( index == HCHLIT_NONE )
        hchlb->curIndex = HCHLIT_NONE;
    else if( selectFlag )
    {
        hchlb->curIndex = index;
        notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));
    }
    else if( hchlb->curIndex == index )
        hchlb->curIndex = HCHLIT_NONE;

    //hchlb->eraseBg = FALSE;
    //WinSendMsg( hwnd, HCHLM_REFRESH, 0, 0 );
    HCHLBRepositionPage( hchlb );

    return MRFROMLONG( TRUE );
}

MRESULT hchlb_umQueryVertInt( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR);

    return MRFROMLONG( hchlb->vertInt );
}

MRESULT hchlb_umSetVertInt( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT  vertInt = SHORT1FROMMP( mp1 );

    if( hchlb == NULL )
        return 0;

    hchlb->vertInt = vertInt;
    hchlb->yChars = ( hchlb->cyClient - hchlb->vertInt - HCH_BORDER * 2 ) /
                    ( HCH_HEIGHT + hchlb->vertInt );
    if( hchlb->yChars <= 0 )
        hchlb->yChars = 1;

    WinSendMsg( hwnd, HCHLM_SETTOPINDEX, MPFROMSHORT( hchlb->topIndex ), 0 );

    return MRFROMLONG( TRUE );
}

MRESULT hchlb_umQueryHorzInt( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    if( hchlb == NULL )
        return MRFROMLONG( HCHLIT_ERROR);

    return MRFROMLONG( hchlb->horzInt );
}

MRESULT hchlb_umSetHorzInt( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT  horzInt = SHORT1FROMMP( mp1 );

    if( hchlb == NULL )
        return 0;

    hchlb->horzInt = horzInt;
    hchlb->xChars = ( hchlb->cxClient - hchlb->horzInt - HCH_BORDER * 2 ) /
                    ( HCH_WIDTH + hchlb->horzInt );
    if( hchlb->xChars <= 0 )
        hchlb->xChars = 1;

    WinSendMsg( hwnd, HCHLM_SETTOPINDEX, MPFROMSHORT( hchlb->topIndex ), 0 );

    return MRFROMLONG( TRUE );
}

MRESULT hchlb_umRefresh( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    refreshClient( hwnd );
    refreshScrollBar( hwnd );

    return 0;
}

void refreshClient( HWND hwnd )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    WinSendMsg( hchlb->hwndClient, HCHLMC_REFRESH, 0, 0 );
}

void refreshScrollBar( HWND hwnd )
{
    PHCHLB  hchlb = WinQueryWindowPtr( hwnd, 0 );
    HWND    hwndVscroll = WinWindowFromID( hwnd, HCHLBID_VSCROLL );
    int     totalLines;
    int     topLineN;

    totalLines = ( hchlb->hchCol->count + hchlb->xChars - 1 ) / hchlb->xChars;
    topLineN = hchlb->topIndex / hchlb->xChars;

    if( totalLines > hchlb->yChars )
        WinSendMsg( hwndVscroll, SBM_SETSCROLLBAR, MPFROMSHORT( topLineN ),
                    MPFROM2SHORT( 0, totalLines - hchlb->yChars ));
    else
        WinSendMsg( hwndVscroll, SBM_SETSCROLLBAR, MPFROMSHORT( topLineN ),
                    MPFROM2SHORT( 0, totalLines ));

    WinSendMsg( hwndVscroll, SBM_SETTHUMBSIZE, MPFROM2SHORT( hchlb->yChars, totalLines ), 0 );
}

void notify( HWND hwnd, USHORT notifyCode, MPARAM mp2 )
{
    HWND hwndOwner = WinQueryWindow( hwnd, QW_OWNER );
    HWND usId = WinQueryWindowUShort( hwnd, QWS_ID );

    WinPostMsg( hwndOwner, WM_CONTROL, MPFROM2SHORT( usId, notifyCode ), mp2 );
}

void HCHLBRepositionPage( PHCHLB hchlb )
{
    int     curLineN;
    int     topLineN;

    if( hchlb == NULL )
        return;

    if( hchlb->hchCol->count == 0 )
        return;

    if( hchlb->curIndex == HCHLIT_NONE )
        return;

    topLineN = hchlb->topIndex / hchlb->xChars;

    if(( hchlb->curIndex < hchlb->topIndex ) ||
       ( hchlb->curIndex > hchlb->bottomIndex ))
    {
        curLineN = hchlb->curIndex / hchlb->xChars;
        topLineN = curLineN - hchlb->yChars / 2;
    }

    HCHLBSetLine( hchlb, topLineN );
}

void HCHLBLineUp( PHCHLB hchlb )
{
    if( hchlb == NULL )
        return;

    if( hchlb->hchCol->count == 0 )
        return;

    HCHLBSetLine( hchlb, hchlb->topIndex / hchlb->xChars - 1 );
}

void HCHLBLineDown( PHCHLB hchlb )
{
    if( hchlb == NULL )
        return;

    if( hchlb->hchCol->count == 0 )
        return;

    HCHLBSetLine( hchlb, hchlb->topIndex / hchlb->xChars + 1 );
}

void HCHLBPageUp( PHCHLB hchlb )
{
    if( hchlb == NULL )
        return;

    if( hchlb->hchCol->count == 0 )
        return;

    HCHLBSetLine( hchlb, hchlb->topIndex / hchlb->xChars - hchlb->yChars + 1 );
}

void HCHLBPageDown( PHCHLB hchlb )
{
    if( hchlb == NULL )
        return;

    if( hchlb->hchCol->count == 0 )
        return;

    HCHLBSetLine( hchlb, hchlb->topIndex / hchlb->xChars + hchlb->yChars - 1 );
}

void HCHLBSetLine( PHCHLB hchlb, int n )
{
    int limitLineN;

    if( hchlb == NULL )
        return;

    if( hchlb->hchCol->count == 0 )
        return;

    limitLineN = ( hchlb->hchCol->count - 1 ) / hchlb->xChars - hchlb->yChars + 1;
    if( n > limitLineN )
        n = limitLineN;

    if( n < 0 )
        n = 0;

    hchlb->eraseBg = FALSE;
/*
    if(( hchlb->topIndex / hchlb->xChars ) == n )
        WinSendMsg( hchlb->hwndHCHLB, HCHLM_REFRESH, 0, 0 );
    else
*/
        WinSendMsg( hchlb->hwndHCHLB, HCHLM_SETTOPINDEX, MPFROMSHORT( n * hchlb->xChars ), 0 );
}


