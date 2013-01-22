#define INCL_PM
#include <os2.h>
#include <stdio.h>

#include "hchlb_internal.h"
#include "../hanlib/han.h"

static MRESULT hchlbc_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmChar( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmButton1Down( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmButton2Down( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmButton1Click( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmButton1DblClk( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );

static MRESULT hchlbc_umRefresh( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hchlbc_umSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 );

static void notify( HWND hwnd, USHORT notifyCode, MPARAM mp2 );
static int pts2idx( PHCHLB hchlb, POINTS *pts );

MRESULT EXPENTRY HCHLBClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
        case WM_CREATE        : return hchlbc_wmCreate( hwnd, mp1, mp2 );
        case WM_DESTROY       : return hchlbc_wmDestroy( hwnd, mp1, mp2 );
        case WM_SIZE          : return hchlbc_wmSize( hwnd, mp1, mp2 );
        case WM_SETFOCUS      : return hchlbc_wmSetFocus( hwnd, mp1, mp2 );
        case WM_CHAR          : return hchlbc_wmChar( hwnd, mp1, mp2 );
        case WM_BUTTON1DOWN   : return hchlbc_wmButton1Down( hwnd, mp1, mp2 );
        case WM_BUTTON2DOWN   : return hchlbc_wmButton2Down( hwnd, mp1, mp2 );
        case WM_BUTTON1CLICK  : return hchlbc_wmButton1Click( hwnd, mp1, mp2 );
        case WM_BUTTON1DBLCLK : return hchlbc_wmButton1DblClk( hwnd, mp1, mp2 );
        case WM_PAINT         : return hchlbc_wmPaint( hwnd, mp1, mp2 );

        case HCHLMC_REFRESH  : return hchlbc_umRefresh( hwnd, mp1, mp2 );
        case HCHLMC_SETFOCUS : return hchlbc_umSetFocus( hwnd, mp1, mp2 );

        default              : return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT hchlbc_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PCREATESTRUCT pcs = ( PCREATESTRUCT )mp2;
    PHCHLB hchlb = ( PHCHLB )mp1;

    hchlb->cxClient = pcs->cx;
    hchlb->cyClient = pcs->cy;

    hchlb->xChars = ( pcs->cx - hchlb->horzInt - HCH_BORDER * 2 ) /
                    ( HCH_WIDTH + hchlb->horzInt );
    if( hchlb->xChars <= 0 )
        hchlb->xChars = 1;

    hchlb->yChars = ( pcs->cy - hchlb->vertInt - HCH_BORDER * 2 ) /
                    ( HCH_HEIGHT + hchlb->vertInt );
    if( hchlb->yChars <= 0 )
        hchlb->yChars = 1;

    WinSetWindowPtr( hwnd, 0, hchlb );

    return 0;
}

MRESULT hchlbc_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    //PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    return 0;
}

MRESULT hchlbc_wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    SHORT cx = SHORT1FROMMP( mp2 ), cy = SHORT2FROMMP( mp2 );

    hchlb->cxClient = cx;
    hchlb->cyClient = cy;

    hchlb->xChars = ( cx - hchlb->horzInt - HCH_BORDER * 2 ) /
                    ( HCH_WIDTH + hchlb->horzInt );
    if( hchlb->xChars <= 0 )
        hchlb->xChars = 1;

    hchlb->yChars = ( cy - hchlb->vertInt - HCH_BORDER * 2 ) /
                    ( HCH_HEIGHT + hchlb->vertInt );
    if( hchlb->yChars <= 0 )
        hchlb->yChars = 1;

    WinSendMsg( hchlb->hwndHCHLB, HCHLM_SETTOPINDEX, MPFROMSHORT( hchlb->topIndex ), 0 );

    return 0;
}

MRESULT hchlbc_wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    USHORT  focus = SHORT1FROMMP( mp2 );

    notify( hwnd, focus ? HCHLN_SETFOCUS : HCHLN_KILLFOCUS, MPFROMHWND( hwnd ));

    return 0;
}


