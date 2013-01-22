#ifndef __HCHLB_INTERNAL_H__
#define __HCHLB_INTERNAL_H__

#include <os2.h>

#include "hchlb.h"
#include "hchcol.h"

typedef struct tagHCHLB {
    HWND        hwndHCHLB;
    HWND        hwndClient;
    PHCHCOL     hchCol;
    int         topIndex;
    int         bottomIndex;
    int         curIndex;
    int         horzInt;
    int         vertInt;
    int         xChars;
    int         yChars;
    int         cxClient;
    int         cyClient;
    LONG        fgColor;
    LONG        bgColor;
    BOOL        eraseBg;
} HCHLB, *PHCHLB;

#define HCH_WIDTH   16
#define HCH_HEIGHT  16
#define HCH_BORDER  1

#define WC_HCHLB_CLIENT "HanCharListBox:Client"

#define HCHLBID_CLIENT  ( TID_USERMAX - 1 )
#define HCHLBID_VSCROLL ( TID_USERMAX - 2 )

#define HCHLMC_REFRESH   ( WM_USER + 1 )
#define HCHLMC_SETFOCUS  ( WM_USER + 2 )

MRESULT EXPENTRY HCHLBWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY HCHLBClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

void HCHLBRepositionPage( PHCHLB hchlb );
void HCHLBLineUp( PHCHLB hchlb );
void HCHLBLineDown( PHCHLB hchlb );
void HCHLBPageUp( PHCHLB hchlb );
void HCHLBPageDown( PHCHLB hchlb );
void HCHLBSetLine( PHCHLB hchlb, int n );

#endif

