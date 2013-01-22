#include "HEF_internal.h"

#include "../hchlb/hchlb.h"

static MRESULT hef_wmCreate(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_wmQueryWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_wmSetWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hef_usermSetHIA(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_usermQueryHIA(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hef_usermQueryText(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQueryTextLength(HWND,MPARAM,MPARAM);

static MRESULT hef_usermSetTextLimit(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_usermQueryTextLimit(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_usermSetHanType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_usermQueryHanType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_usermSetChanged(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQueryChanged(HWND,MPARAM,MPARAM);
static MRESULT hef_usermSetReadonly(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQueryReadonly(HWND,MPARAM,MPARAM);
static MRESULT hef_usermSetSel(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQuerySel(HWND,MPARAM,MPARAM);
static MRESULT hef_usermSetFirstChar(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQueryFirstChar(HWND,MPARAM,MPARAM);

static MRESULT hef_usermSetTextColor(HWND,MPARAM,MPARAM);
static MRESULT hef_usermSetBackColor(HWND,MPARAM,MPARAM);
static MRESULT hef_usermSetSelTextColor(HWND,MPARAM,MPARAM);
static MRESULT hef_usermSetSelBackColor(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQueryTextColor(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQueryBackColor(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQuerySelTextColor(HWND,MPARAM,MPARAM);
static MRESULT hef_usermQuerySelBackColor(HWND,MPARAM,MPARAM);

static MRESULT hef_usermCopy(HWND,MPARAM,MPARAM);
static MRESULT hef_usermCut(HWND,MPARAM,MPARAM);
static MRESULT hef_usermPaste(HWND,MPARAM,MPARAM);
static MRESULT hef_usermClear(HWND,MPARAM,MPARAM);

static MRESULT hef_usermRefresh(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hef_usermDump(HWND hwnd,MPARAM mp1,MPARAM mp2);

BOOL EXPENTRY RegisterHanEntryFieldControl(HAB hab)
{
    if( !WinRegisterClass( hab, WC_HEF, HEFWinProc,
                CS_SYNCPAINT|CS_SIZEREDRAW, 8 ))
        return FALSE;

    if( !WinRegisterClass( hab, WC_HEF_CLIENT, HEFClientWinProc,
                CS_SYNCPAINT|CS_SIZEREDRAW, 8 ))
        return FALSE;

    return RegisterHanCharListBoxControl( hab );
}

void HEFWND_Notify(HWND hwnd,USHORT notifCode,MPARAM mp2)
{
HWND    hwndOwner = WinQueryWindow(hwnd,QW_OWNER);
USHORT  usId = WinQueryWindowUShort(hwnd,QWS_ID);

    WinPostMsg(hwndOwner,WM_CONTROL,
        MPFROM2SHORT(usId,notifCode),
        mp2);
}

MRESULT APIENTRY HEFWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case HEM_QUERYHIA:              return hef_usermQueryHIA(hwnd,mp1,mp2);
    case HEM_SETHIA:                return hef_usermSetHIA(hwnd,mp1,mp2);
    case HEM_QUERYTEXT:             return hef_usermQueryText(hwnd,mp1,mp2);
    case HEM_QUERYTEXTLENGTH:       return hef_usermQueryTextLength(hwnd,mp1,mp2);

    case HEM_SETTEXTCOLOR:          return hef_usermSetTextColor(hwnd,mp1,mp2);
    case HEM_SETBACKCOLOR:          return hef_usermSetBackColor(hwnd,mp1,mp2);
    case HEM_SETSELTEXTCOLOR:       return hef_usermSetSelTextColor(hwnd,mp1,mp2);
    case HEM_SETSELBACKCOLOR:       return hef_usermSetSelBackColor(hwnd,mp1,mp2);
    case HEM_QUERYTEXTCOLOR:        return hef_usermQueryTextColor(hwnd,mp1,mp2);
    case HEM_QUERYBACKCOLOR:        return hef_usermQueryBackColor(hwnd,mp1,mp2);
    case HEM_QUERYSELTEXTCOLOR:     return hef_usermQuerySelTextColor(hwnd,mp1,mp2);
    case HEM_QUERYSELBACKCOLOR:     return hef_usermQuerySelBackColor(hwnd,mp1,mp2);

    case HEM_QUERYTEXTLIMIT:        return hef_usermQueryTextLimit(hwnd,mp1,mp2);
    case HEM_QUERYCHANGED:          return hef_usermQueryChanged(hwnd,mp1,mp2);
    case HEM_QUERYFIRSTCHAR:        return hef_usermQueryFirstChar(hwnd,mp1,mp2);
    case HEM_QUERYREADONLY:         return hef_usermQueryReadonly(hwnd,mp1,mp2);
    case HEM_QUERYSEL:              return hef_usermQuerySel(hwnd,mp1,mp2);
    case HEM_QUERYHANTYPE:          return hef_usermQueryHanType(hwnd,mp1,mp2);

    case HEM_SETTEXTLIMIT:          return hef_usermSetTextLimit(hwnd,mp1,mp2);
    case HEM_SETCHANGED:            return hef_usermSetChanged(hwnd,mp1,mp2);
    case HEM_SETFIRSTCHAR:          return hef_usermSetFirstChar(hwnd,mp1,mp2);
    case HEM_SETREADONLY:           return hef_usermSetReadonly(hwnd,mp1,mp2);
    case HEM_SETSEL:                return hef_usermSetSel(hwnd,mp1,mp2);
    case HEM_SETHANTYPE:            return hef_usermSetHanType(hwnd,mp1,mp2);

    case HEM_COPY:                  return hef_usermCopy(hwnd,mp1,mp2);
    case HEM_CUT:                   return hef_usermCut(hwnd,mp1,mp2);
    case HEM_CLEAR:                 return hef_usermClear(hwnd,mp1,mp2);
    case HEM_PASTE:                 return hef_usermPaste(hwnd,mp1,mp2);

    case HEM_REFRESH:               return hef_usermRefresh(hwnd,mp1,mp2);
    case HEM_DUMP:                  return hef_usermDump(hwnd,mp1,mp2);

    case WM_CREATE:                 return hef_wmCreate(hwnd,mp1,mp2);
    case WM_DESTROY:                return hef_wmDestroy(hwnd,mp1,mp2);
    case WM_SETFOCUS:               return hef_wmSetFocus(hwnd,mp1,mp2);
    case WM_SIZE:                   return hef_wmSize(hwnd,mp1,mp2);
    case WM_PAINT:                  return hef_wmPaint(hwnd,mp1,mp2);
    case WM_QUERYWINDOWPARAMS:      return hef_wmQueryWindowParams(hwnd,mp1,mp2);
    case WM_SETWINDOWPARAMS:        return hef_wmSetWindowParams(hwnd,mp1,mp2);

    default:                        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT hef_wmCreate(HWND hwnd, MPARAM mp1,MPARAM mp2)
{
PCREATESTRUCT pCreate = (PCREATESTRUCT)mp2;
PHEFCTLDATA pHEFCD = (PHEFCTLDATA)mp1;
PHEFCTLDATA pCtlData = NULL;
HEF* hef = NULL;

//  printf("Frame:: WM_CREATE\n");
    if ((pHEFCD != NULL) && (pHEFCD->cb == sizeof(HEFCTLDATA)))
        {

        if (DosAllocMem((PPVOID)&pCtlData,sizeof(HEFCTLDATA),fALLOC))
            return MRFROMLONG(TRUE);
        pCtlData->cb        = pHEFCD->cb;
        pCtlData->han_type  = pHEFCD->han_type;
        if (WinIsWindow(WinQueryAnchorBlock(hwnd),pHEFCD->hwndHIA))
            pCtlData->hwndHIA = pHEFCD->hwndHIA;
            else
            pCtlData->hwndHIA = NULLHANDLE;
        } else {

        if (DosAllocMem((PPVOID)&pCtlData,sizeof(HEFCTLDATA),fALLOC))
            return MRFROMLONG(TRUE);
        pCtlData->cb        = sizeof(HEFCTLDATA);
        pCtlData->han_type  = HEF_HAN_KS;
        pCtlData->hwndHIA = NULLHANDLE;
        }

    if (DosAllocMem((PPVOID)&hef,sizeof(HEF),fALLOC))
        return MRFROMLONG(TRUE);

    hef->pCtlData = pCtlData;
    hef->hwndHEF = hwnd;
    hef->bgColor = SYSCLR_ENTRYFIELD;
    hef->fgColor = SYSCLR_WINDOWTEXT;
    hef->selbgColor = hef->fgColor;
    hef->selfgColor = hef->bgColor;

    if (pCreate->flStyle & HES_READONLY)
        hef->readonly = TRUE;
        else
        hef->readonly = FALSE;
    if (pCreate->flStyle & HES_UNREADABLE)
        hef->unreadable = TRUE;
        else
        hef->unreadable = FALSE;

    hef->allocsize = HEF_DEFAULT_LINESIZE;

    if (DosAllocMem((PVOID)&(hef->str),hef->allocsize,fALLOC))
        {
        DosFreeMem(hef);
        DosFreeMem(pCtlData);
        return MRFROMLONG(TRUE);
        }

    WinSetWindowPtr(hwnd,WINWORD_INSTANCE,hef);

    {
    int x,y,cx,cy;
    if (pCreate->flStyle & HES_MARGIN)
        {
        x = y = HEF_BORDERWIDTH;
        cx = pCreate->cx-(HEF_BORDERWIDTH*2);
        cy = pCreate->cy-(HEF_BORDERWIDTH*2);
        WinSetWindowPos(hwnd,NULLHANDLE,
            pCreate->x-HEF_BORDERWIDTH,
            pCreate->y-HEF_BORDERWIDTH,
            0,0,SWP_MOVE);
        } else {
        x = y = 0;
        cx = pCreate->cx;
        cy = pCreate->cy;
        }
    hef->hwndClient = WinCreateWindow(
            hwnd,
            WC_HEF_CLIENT,(PSZ)NULL,
            WS_VISIBLE|pCreate->flStyle,
            x,y,cx,cy,
            hwnd,
            HWND_TOP,
            HEFID_CLIENT,
            (PVOID)(hef),NULL);
    }
    if (hef->hwndClient==NULLHANDLE)
        {
        DosFreeMem(hef->str);
        DosFreeMem(hef);
        DosFreeMem(pCtlData);
        WinSetWindowPtr(hwnd,WINWORD_INSTANCE,NULL);
        return MRFROMLONG(TRUE);
        }
#ifdef DEBUG
//  dump(stderr,"HEF/pCtlData",sizeof(HEFCTLDATA),(char*)pCtlData);
//  dump(stderr,"HEF/hef",sizeof(HEF),(char*)hef);
//  dump(stderr,"HEF/hef->str",hef->allocsize,hef->str);
#endif

    return 0;
}

MRESULT hef_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

#ifdef DEBUG
    assert(hef!=NULL);
    printf("HEF::WM_DESTROY\n");
#endif
    if (hef==NULL) return 0L;

    if (hef->hwndHIA!=NULLHANDLE)
        WinSendMsg(hef->hwndHIA,HIAM_UNREGISTERNOTIFY,MPFROMLONG(hef->hwndClient),0);

    free(hef->outputBuf);
    DosFreeMem(hef->pCtlData);
    DosFreeMem(hef->str);
    DosFreeMem(hef);

    return 0L;
}

MRESULT hef_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
USHORT ncx = SHORT1FROMMP(mp2);
USHORT ncy = SHORT2FROMMP(mp2);
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (!WinSetWindowPos(hef->hwndClient,NULLHANDLE,
        HEF_BORDERWIDTH,HEF_BORDERWIDTH,
        ncx-(HEF_BORDERWIDTH*2),
        ncy-(HEF_BORDERWIDTH*2),
        SWP_MOVE|SWP_SIZE|SWP_SHOW))
        {
        #ifdef DEBUG
            printf("HEF::WM_SIZE - Unsuccessful.\n");
        #endif
        }

    return 0L;
}

MRESULT hef_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (SHORT1FROMMP(mp2))
        {
        #ifdef DEBUG
            printf("HEF::SETFOCUS\n");
        #endif
        WinSetFocus(HWND_DESKTOP,hef->hwndClient);
        WinPostMsg(hef->hwndClient,HEMC_REFRESHCURSOR,MPFROMLONG(TRUE),0);
        } else {
        #ifdef DEBUG
            printf("HEF::KILLFOCUS\n");
        #endif
//      WinShowCursor(hwnd,FALSE);
//      HEFWND_Notify(hwnd,HEN_KILLFOCUS,0);
        }
    return 0L;
}

MRESULT hef_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HPS hps;
RECTL rectlEntire,rectlUpdate;
ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
POINTL p;

    hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);
#ifdef DEBUG
    assert(hps!=NULLHANDLE);
//  printf("HEF::WM_PAINT\n");
#endif
    if (hps==NULLHANDLE) return 0;

    if (flStyle&HES_MARGIN)
        {
        WinQueryWindowRect(hwnd,&rectlEntire);

        GpiSetColor(hps,SYSCLR_BUTTONDARK);
        p.x = rectlEntire.xRight -1;
        p.y = rectlEntire.yTop -1;
        GpiMove(hps,&p);
        p.x = 0; GpiLine(hps,&p);
        p.y = 0; GpiLine(hps,&p);
        p.x = 1; GpiMove(hps,&p); GpiSetColor(hps,SYSCLR_BUTTONLIGHT);
        p.x = rectlEntire.xRight-1; GpiLine(hps,&p);
        p.y = rectlEntire.yTop-2; GpiLine(hps,&p);
        }
    WinEndPaint(hps);
    return 0L;
}

MRESULT hef_wmQueryWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
PWNDPARAMS pWndParams = mp1;
MRESULT mr = 0;

#ifdef DEBUG
    assert(hef!=NULL);
    assert(pWndParams!=NULL);
    printf("HEF:: WM_QUERYWINDOWPARAMS\n");
#endif

    if (pWndParams->fsStatus & WPM_TEXT)
        {
        if (pWndParams->fsStatus & WPM_CCHTEXT)
            {
            if (pWndParams->cchText-1 < strlen(hef->str))
                {
                strncpy(pWndParams->pszText,hef->str,pWndParams->cchText);
                pWndParams->pszText[pWndParams->cchText-1] = 0;
                } else strcpy(pWndParams->pszText,hef->str);
            } else strcpy(pWndParams->pszText,hef->str);
        if (hef->pCtlData->han_type == HEF_HAN_KS)
            hch_sy2ksstr(pWndParams->pszText);
        }
    if (pWndParams->fsStatus & WPM_CCHTEXT)
        {
        int len = strlen(hef->str);
        pWndParams->cchText = len;
        mr = MRFROMLONG(len);
        }
    if (pWndParams->fsStatus & WPM_CBCTLDATA)
        {
        #ifdef DEBUG
        printf("WPM_CBCTLDATA\n");
        #endif
        }
    if (pWndParams->fsStatus & WPM_CTLDATA)
        {
        #ifdef DEBUG
        printf("WPM_CTLDATA\n");
        #endif
        }
    if (pWndParams->fsStatus & WPM_PRESPARAMS)
        {
        #ifdef DEBUG
        printf("WPM_PRESPARAMS\n");
        #endif
        }
    if (pWndParams->fsStatus & WPM_CBPRESPARAMS)
        {
        #ifdef DEBUG
        printf("WPM_CBPRESPARAMS\n");
        #endif
        }

    return mr;
}

MRESULT hef_wmSetWindowParams(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PWNDPARAMS pWndParams = mp1;
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

#ifdef DEBUG
    assert(pWndParams!=NULL);
    printf("HEF:: WM_SETWINDOWPARAMS\n");
#endif

    if (pWndParams->fsStatus & WPM_CCHTEXT)
        {
#ifdef DEBUG
        printf("WPM_CCHTEXT\n");
#endif
        }
    if (pWndParams->fsStatus & WPM_TEXT)
        {
        if (strlen(pWndParams->pszText)+1 > hef->allocsize)
            {
            strncpy(hef->str,pWndParams->pszText,hef->allocsize-1);
            hef->str[hef->allocsize-1]=0;
            } else {
            strcpy(hef->str,pWndParams->pszText);
            }
        if (hef->pCtlData->han_type == HEF_HAN_KS)
            hch_ks2systr(hef->str);
        WinSendMsg( hwnd, HEM_REFRESH, 0, 0 );
        HEFSetChanged(hef);
        }
    if (pWndParams->fsStatus & WPM_CBCTLDATA)
        {
#ifdef DEBUG
        printf("WPM_CBCTLDATA\n");
#endif
        }
    if (pWndParams->fsStatus & WPM_CTLDATA)
        {
#ifdef DEBUG
        printf("WPM_CTLDATA\n");
#endif
        }
    if (pWndParams->fsStatus & WPM_PRESPARAMS)
        {
#ifdef DEBUG
        printf("WPM_PRESPARAMS\n");
#endif
        }
    if (pWndParams->fsStatus & WPM_CBPRESPARAMS)
        {
#ifdef DEBUG
        printf("WPM_CBPRESPARAMS\n");
#endif
        }

    return 0;
}

MRESULT hef_usermQueryText(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
char *str = mp1;

    strcpy(str,hef->str);
    if (hef->pCtlData->han_type == HEF_HAN_KS)
        hch_sy2ksstr(str);
    return 0;
}

MRESULT hef_usermQueryTextLength(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(strlen(hef->str));
}

MRESULT hef_usermRefresh(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//  printf("HEF::Refresh\n");

    WinInvalidateRect(hwnd,NULL,TRUE);
    WinSendMsg(hef->hwndClient,HEMC_REFRESHCURSOR,0L,0L);
    return 0L;
}

MRESULT hef_usermSetTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hef->fgColor;
    hef->fgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hef_usermSetBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hef->bgColor;
    hef->bgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hef_usermSetSelTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hef->selfgColor;
    hef->selfgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hef_usermSetSelBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
LONG oldColor;

    oldColor = hef->selbgColor;
    hef->selbgColor = (LONG)mp1;
    return MRFROMLONG(oldColor);
}

MRESULT hef_usermQueryTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->fgColor);
}

MRESULT hef_usermQueryBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->bgColor);
}

MRESULT hef_usermQuerySelTextColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->selfgColor);
}

MRESULT hef_usermQuerySelBackColor(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->selbgColor);
}