MRESULT hchlbc_wmChar( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB  hchlb = WinQueryWindowPtr( hwnd, 0 );
    USHORT  fsFlags = SHORT1FROMMP(mp1);
    UCHAR   ucVkey  = CHAR3FROMMP(mp2);
//  UCHAR   ucChar  = CHAR1FROMMP(mp2);
//  UCHAR   ucScancode = CHAR4FROMMP(mp1);

    if(( fsFlags & KC_KEYUP ) || ( fsFlags & !KC_VIRTUALKEY ) || ( ucVkey == VK_TAB ))
        return WinDefWindowProc( hwnd, WM_CHAR, mp1, mp2 );

    switch( ucVkey )
    {
        case VK_LEFT :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                if( hchlb->curIndex > 0 )
                    hchlb->curIndex --;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));

                if( hchlb->curIndex < hchlb->topIndex )
                    HCHLBLineUp( hchlb );

                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_RIGHT :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                if( hchlb->curIndex < ( hchlb->hchCol->count - 1 ))
                    hchlb->curIndex ++;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));

                if( hchlb->curIndex > hchlb->bottomIndex )
                    HCHLBLineDown( hchlb );

                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_UP :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                hchlb->curIndex -= hchlb->xChars;
                if( hchlb->curIndex < 0 )
                    hchlb->curIndex = 0;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));

                if( hchlb->curIndex < hchlb->topIndex )
                    HCHLBLineUp( hchlb );

                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_DOWN :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                hchlb->curIndex += hchlb->xChars;
                if( hchlb->curIndex >= hchlb->hchCol->count )
                    hchlb->curIndex = hchlb->hchCol->count - 1;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));

                if( hchlb->curIndex > hchlb->bottomIndex )
                    HCHLBLineDown( hchlb );

                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_PAGEUP :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                hchlb->curIndex -= hchlb->xChars * ( hchlb->yChars - 1 );
                if( hchlb->curIndex < 0 )
                    hchlb->curIndex = 0;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));

                HCHLBPageUp( hchlb );
                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_PAGEDOWN :
        case VK_PAGEDOWN + 0x90 :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                hchlb->curIndex += hchlb->xChars * ( hchlb->yChars - 1 );
                if( hchlb->curIndex >= hchlb->hchCol->count )
                    hchlb->curIndex = hchlb->hchCol->count - 1;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));

                HCHLBPageDown( hchlb );
                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_HOME :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                hchlb->curIndex = 0;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE ));

                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_END :
            if( hchlb->curIndex != HCHLIT_NONE )
            {
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, FALSE ));
                hchlb->curIndex = hchlb->hchCol->count - 1;
                notify( hwnd, HCHLN_SELECT, MPFROM2SHORT( hchlb->curIndex, TRUE));

                HCHLBRepositionPage( hchlb );
            }
            break;

        case VK_ENTER :
        case VK_NEWLINE :
            if( hchlb->curIndex != HCHLIT_NONE )
                notify( hwnd, HCHLN_ENTER, MPFROMSHORT( hchlb->curIndex ));
            break;

        default :
            return WinDefWindowProc( hwnd, WM_CHAR, mp1, mp2 );
    }

    return MRFROMLONG( TRUE );
}

MRESULT hchlbc_wmButton1Down( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    if( WinQueryFocus( HWND_DESKTOP ) != hwnd )
        WinSetFocus( HWND_DESKTOP, hwnd );

    return MRFROMLONG( TRUE );
}

MRESULT hchlbc_wmButton2Down( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    if( WinQueryFocus( HWND_DESKTOP ) != hwnd )
        WinSetFocus( HWND_DESKTOP, hwnd );

    return MRFROMLONG( TRUE );
}

MRESULT hchlbc_wmButton1Click( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    POINTS pts;
    int index;

    pts.x = SHORT1FROMMP( mp1 );
    pts.y = SHORT2FROMMP( mp1 );

    index = pts2idx( hchlb, &pts );
    if( index != HCHLIT_NONE )
        WinSendMsg( hchlb->hwndHCHLB, HCHLM_SELECTITEM,
                    MPFROMSHORT( hchlb->topIndex + index ), MPFROMLONG( TRUE ));

    return MRFROMLONG( TRUE );
}

MRESULT hchlbc_wmButton1DblClk( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    POINTS pts;
    int index;

    pts.x = SHORT1FROMMP( mp1 );
    pts.y = SHORT2FROMMP( mp1 );

    index = pts2idx( hchlb, &pts );
    if( index != HCHLIT_NONE )
    {
        WinSendMsg( hchlb->hwndHCHLB, HCHLM_SELECTITEM,
                    MPFROMSHORT( hchlb->topIndex + index ), MPFROMLONG( TRUE ));

        notify( hwnd, HCHLN_ENTER, MPFROMSHORT( hchlb->topIndex + index ));
    }

    return MRFROMLONG( TRUE );
}


MRESULT hchlbc_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );
    HPS hps;
    RECTL rectlEntire, rectlUpdate;
    POINTL ptl;
    int x, y, i;

    WinQueryWindowRect( hwnd, &rectlEntire );

    hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);

    if( hchlb->eraseBg )
        WinFillRect( hps, &rectlUpdate, hchlb->bgColor );

    GpiSetColor( hps, SYSCLR_WINDOWTEXT );

    ptl.x = rectlEntire.xRight - 1;
    ptl.y = rectlEntire.yTop - 1;
    GpiMove( hps, &ptl );

    ptl.x = rectlEntire.xLeft;
    GpiLine( hps, &ptl );

    ptl.y = rectlEntire.yBottom;
    GpiLine( hps, &ptl );

    GpiSetColor( hps, SYSCLR_BUTTONMIDDLE );

    ptl.x = rectlEntire.xLeft + 1;
    GpiMove( hps, &ptl );

    ptl.x = rectlEntire.xRight - 1;
    GpiLine( hps, &ptl );

    ptl.y = rectlEntire.yTop - 1;
    GpiLine( hps, &ptl );

    GpiSetBackMix( hps, BM_OVERPAINT );

    x = HCH_BORDER + hchlb->horzInt;
    y = rectlEntire.yTop - HCH_BORDER;

    for( i = hchlb->topIndex; i <= hchlb->bottomIndex; i ++ )
    {
        if(( i - hchlb->topIndex ) % hchlb->xChars == 0 )
        {
            x = HCH_BORDER + hchlb->horzInt;
            y -= ( HCH_HEIGHT + hchlb->vertInt );
        }

        if( i == hchlb->curIndex )
        {
            GpiSetColor( hps, hchlb->bgColor );
            GpiSetBackColor( hps, hchlb->fgColor );
        }
        else
        {
            GpiSetColor( hps, hchlb->fgColor );
            GpiSetBackColor( hps, hchlb->bgColor );
        }

        HanOutHch(hps, x, y, HCHColQueryHch( hchlb->hchCol, i ));

        x += ( HCH_WIDTH + hchlb->horzInt );
    }

    GpiSetColor( hps, hchlb->fgColor );
    GpiSetBackColor( hps, hchlb->bgColor );

    for( ; ( i - hchlb->topIndex ) % hchlb->xChars != 0; i ++ )
    {
        HanOut( hps, x, y, "  " );
        x += ( HCH_WIDTH + hchlb->horzInt );
    }

