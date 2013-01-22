#include <stdio.h>
#include <ctype.h>

#include "HMLE_internal.h"

#include "../hchlb/hchlb.h"

#define HMLEID_CLIENT           (TID_USERMAX - 1)
#define HMLEID_VSCROLL          (TID_USERMAX - 2)
#define HMLEID_HSCROLL          (TID_USERMAX - 3)

static MRESULT hmle_wmChar(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmCreate(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmVScroll(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmHScroll(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_wmCalcFrameRect(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hmle_usermSetHIA(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermQueryHIA(HWND,MPARAM,MPARAM);

static MRESULT hmle_usermNewDoc(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermExport(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermImport(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermSetImportExport(HWND,MPARAM,MPARAM);

static MRESULT hmle_usermQueryFormatTextLength(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQueryTextLength(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQuerySelText(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hmle_usermCopy(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermCut(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermPaste(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermClear(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hmle_usermSearch(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermInsert(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermDelete(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hmle_usermSetTextColor(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermSetBackColor(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermSetSelTextColor(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermSetSelBackColor(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermQueryTextColor(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermQueryBackColor(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermQuerySelTextColor(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermQuerySelBackColor(HWND,MPARAM,MPARAM);

static MRESULT hmle_usermSetChanged(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQueryChanged(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermSetEolType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQueryEolType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermSetHanType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQueryHanType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermSetAutoIndent( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hmle_usermQueryAutoIndent( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT hmle_usermSetReadonly(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQueryReadonly(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermSetSel(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQuerySel(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermSetFirstChar(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQueryFirstChar(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermSetTabstop(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmle_usermQueryTabstop(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hmle_usermRefresh(HWND,MPARAM,MPARAM);
static MRESULT hmle_usermRefreshScrollbar(HWND,MPARAM,MPARAM);

static MRESULT hmle_usermSetWrap( HWND, MPARAM, MPARAM );
static MRESULT hmle_usermQueryWrap( HWND, MPARAM, MPARAM );

static MRESULT hmle_usermDump(HWND,MPARAM,MPARAM);

// Register the class.
BOOL EXPENTRY RegisterHanMLEControl(HAB hab)
{
BOOL rc;
    rc = WinRegisterClass( hab, WC_HMLE, HMLEWinProc,
                CS_SIZEREDRAW, 8 );
    if (rc==FALSE) return rc;
    rc = WinRegisterClass( hab, WC_HMLE_Client, HMLEClientWinProc,
                CS_SIZEREDRAW, 8 );
    if (rc==FALSE) return rc;

    return RegisterHanCharListBoxControl( hab );
}

void HMLEWND_Notify(HWND hwnd,USHORT notifCode,MPARAM mp2)
{
HWND    hwndOwner = WinQueryWindow(hwnd,QW_OWNER);
USHORT  usId = WinQueryWindowUShort(hwnd,QWS_ID);

    WinPostMsg(hwndOwner,WM_CONTROL,
        MPFROM2SHORT(usId,notifCode),
        mp2);
}

MRESULT APIENTRY HMLEWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case HMLM_SETHIA:               return hmle_usermSetHIA(hwnd,mp1,mp2);
    case HMLM_QUERYHIA:             return hmle_usermQueryHIA(hwnd,mp1,mp2);

    case HMLM_NEWDOC:               return hmle_usermNewDoc(hwnd,mp1,mp2);
    case HMLM_IMPORT:               return hmle_usermImport(hwnd,mp1,mp2);
    case HMLM_EXPORT:               return hmle_usermExport(hwnd,mp1,mp2);
    case HMLM_SETIMPORTEXPORT:      return hmle_usermSetImportExport(hwnd,mp1,mp2);
    case HMLM_QUERYFORMATTEXTLENGTH:return hmle_usermQueryFormatTextLength(hwnd,mp1,mp2);
    case HMLM_QUERYTEXTLENGTH:      return hmle_usermQueryTextLength(hwnd,mp1,mp2);
    case HMLM_QUERYSELTEXT:         return hmle_usermQuerySelText(hwnd,mp1,mp2);

    case HMLM_REFRESH:              return hmle_usermRefresh(hwnd,mp1,mp2);
    case HMLM_REFRESHSCROLLBAR:     return hmle_usermRefreshScrollbar(hwnd,mp1,mp2);

    case HMLM_SETTEXTCOLOR:         return hmle_usermSetTextColor(hwnd,mp1,mp2);
    case HMLM_SETBACKCOLOR:         return hmle_usermSetBackColor(hwnd,mp1,mp2);
    case HMLM_SETSELTEXTCOLOR:      return hmle_usermSetSelTextColor(hwnd,mp1,mp2);
    case HMLM_SETSELBACKCOLOR:      return hmle_usermSetSelBackColor(hwnd,mp1,mp2);
    case HMLM_QUERYTEXTCOLOR:       return hmle_usermQueryTextColor(hwnd,mp1,mp2);
    case HMLM_QUERYBACKCOLOR:       return hmle_usermQueryBackColor(hwnd,mp1,mp2);
    case HMLM_QUERYSELTEXTCOLOR:    return hmle_usermQuerySelTextColor(hwnd,mp1,mp2);
    case HMLM_QUERYSELBACKCOLOR:    return hmle_usermQuerySelBackColor(hwnd,mp1,mp2);

    case HMLM_SETCHANGED:           return hmle_usermSetChanged(hwnd,mp1,mp2);
    case HMLM_SETEOLTYPE:           return hmle_usermSetEolType(hwnd,mp1,mp2);
    case HMLM_SETHANTYPE:           return hmle_usermSetHanType(hwnd,mp1,mp2);
    case HMLM_SETAUTOINDENT:        return hmle_usermSetAutoIndent( hwnd, mp1, mp2 );
    case HMLM_SETREADONLY:          return hmle_usermSetReadonly(hwnd,mp1,mp2);
    case HMLM_SETSEL:               return hmle_usermSetSel(hwnd,mp1,mp2);
    case HMLM_SETFIRSTCHAR:         return hmle_usermSetFirstChar(hwnd,mp1,mp2);
    case HMLM_SETTABSTOP:           return hmle_usermSetTabstop(hwnd,mp1,mp2);
    case HMLM_QUERYCHANGED:         return hmle_usermQueryChanged(hwnd,mp1,mp2);
    case HMLM_QUERYEOLTYPE:         return hmle_usermQueryEolType(hwnd,mp1,mp2);
    case HMLM_QUERYHANTYPE:         return hmle_usermQueryHanType(hwnd,mp1,mp2);
    case HMLM_QUERYAUTOINDENT:      return hmle_usermQueryAutoIndent( hwnd, mp1, mp2 );
    case HMLM_QUERYREADONLY:        return hmle_usermQueryReadonly(hwnd,mp1,mp2);
    case HMLM_QUERYSEL:             return hmle_usermQuerySel(hwnd,mp1,mp2);
    case HMLM_QUERYFIRSTCHAR:       return hmle_usermQueryFirstChar(hwnd,mp1,mp2);
    case HMLM_QUERYTABSTOP:         return hmle_usermQueryTabstop(hwnd,mp1,mp2);

    case HMLM_SEARCH:               return hmle_usermSearch(hwnd,mp1,mp2);
    case HMLM_INSERT:               return hmle_usermInsert(hwnd,mp1,mp2);
    case HMLM_DELETE:               return hmle_usermDelete(hwnd,mp1,mp2);
    case HMLM_COPY:                 return hmle_usermCopy(hwnd,mp1,mp2);
    case HMLM_CUT:                  return hmle_usermCut(hwnd,mp1,mp2);
    case HMLM_PASTE:                return hmle_usermPaste(hwnd,mp1,mp2);
    case HMLM_CLEAR:                return hmle_usermClear(hwnd,mp1,mp2);

    case HMLM_SETWRAP:              return hmle_usermSetWrap( hwnd, mp1, mp2 );
    case HMLM_QUERYWRAP:            return hmle_usermQueryWrap( hwnd, mp1, mp2 );

    case HMLM_DUMP:                 return hmle_usermDump(hwnd,mp1,mp2);

    case WM_CALCFRAMERECT:          return hmle_wmCalcFrameRect(hwnd,mp1,mp2);
    case WM_CHAR:                   return hmle_wmChar( hwnd, mp1, mp2 );
    case WM_CREATE:                 return hmle_wmCreate( hwnd, mp1, mp2 );
    case WM_DESTROY:                return hmle_wmDestroy( hwnd, mp1, mp2 );
    case WM_SETFOCUS:               return hmle_wmSetFocus( hwnd, mp1, mp2 );
    case WM_SIZE:                   return hmle_wmSize( hwnd, mp1, mp2 );
    case WM_PAINT:                  return hmle_wmPaint( hwnd, mp1, mp2 );
    case WM_VSCROLL:                return hmle_wmVScroll(hwnd,mp1,mp2);
    case WM_HSCROLL:                return hmle_wmHScroll(hwnd,mp1,mp2);
    default:                        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT hmle_wmCreate(HWND hwnd, MPARAM mp1,MPARAM mp2)
{
PCREATESTRUCT pCreate = (PCREATESTRUCT)mp2;
PHMLECTLDATA pHMLECD = (PHMLECTLDATA)mp1;
PHMLECTLDATA pCtlData = NULL;
HMLE* hmle = NULL;

        if (DosAllocMem((PPVOID)&pCtlData,sizeof(HMLECTLDATA),fALLOC))
            return MRFROMLONG(TRUE);
    if ((pHMLECD != NULL) && (pHMLECD->cb == sizeof(HMLECTLDATA)))
        {
        pCtlData->cb        = pHMLECD->cb;
        pCtlData->han_type  = pHMLECD->han_type;
        pCtlData->eol_type  = pHMLECD->eol_type;
        pCtlData->autoIndent = pHMLECD->autoIndent;
        pCtlData->maxLineSize = pHMLECD->maxLineSize;
        pCtlData->wordWrapSize = pHMLECD->wordWrapSize;
        if (WinIsWindow(WinQueryAnchorBlock(hwnd),pHMLECD->hwndHIA))
            pCtlData->hwndHIA = pHMLECD->hwndHIA;
            else
            pCtlData->hwndHIA = NULLHANDLE;
        } else {
        pCtlData->cb        = sizeof(HMLECTLDATA);
        pCtlData->han_type  = HMLE_HAN_KS;
        pCtlData->eol_type  = HMLE_EOL_CRLF;
        pCtlData->autoIndent = TRUE;
        pCtlData->maxLineSize = HMLE_DEFAULT_MAXLINESIZE;
        pCtlData->wordWrapSize = 0;
        pCtlData->hwndHIA   = NULLHANDLE;
        }

    if (DosAllocMem((PPVOID)&hmle,sizeof(HMLE),fALLOC))
        {
        DosFreeMem(pCtlData);
        return MRFROMLONG(TRUE);
        }
    hmle->pCtlData = pCtlData;
    hmle->hwndHMLE = hwnd;
    hmle->hwndHIA = pCtlData->hwndHIA;
    hmle->bufImportExport = NULL;
    hmle->ieBufSize = 0;
    hmle->eol_type = pCtlData->eol_type;
    hmle->han_type = pCtlData->han_type;
    hmle->autoIndent = pCtlData->autoIndent;
    hmle->bgColor = SYSCLR_WINDOW;
    hmle->fgColor = SYSCLR_WINDOWTEXT;
    hmle->selbgColor = hmle->fgColor;
    hmle->selfgColor = hmle->bgColor;
    if (pCreate->flStyle&HMLS_READONLY)
        hmle->readonly = TRUE;
        else
        hmle->readonly = FALSE;
    hmle->connectedToHIA = FALSE;

    hmle->dropFileNotif = NULL;
    if (pCreate->flStyle & HMLS_ACCEPTFILEDROP)
        {
        if (DosAllocMem((PPVOID)&(hmle->dropFileNotif),sizeof(HMLEFILEDROPNOTIF),fALLOC))
            {
            DosFreeMem(pCtlData);
            DosFreeMem(hmle);
            }
        if (DosAllocMem((PPVOID)&(hmle->dropFileNotif->fullPath),CCHMAXPATH,fALLOC))
            {
            DosFreeMem(pCtlData);
            DosFreeMem(hmle);
            DosFreeMem(hmle->dropFileNotif);
            return MRFROMLONG(TRUE);
            }
        hmle->dropFileNotif->fullPath[0]=0;
        }

    hmle->doc = HMLECreateDoc(hmle->pCtlData->maxLineSize);
#ifdef DEBUG
    assert(hmle->doc!=NULL);
#endif
    if (hmle->doc==NULL)
        {
        if (hmle->dropFileNotif!=NULL)
            {
            DosFreeMem(hmle->dropFileNotif->fullPath);
            DosFreeMem(hmle->dropFileNotif);
            }
        DosFreeMem(hmle);
        return MRFROMLONG(TRUE);
        }
    hmle->doc->tabsize = 8;
    hmle->beginLineN = 0;
    hmle->beginColN = 0;

    hmle->doc->wordWrap = pCreate->flStyle & HMLS_WORDWRAP;
    hmle->doc->wordWrapSizeAuto = pCtlData->wordWrapSize == 0;
    hmle->doc->wordWrapSize = pCtlData->wordWrapSize;

    WinSetWindowPtr(hwnd,WINWORD_INSTANCE,hmle);

    {
    HWND    hwndClient=NULLHANDLE;
    LONG    x=0,y=0;
    LONG    cx,cy;
    LONG    sv_cxVscroll = WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL);
    LONG    sv_cyHscroll = WinQuerySysValue(HWND_DESKTOP,SV_CYHSCROLL);

    cx = pCreate->cx;
    cy = pCreate->cy;

#ifdef DEBUG
    printf("cx,cy = %d,%d\n",cx,cy);
#endif

    if (pCreate->flStyle & HMLS_VSCROLL)
        {
        HWND hwndVScroll;
        hwndVScroll = WinCreateWindow(
                hwnd,
                WC_SCROLLBAR,(PSZ)NULL,
                WS_VISIBLE|SBS_VERT,
                cx-sv_cxVscroll,0,
                sv_cxVscroll,cy,
                hwnd,
                HWND_TOP,HMLEID_VSCROLL,
                NULL,NULL);
        cx-=(sv_cxVscroll-1);
        }
    if (pCreate->flStyle & HMLS_HSCROLL)
        {
        HWND hwndHScroll;
        hwndHScroll = WinCreateWindow(
                hwnd,
                WC_SCROLLBAR,(PSZ)NULL,
                WS_VISIBLE|SBS_HORZ,
                0,0,
                cx,sv_cyHscroll,
                hwnd,
                HWND_TOP,HMLEID_HSCROLL,
                NULL,NULL);
        cy-=sv_cyHscroll;
        y = sv_cyHscroll;
        }
    hwndClient = WinCreateWindow (
            hwnd,
            WC_HMLE_Client,(PSZ)NULL,
            pCreate->flStyle | WS_SYNCPAINT,
            x+HMLE_BORDERSIZE,y+HMLE_BORDERSIZE,
            cx-(HMLE_BORDERSIZE*2),cy-(HMLE_BORDERSIZE*2),
            hwnd,
            HWND_TOP,
            HMLEID_CLIENT,
            (PVOID)(hmle),NULL);
    hmle->hwndClient = hwndClient;
#ifdef DEBUG
    assert(hwndClient!=NULLHANDLE);
#endif

    WinPostMsg(hwnd,HMLM_REFRESHSCROLLBAR,0L,0L);
    }

    return FALSE;
}

MRESULT hmle_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//HWND hwndC;
PHMLECTLDATA  pCtlData;

/*  hwndC = WinWindowFromID(hwnd,HMLEID_HSCROLL);
    WinPostMsg(hwndC,WM_DESTROY,0,0);
    hwndC = WinWindowFromID(hwnd,HMLEID_VSCROLL);
    WinPostMsg(hwndC,WM_DESTROY,0,0);
    hwndC = WinWindowFromID(hwnd,HMLEID_CLIENT);
    WinPostMsg(hwndC,WM_DESTROY,0,0);
*/
    if (hmle->hwndHIA!=NULLHANDLE)
        WinSendMsg(hmle->hwndHIA,HIAM_UNREGISTERNOTIFY,MPFROMLONG(hmle->hwndClient),0);

    if (hmle->dropFileNotif)
        {
        DosFreeMem(hmle->dropFileNotif->fullPath);
        DosFreeMem(hmle->dropFileNotif);
        }
    if (hmle->doc) HMLEDestroyDoc(hmle->doc);
    pCtlData = hmle->pCtlData;
    DosFreeMem(hmle);
    DosFreeMem(pCtlData);

    return 0L;
}

MRESULT hmle_wmChar(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
#ifdef DEBUG
USHORT  fsFlags = SHORT1FROMMP(mp1);
UCHAR   ucVkey  = CHAR3FROMMP(mp2);

    if ((ucVkey == VK_ESC) && (fsFlags & KC_VIRTUALKEY)
        && !(fsFlags & KC_KEYUP))
        {
        HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
        HMLEDocDump(hmle->doc,stderr,0);
        }
#endif

    return WinDefWindowProc(hwnd,WM_CHAR,mp1,mp2);
}

MRESULT hmle_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG flStyle = WinQueryWindowULong( hwnd, QWL_STYLE );
USHORT ncx,ncy;
HWND hwndVScroll = NULLHANDLE,hwndHScroll = NULLHANDLE;
LONG x = 0, y = 0;
LONG    sv_cxVscroll = WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL);
LONG    sv_cyHscroll = WinQuerySysValue(HWND_DESKTOP,SV_CYHSCROLL);

    ncx = SHORT1FROMMP(mp2);
    ncy = SHORT2FROMMP(mp2);

    if( flStyle & HMLS_BORDER )
    {
        ncx -= HMLE_BORDERSIZE * 2;
        ncy -= HMLE_BORDERSIZE * 2;
        x += HMLE_BORDERSIZE;
        y += HMLE_BORDERSIZE;
    }

    if (flStyle & HMLS_VSCROLL)
        {
        hwndVScroll = WinWindowFromID(hwnd,HMLEID_VSCROLL);
        WinSetWindowPos(hwndVScroll,NULLHANDLE,
                x + ncx - sv_cxVscroll, y, sv_cxVscroll,ncy,
                SWP_SIZE|SWP_MOVE);
        ncx -= sv_cxVscroll;
        }
    if (flStyle & HMLS_HSCROLL)
        {
        hwndHScroll = WinWindowFromID(hwnd,HMLEID_HSCROLL);
        WinSetWindowPos(hwndHScroll,NULLHANDLE,
                x, y, ncx + 1,sv_cyHscroll,
                SWP_SIZE|SWP_MOVE);
        ncy -= sv_cyHscroll;
        y += sv_cyHscroll;

        if( flStyle & HMLS_VSCROLL )
            WinSetWindowPos(hwndVScroll,NULLHANDLE,
                x + ncx, y - 1,
                sv_cxVscroll,ncy + 1,
                SWP_SIZE|SWP_MOVE);
        }

    WinSetWindowPos(hmle->hwndClient,NULLHANDLE,
            x,y,ncx,ncy, SWP_SIZE|SWP_MOVE);

/*
    WinSetWindowPos(hmle->hwndClient,NULLHANDLE,
            HMLE_BORDERSIZE,y+HMLE_BORDERSIZE,
            ncx-(HMLE_BORDERSIZE*2),ncy-(HMLE_BORDERSIZE*2),
            SWP_SIZE|SWP_MOVE);
*/
    return 0L;
}

MRESULT hmle_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HWND hwndVScroll = WinWindowFromID(hwnd,HMLEID_VSCROLL);
HWND hwndHScroll = WinWindowFromID(hwnd,HMLEID_HSCROLL);
HWND hwndMp1 = (HWND)mp1;
//BOOL rc;

    if (SHORT1FROMMP(mp2))
        {
        if ((hwndMp1!=hwndVScroll)&&(hwndMp1!=hwndHScroll))
            {
            WinPostMsg(hmle->hwndClient,HMLMC_REFRESHCURSOR,MRFROMLONG(TRUE),0); // client receives Focus
//          WinPostMsg(hwnd,HMLM_REFRESH,0,0);
//          HMLEWND_Notify(hwnd,HMLN_SETFOCUS,0);
            }
        } else {

//      WinShowCursor(hmle->hwndClient,FALSE);
//      HMLEWND_Notify(hwnd,HMLN_KILLFOCUS,0);
        }

    return 0L;
}

MRESULT hmle_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HPS hps;
RECTL rectlUpdate;
ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);

    hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);
#ifdef DEBUG
    assert(hps!=NULLHANDLE);
#endif

    if(( flStyle & ( HMLS_HSCROLL | HMLS_VSCROLL )) == ( HMLS_HSCROLL | HMLS_VSCROLL ))
    {
        WinQueryWindowRect(hwnd,&rectlUpdate);

        rectlUpdate.xLeft = rectlUpdate.xRight - WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL) + 1;
        rectlUpdate.yTop = rectlUpdate.yBottom + WinQuerySysValue(HWND_DESKTOP,SV_CYHSCROLL) - 1;
        WinFillRect( hps, &rectlUpdate, SYSCLR_BUTTONMIDDLE );
    }

    if (flStyle&HMLS_BORDER)
        {
        RECTL rectlEntire;
        POINTL p;

        WinQueryWindowRect(hwnd,&rectlEntire);

/*      if (flStyle & HMLS_VSCROLL)
            {
            LONG sv_cxVscroll = WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL);
            rectlEntire.xRight -= (sv_cxVscroll-1);
            } */

        GpiSetColor(hps,SYSCLR_BUTTONDARK);
        p.x = rectlEntire.xRight - 1;
        p.y = rectlEntire.yTop - 1;
        GpiMove(hps,&p);
        p.x = rectlEntire.xLeft; GpiLine(hps,&p);
        p.y = rectlEntire.yBottom; GpiLine(hps,&p);
        p.x = rectlEntire.xLeft + 1; GpiMove(hps,&p); GpiSetColor(hps,SYSCLR_BUTTONLIGHT);
        p.x = rectlEntire.xRight - 1; GpiLine(hps,&p);
        p.y = rectlEntire.yTop - 2; GpiLine(hps,&p);
        }
    WinEndPaint(hps);

    return 0L;
}

MRESULT hmle_wmVScroll(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
USHORT cmd = SHORT2FROMMP(mp2);
USHORT pos = SHORT1FROMMP(mp2);

    switch (cmd)
    {
    case SB_LINEUP:             HMLELineUp(hmle);       break;
    case SB_LINEDOWN:           HMLELineDown(hmle);     break;
    case SB_PAGEUP:             HMLEPageUp(hmle);       break;
    case SB_PAGEDOWN:           HMLEPageDown(hmle);     break;
    case SB_SLIDERTRACK:
        HMLESetPageLine(hmle,pos);
        break;
    case SB_SLIDERPOSITION:
        HMLESetPageLine(hmle,pos);
        break;
    }

    WinPostMsg(hmle->hwndClient,HMLMC_REFRESHCURSOR,0,0);
    return 0L;
}

MRESULT hmle_wmHScroll(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
USHORT cmd = SHORT2FROMMP(mp2);
USHORT pos = SHORT1FROMMP(mp2);

    switch (cmd)
    {
    case SB_LINEUP:             HMLEColLeft(hmle);       break;
    case SB_LINEDOWN:           HMLEColRight(hmle);      break;
    case SB_PAGEUP:             HMLEPageLeft(hmle);      break;
    case SB_PAGEDOWN:           HMLEPageRight(hmle);     break;
    case SB_SLIDERTRACK:
        HMLESetPageCol(hmle,pos);
        break;
    case SB_SLIDERPOSITION:
        HMLESetPageCol(hmle,pos);
        break;
    }

    WinPostMsg(hmle->hwndClient,HMLMC_REFRESHCURSOR,0,0);
    return 0L;
}

MRESULT hmle_wmCalcFrameRect(HWND hwnd,MPARAM mp1,MPARAM mp2)
// modified feature
//  input :
//  mp1 - PPOINTL : x = cols , y = lines
//  mp2 - not used
//  output :
//  mp1 - PPOINTL : x = cx , y = cy
//  mp2 - not used
//  return - 0
{
PPOINTL p = mp1;
ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);

    p->x = p->x*8;
    if (flStyle & HMLS_VSCROLL)
        p->x += WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL);

    p->y = p->y*16;
    if (flStyle & HMLS_HSCROLL)
        p->y += WinQuerySysValue(HWND_DESKTOP,SV_CYHSCROLL);

    if( flStyle & HMLS_BORDER )
    {
        p->x += HMLE_BORDERSIZE * 2;
        p->y += HMLE_BORDERSIZE * 2;
    }

    return 0;
}

MRESULT hmle_usermNewDoc(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLEDoc *newdoc;

#ifdef DEBUG
    assert(hmle!=NULL);
    assert(hmle->pCtlData!=NULL);
#endif

    newdoc = HMLECreateDoc(hmle->pCtlData->maxLineSize);
#ifdef DEBUG
    assert(newdoc!=NULL);
#endif

    if (newdoc==NULL) return MRFROMLONG(-1);

    newdoc->wordWrap = hmle->doc->wordWrap;
    newdoc->wordWrapSizeAuto = hmle->doc->wordWrapSizeAuto;
    newdoc->wordWrapSize = hmle->doc->wordWrapSize;

    HMLEDestroyDoc(hmle->doc);

    hmle->doc = newdoc;
    hmle->beginLineN = 0;
    hmle->beginColN = 0;

    WinPostMsg(hwnd,HMLM_REFRESH,0,0);
    return 0L;
}

MRESULT hmle_usermRefresh(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLEIpt curIpt;

    HMLERepositionPage( hmle );
    WinPostMsg(hwnd,HMLM_REFRESHSCROLLBAR,0L,0L);
    WinPostMsg(hmle->hwndClient,HMLMC_REFRESHCURSOR,0,0);
    HMLEDocQueryCurIpt(hmle->doc, &curIpt);
    HMLEStatNotify(hmle,HMLN_NOTIFYCURSORPOS,
                   MPFROM2SHORT(curIpt.ln, HMLEDocColFromStx(hmle->doc,curIpt.stx)));
    WinInvalidateRect(hwnd,NULL,TRUE);

    return 0L;
}

MRESULT hmle_usermRefreshScrollbar(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HWND    hwndVScroll = WinWindowFromID(hwnd,HMLEID_VSCROLL);
HWND    hwndHScroll = WinWindowFromID(hwnd,HMLEID_HSCROLL);
ULONG   flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);

    if (flStyle & HMLS_VSCROLL)
        {
        int doclines = HMLEDocQueryNumberOfLines(hmle->doc);
//      printf("doclines = %d\nySize = %d\n",doclines,hmle->ySize);

        if (doclines > hmle->ySize)
            WinSendMsg(hwndVScroll,SBM_SETSCROLLBAR,
                (MPARAM)(hmle->beginLineN),MPFROM2SHORT(0,doclines-hmle->ySize+1));
            else
            WinSendMsg(hwndVScroll,SBM_SETSCROLLBAR,
                (MPARAM)(hmle->beginLineN),MPFROM2SHORT(0,doclines+1));
        WinSendMsg(hwndVScroll,SBM_SETTHUMBSIZE,
                MPFROM2SHORT(hmle->ySize,doclines),0);
        }

    if (flStyle & HMLS_HSCROLL)
        {
        int docMaxCols = HMLEDocQueryMaxCols(hmle->doc);

        if (docMaxCols > hmle->xSize)
            WinSendMsg(hwndHScroll,SBM_SETSCROLLBAR,
                (MPARAM)(hmle->beginColN),MPFROM2SHORT(0,docMaxCols-hmle->xSize+1));
            else
            WinSendMsg(hwndHScroll,SBM_SETSCROLLBAR,
                (MPARAM)(hmle->beginColN),MPFROM2SHORT(0,docMaxCols+1));
        WinSendMsg(hwndHScroll,SBM_SETTHUMBSIZE,
                MPFROM2SHORT(hmle->xSize/2,docMaxCols),0);
        }

    return 0L;
}

MRESULT hmle_usermSetImportExport(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    hmle->bufImportExport = (PVOID)mp1;
    hmle->ieBufSize = (ULONG)mp2;

    return MRFROMLONG(TRUE);
}

MRESULT hmle_usermQueryImportExport(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    *((PVOID*)mp1) = hmle->bufImportExport;
    *((ULONG*)mp2) = hmle->ieBufSize;

    return MRFROMLONG(TRUE);
}

MRESULT hmle_usermImport(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLETextThunk *textThunk = NULL;

    if (hmle->bufImportExport==NULL) return MRFROMLONG(-1);
    if (hmle->readonly) return MRFROMLONG(-1);

    textThunk = HMLECreateTextThunk(hmle->bufImportExport);
    if (hmle->han_type == HMLE_HAN_KS)
        {
        char *str = HMLETextThunkQueryStr(textThunk);
        hch_ks2systr(str);
        }
    HMLEDocInsertTextThunk(hmle->doc,textThunk);
    if (hmle->doc->errno==HMLEDOC_ERROR_INSERTIONTRUNCATED)
        HMLEWND_Notify(hwnd,HMLN_OVERFLOW,0);
    HMLEDestroyTextThunk(textThunk);

    return 0L;
}

MRESULT hmle_usermExport(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLETextThunk *textThunk = NULL;

HMLEIpt beginIpt, endIpt;
PULONG pBeginOfs = mp1, pCount = mp2;
int beginOfs;

    if (hmle->bufImportExport==NULL) return MRFROMLONG(-1);

    if (pBeginOfs!=NULL)
        beginOfs = *pBeginOfs;
        else
        beginOfs = 0;
    HMLEDocQueryIpt(hmle->doc,&beginIpt,beginOfs);

    if ((pCount!=NULL) && (*pCount!=0))
        {
        HMLEDocQueryIpt(hmle->doc,&endIpt,beginOfs+*pCount);
        } else {
        endIpt.ln = HMLEDocQueryNumberOfLines(hmle->doc)-1;
        endIpt.stx = HMLELineQueryLen(HMLEDocQueryLine(hmle->doc,endIpt.ln));
        }

    textThunk = HMLEDocCreateTextThunk(hmle->doc,&beginIpt,&endIpt);
    if (textThunk==NULL) return MRFROMLONG(-1);
    if (hmle->han_type == HMLE_HAN_KS)
        {
        char *str = HMLETextThunkQueryStr(textThunk);

        hch_sy2ksstr(str);
        }
    if (hmle->eol_type == HMLE_EOL_CRLF)
        HMLETextThunkGetCRLFString(textThunk,hmle->bufImportExport);
        else
        HMLETextThunkGetLFString(textThunk,hmle->bufImportExport);

    HMLEDestroyTextThunk(textThunk);
    return 0;
}

MRESULT hmle_usermQueryFormatTextLength(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLETextThunk *textThunk = NULL;
HMLEIpt beginIpt,endIpt;
int size = 0;

    beginIpt.ln = 0;
    beginIpt.stx = 0;
    endIpt.ln = HMLEDocQueryNumberOfLines(hmle->doc)-1;
    endIpt.stx = HMLELineQueryLen(HMLEDocQueryLine(hmle->doc,endIpt.ln));
    textThunk = HMLEDocCreateTextThunk(hmle->doc,&beginIpt,&endIpt);
    if (textThunk == NULL) return MRFROMLONG(-1);
    if (hmle->eol_type == HMLE_EOL_CRLF)
        size = HMLETextThunkQueryCRLFStringLen(textThunk);
        else
        size = HMLETextThunkQueryLFStringLen(textThunk);
    HMLEDestroyTextThunk(textThunk);
    return MRFROMLONG(size);
}

MRESULT hmle_usermQueryTextLength(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLETextThunk *textThunk = NULL;
HMLEIpt beginIpt,endIpt;
int size = 0;

    beginIpt.ln = 0;
    beginIpt.stx = 0;
    endIpt.ln = HMLEDocQueryNumberOfLines(hmle->doc)-1;
    endIpt.stx = HMLELineQueryLen(HMLEDocQueryLine(hmle->doc,endIpt.ln));
    textThunk = HMLEDocCreateTextThunk(hmle->doc,&beginIpt,&endIpt);
    if (textThunk == NULL) return MRFROMLONG(-1);
    size = HMLETextThunkQueryLFStringLen(textThunk);
    HMLEDestroyTextThunk(textThunk);
    return MRFROMLONG(size);
}

MRESULT hmle_usermQuerySelText(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLETextThunk *textThunk = NULL;
char *buf = mp1;
int size = 0;

    if (HMLEDocQueryMarkingState(hmle->doc)==0) return 0;

    textThunk = HMLEDocGetSelection(hmle->doc);
    if (hmle->han_type == HMLE_HAN_KS)
        {
        char *str = HMLETextThunkQueryStr(textThunk);
        hch_sy2ksstr(str);
        }
    if (hmle->eol_type == HMLE_EOL_CRLF)
        {
        HMLETextThunkGetCRLFString(textThunk,buf);
        size = HMLETextThunkQueryCRLFStringLen(textThunk);
        } else {
        HMLETextThunkGetLFString(textThunk,buf);
        size = HMLETextThunkQueryLFStringLen(textThunk);
        }
    HMLEDestroyTextThunk(textThunk);
    return MRFROMLONG(size);
}

MRESULT hmle_usermCopy(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    HMLEClipboardExport(hmle);
    return 0;
}

MRESULT hmle_usermCut(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (hmle->readonly) return 0;

    HMLEClipboardExport(hmle);
    HMLEDocDeleteSelection(hmle->doc);
    HMLEWND_Notify(hwnd,HMLN_ENDSELECTION,0);

    WinPostMsg(hwnd,HMLM_REFRESH,0,0);

    HMLESetChange(hmle);

    return 0;
}

MRESULT hmle_usermPaste(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (hmle->readonly) return 0;

    HMLEClipboardImport(hmle);

    return 0;
}

MRESULT hmle_usermClear(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int deleteSize = 0;

    if (hmle->readonly) return 0;

    deleteSize = HMLEDocDeleteSelection(hmle->doc);
    HMLEWND_Notify(hwnd,HMLN_ENDSELECTION,0);
    WinPostMsg(hwnd,HMLM_REFRESH,0,0);

    HMLESetChange(hmle);

    return MRFROMLONG(deleteSize);
}

MRESULT hmle_usermSearch(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
PMLE_SEARCHDATA pSearchData = mp2;
ULONG ulStyle = (ULONG)mp1;
HMLEIpt beginIpt,endIpt,foundIpt;
int ret = FALSE;
char *findStr = NULL;

#ifdef DEBUG
    printf("HMLE::Search\n");
    printf("beginLineStr = %x\n",hmle->doc->beginLine->str);
#endif

    if (pSearchData->iptStart == -1)
        {
        HMLEDocQueryCurIpt(hmle->doc,&beginIpt);
        } else {
        HMLEDocQueryIpt(hmle->doc,&beginIpt,pSearchData->iptStart);
        }
    if (pSearchData->iptStop == -1)
        {
        HMLELine *endLine = HMLEDocQueryEndLine(hmle->doc);
        endIpt.ln = HMLELineQueryLineNumber(endLine);
        endIpt.stx = HMLELineQueryLen(endLine);
        } else {
        HMLEDocQueryIpt(hmle->doc,&endIpt,pSearchData->iptStop);
        }

    if (pSearchData->pchFind!=NULL)
        {
        findStr = pSearchData->pchFind;
        findStr = malloc(strlen(pSearchData->pchFind)+1);
        strcpy(findStr,pSearchData->pchFind);
        if (hmle->han_type == HMLE_HAN_KS)
            hch_ks2systr(findStr);

        }

    if (!(ulStyle&MLFSEARCH_CHANGEALL))
    {
    if (HMLEDocFindString(hmle->doc,findStr,//pSearchData->pchFind,
                &beginIpt,&endIpt,&foundIpt,
                (ulStyle&MLFSEARCH_CASESENSITIVE)))
        {
        HMLEIpt curIpt;

        HMLEDocQueryCurIpt(hmle->doc,&curIpt);
        pSearchData->iptStart = HMLEDocQueryOfs(hmle->doc,&foundIpt);

        if ((pSearchData->pchReplace!=NULL) && !(hmle->readonly))
            {
            HMLETextThunk *textThunk = HMLECreateTextThunk(pSearchData->pchReplace);
            if (textThunk!=NULL)
                {
                if (hmle->han_type == HMLE_HAN_KS)
                    {
                    char *buf = HMLETextThunkQueryStr(textThunk);
                    hch_ks2systr(buf);
                    }
                HMLEDocSetCurIpt(hmle->doc,&foundIpt);
                HMLEDocBeginSelectionHere(hmle->doc,&foundIpt);
                HMLEDocMoveCurStx(hmle->doc,strlen(pSearchData->pchFind));
                HMLEDocDeleteSelection(hmle->doc);
                HMLEDocInsertTextThunk(hmle->doc,textThunk);
                    if (hmle->doc->errno==HMLEDOC_ERROR_INSERTIONTRUNCATED)
                        HMLEWND_Notify(hwnd,HMLN_OVERFLOW,0);
                HMLEDocSetCurIpt(hmle->doc,&curIpt);
                HMLEDestroyTextThunk(textThunk);

                HMLESetChange(hmle);
                }
            }
        if (ulStyle&MLFSEARCH_SELECTMATCH)
            {
            HMLEDocSetCurIpt(hmle->doc,&foundIpt);
            HMLEDocBeginSelectionHere(hmle->doc,&foundIpt);
            HMLEDocMoveCurStx(hmle->doc,strlen(pSearchData->pchFind));
            HMLEWND_Notify(hwnd,HMLN_BEGINSELECTION,0);
            }
        ret = TRUE;
        } else {
        if (findStr!=NULL) free(findStr);
        return FALSE;
        }

    } else { // Change All

    int replaceCount = 0;
    HMLEIpt oldIpt;
    HMLEDocQueryCurIpt(hmle->doc,&oldIpt);

    if (hmle->readonly) return 0;

    if (pSearchData->pchReplace==NULL) return FALSE;
    if (HMLEDocQueryMarkingState(hmle->doc))
        {
        HMLEDocQuerySelectionBeginIpt(hmle->doc,&beginIpt);
        HMLEDocEndSelection(hmle->doc);
        HMLEWND_Notify(hwnd,HMLN_ENDSELECTION,0);
        }
    while (HMLEDocFindString(hmle->doc,findStr,//pSearchData->pchFind,
        &beginIpt,&endIpt,&foundIpt,
        (ulStyle&MLFSEARCH_CASESENSITIVE)))
        {
        HMLETextThunk *textThunk;
        HMLEIpt curIpt;

        HMLEDocQueryCurIpt(hmle->doc,&curIpt);
        pSearchData->iptStart = HMLEDocQueryOfs(hmle->doc,&foundIpt);
        textThunk = HMLECreateTextThunk(pSearchData->pchReplace);
        if (textThunk!=NULL)
            {
            if (hmle->han_type == HMLE_HAN_KS)
                {
                char *buf = HMLETextThunkQueryStr(textThunk);
                hch_ks2systr(buf);
                }
            HMLEDocSetCurIpt(hmle->doc,&foundIpt);
            HMLEDocBeginSelectionHere(hmle->doc,&foundIpt);
            HMLEDocMoveCurStx(hmle->doc,strlen(pSearchData->pchFind));
            HMLEDocDeleteSelection(hmle->doc);
            HMLEDocInsertTextThunk(hmle->doc,textThunk);
                if (hmle->doc->errno==HMLEDOC_ERROR_INSERTIONTRUNCATED)
                    HMLEWND_Notify(hwnd,HMLN_OVERFLOW,0);
            HMLEDocQueryCurIpt(hmle->doc,&beginIpt);
            HMLEDocSetCurIpt(hmle->doc,&curIpt);
            HMLEDestroyTextThunk(textThunk);
            HMLESetChange(hmle);
            }
        replaceCount++;
        } // while successful search
    ret = replaceCount;
    HMLEDocSetCurIpt(hmle->doc,&oldIpt);
    } // change all
    hmle->beginColN = 0;
    HMLERepositionPage(hmle);
    WinPostMsg(hwnd,HMLM_REFRESH,0,0);
    if (findStr!=NULL) free(findStr);
    return MRFROMLONG(ret);
}

MRESULT hmle_usermInsert(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
char *insertStr = mp1;

    if (hmle->readonly) return 0;

    if (HMLEDocQueryMarkingState(hmle->doc)!=0)
        {
        HMLEDocDeleteSelection(hmle->doc);
        HMLEWND_Notify(hwnd,HMLN_ENDSELECTION,0);
        HMLESetChange(hmle);
        }
    if (insertStr==NULL)
        {
        return 0L;
        } else {
        HMLETextThunk *textThunk = HMLECreateTextThunk(insertStr);
        #ifdef DEBUG
            assert(textThunk!=NULL);
        #endif
        if (textThunk==NULL) return 0L;
        if (hmle->han_type == HMLE_HAN_KS)
            {
            char *buf = HMLETextThunkQueryStr(textThunk);
            hch_ks2systr(buf);
            }
        HMLEDocInsertTextThunk(hmle->doc,textThunk);
            if (hmle->doc->errno==HMLEDOC_ERROR_INSERTIONTRUNCATED)
                HMLEWND_Notify(hwnd,HMLN_OVERFLOW,0);
        HMLEDestroyTextThunk(textThunk);
        HMLESetChange(hmle);
        }
    WinPostMsg(hwnd,HMLM_REFRESH,0,0);
    return 0;
}

MRESULT hmle_usermDelete(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int beginOfs = (int)mp1;
int delCount = (int)mp2;
HMLEIpt beginIpt,endIpt;

    if (hmle->readonly) return 0;

    if (beginOfs == -1)
        return WinSendMsg(hwnd,HMLM_CLEAR,0,0);
    if (HMLEDocQueryIpt(hmle->doc,&beginIpt,beginOfs)!=0)
        return 0;
    if (HMLEDocQueryIpt(hmle->doc,&endIpt,beginOfs+delCount)!=0)
        return 0;
    delCount = HMLEDocDelete(hmle->doc,&beginIpt,&endIpt);
    if (delCount>0) HMLESetChange(hmle);

    return MRFROMLONG(delCount);
}

MRESULT hmle_usermSetTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hmle->fgColor;
    hmle->fgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hmle_usermSetBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hmle->bgColor;
    hmle->bgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hmle_usermSetSelTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hmle->selfgColor;
    hmle->selfgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hmle_usermSetSelBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hmle->selbgColor;
    hmle->selbgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hmle_usermQueryTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->fgColor);
}

MRESULT hmle_usermQueryBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->bgColor);
}

MRESULT hmle_usermQuerySelTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->selfgColor);
}

MRESULT hmle_usermQuerySelBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->selbgColor);
}

MRESULT hmle_usermSetHIA(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HWND oldHIA = hmle->hwndHIA;

    hmle->hwndHIA = (HWND)mp1;

    if (oldHIA!=NULLHANDLE)
        if (WinSendMsg(oldHIA,HIAM_UNREGISTERNOTIFY,MPFROMLONG(hmle->hwndClient),0))
            {
            #ifdef DEBUG
            printf("successful UNREGISTER\n");
            #endif
            } else {
            #ifdef DEBUG
            printf("failed to unregister\n");
            #endif
            }
    if (hmle->hwndHIA!=NULLHANDLE)
        if (WinSendMsg(hmle->hwndHIA,HIAM_REGISTERNOTIFY,
            MPFROMLONG(hmle->hwndClient),MPFROMLONG(HMLECID_HIA)))
            {
            #ifdef DEBUG
            printf("successful Register\n");
            #endif
            } else {
            #ifdef DEBUG
            printf("failed to Register\n");
            #endif
            }

    return MRFROMLONG(oldHIA);
}

MRESULT hmle_usermQueryHIA(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->hwndHIA);
}

MRESULT hmle_usermDump(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    HMLEDocDump(hmle->doc,(FILE*)mp1,(int)mp2);

    return 0;
}

MRESULT hmle_usermSetChanged(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    hmle->doc->changed = (BOOL)mp1;

    return 0L;
}

MRESULT hmle_usermQueryChanged(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
BOOL changed = FALSE;

    changed = hmle->doc->changed;

    return MRFROMLONG(changed);
}

MRESULT hmle_usermSetEolType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG oldtype = hmle->eol_type;

    hmle->eol_type = (int)mp1;
    return MRFROMLONG(oldtype);
}

MRESULT hmle_usermQueryEolType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->eol_type);
}

MRESULT hmle_usermSetHanType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG oldtype = hmle->han_type;

    hmle->han_type = (int)mp1;
    return MRFROMLONG(oldtype);
}

MRESULT hmle_usermQueryHanType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->han_type);
}

MRESULT hmle_usermSetAutoIndent( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HMLE *hmle = WinQueryWindowPtr( hwnd, WINWORD_INSTANCE );
    BOOL oldAutoIndent = hmle->autoIndent;

    hmle->autoIndent = ( BOOL )mp1;

    return MRFROMSHORT( oldAutoIndent );
}

MRESULT hmle_usermQueryAutoIndent( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HMLE *hmle = WinQueryWindowPtr( hwnd, WINWORD_INSTANCE );

    return MRFROMSHORT( hmle->autoIndent );
}

MRESULT hmle_usermSetReadonly(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG prevState = 0;
ULONG newState = (ULONG)mp1;

    if (hmle->readonly)
        prevState = TRUE;
        else
        prevState = FALSE;
    hmle->readonly = newState;

    return MRFROMLONG(prevState);
}

MRESULT hmle_usermQueryReadonly(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (hmle->readonly)
        return MRFROMLONG(TRUE);
        else
        return MRFROMLONG(FALSE);
}

MRESULT hmle_usermSetSel(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG anchorOfs = (ULONG)mp1;
ULONG cursorOfs = (ULONG)mp2;
HMLEIpt anchorIpt,cursorIpt;

    HMLEDocQueryIpt(hmle->doc,&anchorIpt,anchorOfs);
    HMLEDocQueryIpt(hmle->doc,&cursorIpt,cursorOfs);
    HMLEDocBeginSelectionHere(hmle->doc,&anchorIpt);
    HMLEWND_Notify(hwnd,HMLN_BEGINSELECTION,0);
    HMLEDocSetCurIpt(hmle->doc,&cursorIpt);
    HMLERepositionPage(hmle);
    WinPostMsg(hwnd,HMLM_REFRESH,0,0);

    return (MRFROMLONG(TRUE));
}

MRESULT hmle_usermQuerySel(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG anchorOfs = 0;
ULONG cursorOfs = 0;
ULONG minOfs,maxOfs;
HMLEIpt anchorIpt,cursorIpt;
ULONG queryMode = (ULONG)mp1;

    HMLEDocQueryCurIpt(hmle->doc,&cursorIpt);
    if (HMLEDocQueryMarkingState(hmle->doc)!=0)
        HMLEDocQueryAnchorIpt(hmle->doc,&anchorIpt);
        else
        HMLEDocQueryCurIpt(hmle->doc,&anchorIpt);
    anchorOfs = HMLEDocQueryOfs(hmle->doc,&anchorIpt);
    cursorOfs = HMLEDocQueryOfs(hmle->doc,&cursorIpt);
    minOfs = ((anchorOfs<cursorOfs)?anchorOfs:cursorOfs);
    maxOfs = ((anchorOfs>cursorOfs)?anchorOfs:cursorOfs);
    switch (queryMode)
    {
    case HMLFQS_MINSEL:
        return MRFROMLONG(minOfs);
    case HMLFQS_MAXSEL:
        return MRFROMLONG(maxOfs);
    case HMLFQS_ANCHORSEL:
        return MRFROMLONG(anchorOfs);
    case HMLFQS_CURSORSEL:
        return MRFROMLONG(cursorOfs);
    case HMLFQS_MINMAXSEL:
        return MRFROM2SHORT(minOfs,maxOfs);
    }
    return 0;
}

MRESULT hmle_usermSetFirstChar(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int ofs = (int)mp1;
HMLEIpt ipt;

    HMLEDocQueryIpt(hmle->doc,&ipt,ofs);
    HMLEDocSetCurIpt(hmle->doc,&ipt);
    WinPostMsg(hwnd,HMLM_REFRESH,0,0);

    return 0L;
}

MRESULT hmle_usermQueryFirstChar(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HMLEIpt ipt;
int ofs;

    ipt.ln = hmle->beginLineN;
    ipt.stx = HMLEDocCol2Stx(
                hmle->doc,
                HMLEDocQueryLine(hmle->doc,hmle->beginLineN),
                hmle->beginColN);
    ofs = HMLEDocQueryOfs(hmle->doc,&ipt);
    return MRFROMLONG(ofs);
}

MRESULT hmle_usermSetTabstop(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int tabstop = (int)mp1;

    hmle->doc->tabsize = tabstop;
    WinPostMsg(hwnd,HMLM_REFRESH,0,0);
    return MRFROMLONG(tabstop);
}

MRESULT hmle_usermQueryTabstop(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hmle->doc->tabsize);
}

static MRESULT hmle_usermSetWrap( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
    BOOL wordWrap = ( BOOL )mp1;
    int  wordWrapSize = ( BOOL )mp2;

    hmle->doc->wordWrap = wordWrap;
    hmle->doc->wordWrapSizeAuto = FALSE;
    hmle->doc->wordWrapSize = wordWrapSize;
    if( wordWrapSize == 0 )
    {
        hmle->doc->wordWrapSizeAuto = TRUE;
        hmle->doc->wordWrapSize = hmle->xSize - 2;
    }

    HMLEDocWordWrap( hmle->doc, NULL );
    WinPostMsg( hwnd, HMLM_REFRESH, 0, 0 );

    return MRFROMLONG( TRUE );
}

static MRESULT hmle_usermQueryWrap( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
    BOOL wordWrap = hmle->doc->wordWrap;
    int  wordWrapSize = hmle->doc->wordWrapSize;

    if( hmle->doc->wordWrapSizeAuto )
        wordWrapSize = 0;

    return MRFROM2SHORT( wordWrap, wordWrapSize );
}

/*
MRESULT hmle_usermSetKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    WinSendMsg(hmle->hwndHIA,HIAM_SETKBDTYPE,mp1,mp2);
    return 0;
}

MRESULT hmle_usermQueryKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG flState;
int kbdtype;

    flState = (ULONG)WinSendMsg(hmle->hwndHIA,HIAM_QUERYSTATE,0,0);
    switch (flState & HIAST_KBDMASK)
    {
    case HIAST_KBD2:        kbdtype = HAN_KBD_2;        break;
    case HIAST_KBD390:      kbdtype = HAN_KBD_390;      break;
    case HIAST_KBD3FINAL:   kbdtype = HAN_KBD_3FINAL;   break;
    default:                kbdtype = HAN_KBD_2;        break;
    }
    return MRFROMLONG(kbdtype);
}

MRESULT hmle_usermSetHanInputMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    WinSendMsg(hmle->hwndHIA,HIAM_SETHANMODE,mp1,mp2);
    return 0;
}

MRESULT hmle_usermQueryHanInputMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG flState;
int hanmode;

    flState = (ULONG)WinSendMsg(hmle->hwndHIA,HIAM_QUERYSTATE,0,0);
    if (flState & HIAST_HANMODE)
        hanmode = HMLE_HAN;
        else
        hanmode = HMLE_ENG;
    return MRFROMLONG(hanmode);
}

MRESULT hmle_usermSetInsertMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    WinSendMsg(hmle->hwndHIA,HIAM_SETINSERTMODE,mp1,mp2);
    return 0;
}

MRESULT hmle_usermQueryInsertMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG flState;
int insertmode;

    flState = (ULONG)WinSendMsg(hmle->hwndHIA,HIAM_QUERYSTATE,0,0);
    if (flState & HIAST_INSERTMODE)
        insertmode = HMLE_INSERT;
        else
        insertmode = HMLE_OVERWRITE;
    return MRFROMLONG(insertmode);
}

*/