MRESULT hef_usermSetHIA(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HWND oldHIA = hef->hwndHIA;

    hef->hwndHIA = (HWND)mp1;

    if (oldHIA!=NULLHANDLE)
        if (WinSendMsg(oldHIA,HIAM_UNREGISTERNOTIFY,MPFROMLONG(hef->hwndClient),0))
            {
            #ifdef DEBUG
            printf("HEF : successful UNREGISTER\n");
            #endif
            } else {
            #ifdef DEBUG
            printf("HEF : failed to unregister\n");
            #endif
            }
    if (hef->hwndHIA!=NULLHANDLE)
        if (WinSendMsg(hef->hwndHIA,HIAM_REGISTERNOTIFY,
            MPFROMLONG(hef->hwndClient),MPFROMLONG(HEFCID_HIA)))
            {
            #ifdef DEBUG
            printf("HEF : successful Register\n");
            #endif
            } else {
            #ifdef DEBUG
            printf("HEF : failed to Register\n");
            #endif
            }

    return 0;
}

MRESULT hef_usermQueryHIA(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->hwndHIA);
}

MRESULT hef_usermDump(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
#ifdef DEBUG
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    dump32(stderr,"HEF/pCtlData",sizeof(HEFCTLDATA),(char*)hef->pCtlData);
    dump32(stderr,"HEF/hef",sizeof(HEF),(char*)hef);
    dump(stderr,"HEF/hef->str",hef->allocsize,hef->str);
#endif
    return 0;
}