/*
    rectlUpdate.yTop = y;
    rectlUpdate.xRight = rectlEntire.xRight - HCH_BORDER;
    rectlUpdate.yBottom = HCH_BORDER;
    rectlUpdate.xLeft = HCH_BORDER;
    WinFillRect( hps, &rectlUpdate, hchlb->bgColor );

    rectlUpdate.yTop = rectlEntire.yTop - HCH_BORDER;
    rectlUpdate.yBottom = rectlUpdate.yTop - hchlb->vertInt;
    while( rectlUpdate.yTop > y )
    {
        WinFillRect( hps, &rectlUpdate, hchlb->bgColor );

        rectlUpdate.yTop -= HCH_HEIGHT + hchlb->vertInt;
        rectlUpdate.yBottom = rectlUpdate.yTop - hchlb->vertInt;
    }

    rectlUpdate.yTop = rectlEntire.yTop - HCH_BORDER;
    rectlUpdate.xRight = rectlEntire.xRight - HCH_BORDER;
    rectlUpdate.yBottom = HCH_BORDER;
    rectlUpdate.xLeft = x;
    WinFillRect( hps, &rectlUpdate, hchlb->bgColor );

    rectlUpdate.xLeft = HCH_BORDER;
    rectlUpdate.xRight = rectlUpdate.xLeft + hchlb->horzInt;
    while( rectlUpdate.xLeft < x )
    {
        WinFillRect( hps, &rectlUpdate, hchlb->bgColor );

        rectlUpdate.xLeft += HCH_WIDTH + hchlb->horzInt;
        rectlUpdate.xRight = rectlUpdate.xLeft + hchlb->horzInt;
    }
*/
    WinEndPaint(hps);
    return 0;
}

MRESULT hchlbc_umRefresh( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    PHCHLB hchlb = WinQueryWindowPtr( hwnd, 0 );

    WinInvalidateRect( hwnd, NULL, FALSE );
    hchlb->eraseBg = TRUE;

    return 0;
}

MRESULT hchlbc_umSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    WinSetFocus( HWND_DESKTOP, hwnd );

    return 0;
}

void notify( HWND hwnd, USHORT notifyCode, MPARAM mp2 )
{
    HWND    hwndOwner = WinQueryWindow( hwnd, QW_OWNER );
    USHORT  usIdOwner = WinQueryWindowUShort( hwndOwner, QWS_ID );
    HWND    hwndOwnerOwner = WinQueryWindow( hwndOwner, QW_OWNER );

    WinPostMsg( hwndOwnerOwner, WM_CONTROL,
                MPFROM2SHORT( usIdOwner, notifyCode ),
                mp2 );
}

int pts2idx( PHCHLB hchlb, POINTS *pts )
{
    RECTL   rcl;
    int     x, y;
    int     xIdx, yIdx;
    int     i;

    if(( hchlb == NULL ) || ( pts == NULL ))
        return HCHLIT_NONE;

    WinQueryWindowRect( hchlb->hwndClient, &rcl );

    xIdx = -1;
    for( x = hchlb->horzInt, i = 0; i < hchlb->xChars;
         x += HCH_WIDTH + hchlb->horzInt, i ++ )
    {
        if(( pts->x >= x ) && ( pts->x < x + HCH_WIDTH ))
            xIdx = i;
    }

    yIdx = -1;
    for( y = rcl.yTop - hchlb->vertInt, i = 0; i < hchlb->yChars;
         y -= HCH_HEIGHT + hchlb->vertInt, i ++ )
    {
        if(( pts->y < y ) && ( pts->y >= y - HCH_HEIGHT ))
            yIdx = i;
    }

    i = yIdx * hchlb->xChars + xIdx;

    if(( xIdx < 0 ) || ( yIdx < 0 ) || ( i >= hchlb->hchCol->count ))
        return HCHLIT_NONE;

    return i;
}