MRESULT hef_usermCopy(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (HEFClipboardExport(hef)<0)
        return MRFROMLONG(FALSE);
    return MRFROMLONG(TRUE);
}

MRESULT hef_usermCut(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (HEFClipboardExport(hef)==0)
        HEFDeleteSelection(hef);
        else return MRFROMLONG(FALSE);
    return MRFROMLONG(TRUE);
}

MRESULT hef_usermClear(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (HEFSelectionState(hef))
        HEFDeleteSelection(hef);

    return MRFROMLONG(TRUE);
}

MRESULT hef_usermPaste(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int ret;

    ret = HEFClipboardImport(hef);
    if (ret<0) return MRFROMLONG(FALSE);
    return MRFROMLONG(TRUE);
}

MRESULT hef_usermQueryTextLimit(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->allocsize);
}

MRESULT hef_usermSetTextLimit(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int newsize;
char *buf;

    newsize = (int)mp1;
    if (DosAllocMem((PVOID)&buf,newsize,fALLOC))
        return MRFROMLONG(FALSE);

    if (hef->str!=NULL) DosFreeMem(hef->str);

    hef->str = buf;
    hef->allocsize = newsize;
    return MRFROMLONG(TRUE);
}

MRESULT hef_usermQueryChanged(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int ret;

    ret = hef->changed;
    hef->changed = FALSE;
    return MRFROMLONG(ret);
}

MRESULT hef_usermSetChanged(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int ret;

    ret = hef->changed;
    hef->changed = SHORT1FROMMP(mp1);
    return MRFROMLONG(ret);
}

MRESULT hef_usermQueryFirstChar(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->beginCol);
}

MRESULT hef_usermSetFirstChar(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int col = SHORT1FROMMP(mp1);

    if (col>strlen(hef->str)) return MRFROMLONG(FALSE);

    hef->beginCol = SHORT1FROMMP(mp1);

    return MRFROMLONG(TRUE);
}

MRESULT hef_usermQueryReadonly(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return MRFROMLONG(hef->readonly);
}

MRESULT hef_usermSetReadonly(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int ret = hef->readonly;

    hef->readonly = SHORT1FROMMP(mp1);
    return MRFROMLONG(ret);
}

MRESULT hef_usermQuerySel(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (HEFSelectionState(hef))
        {
        if (hef->anchorStx > hef->curStx)
            return MRFROM2SHORT(hef->anchorStx,hef->curStx);
            else
            return MRFROM2SHORT(hef->curStx,hef->anchorStx);
        }
    return MRFROM2SHORT(hef->curStx,hef->curStx);
}

MRESULT hef_usermSetSel(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
USHORT minSel = SHORT1FROMMP(mp1);
USHORT maxSel = SHORT2FROMMP(mp1);

    if (minSel>strlen(hef->str)) return MRFROMLONG(FALSE);
    if (maxSel>strlen(hef->str)) maxSel = strlen( hef->str );

    HEFBeginSelectionHere(hef,minSel);
    HEFWND_Notify(hwnd,HEN_BEGINSELECTION,0);
    hef->curStx = maxSel;

    return MRFROMLONG(TRUE);
}

MRESULT hef_usermQueryHanType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

#ifdef DEBUG
    assert(hef!=NULL);
    assert(hef->pCtlData!=NULL);
#endif

    return MRFROMLONG(hef->pCtlData->han_type);
}

MRESULT hef_usermSetHanType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

#ifdef DEBUG
    assert(hef!=NULL);
    assert(hef->pCtlData!=NULL);
#endif

    hef->pCtlData->han_type = (int)mp1;

    return 0;
}


