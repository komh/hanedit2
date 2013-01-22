#include "HEF_internal.h"

#include "../hchlb/hchlb.h"

#define HEF_DEFAULT_SEPARATORS  (" \t;:,./\\\"\'()[]{}<>+-_|=")
#define HEF_MINIMAL_SEPARATORS  (" \t")

static MRESULT hefc_wmButton1Click(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmButton1Down(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmButton2Down(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmChar(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmControl(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmCreate(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmMouseMove(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmButton1MotionStart(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmButton1MotionEnd(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_wmTimer(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_dmDragOver(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hefc_dmDrop(HWND hwnd,MPARAM mp1,MPARAM mp2);

MRESULT hefc_usermRefresh(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT hefc_usermRefreshCursor(HWND hwnd,MPARAM mp1,MPARAM mp2);

MRESULT APIENTRY HEFClientWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case HEMC_REFRESH:              return hefc_usermRefresh(hwnd,mp1,mp2);
    case HEMC_REFRESHCURSOR:        return hefc_usermRefreshCursor(hwnd,mp1,mp2);

    case WM_BUTTON1CLICK:           return hefc_wmButton1Click(hwnd,mp1,mp2);
    case WM_BUTTON1DOWN:            return hefc_wmButton1Down(hwnd,mp1,mp2);
    case WM_BUTTON2DOWN:            return hefc_wmButton2Down(hwnd,mp1,mp2);
    case WM_CHAR:                   return hefc_wmChar(hwnd,mp1,mp2);
    case WM_CONTROL:                return hefc_wmControl(hwnd,mp1,mp2);
    case WM_CREATE:                 return hefc_wmCreate(hwnd,mp1,mp2);
    case WM_DESTROY:                return hefc_wmDestroy(hwnd,mp1,mp2);
    case WM_MOUSEMOVE:              return hefc_wmMouseMove(hwnd,mp1,mp2);
    case WM_BUTTON1MOTIONSTART:     return hefc_wmButton1MotionStart(hwnd,mp1,mp2);
    case WM_BUTTON1MOTIONEND:       return hefc_wmButton1MotionEnd(hwnd,mp1,mp2);
    case WM_SETFOCUS:               return hefc_wmSetFocus(hwnd,mp1,mp2);
    case WM_SIZE:                   return hefc_wmSize(hwnd,mp1,mp2);
    case WM_PAINT:                  return hefc_wmPaint(hwnd,mp1,mp2);
    case WM_TIMER:                  return hefc_wmTimer(hwnd,mp1,mp2);
    case WM_QUERYCONVERTPOS:        return MRFROMSHORT( QCP_NOCONVERT );
    case DM_DRAGOVER:               return hefc_dmDragOver(hwnd,mp1,mp2);
    case DM_DROP:                   return hefc_dmDrop(hwnd,mp1,mp2);
    default:                        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
}

MRESULT hefc_wmCreate(HWND hwnd, MPARAM mp1,MPARAM mp2)
{
PCREATESTRUCT pCreate = (PCREATESTRUCT)mp2;
HEF* hef = mp1;

#ifdef DEBUG
    printf("HEFClient:: WM_CREATE\n");
    assert(hef!=NULL);
    assert(hef->pCtlData!=NULL);
#endif

/*  if (hef->pCtlData->hwndHIA == NULLHANDLE)
        {
        hef->hwndHIA = HIACreateHanAutomata(hwnd,HEFCID_HIA);
        } else {
        hef->hwndHIA = hef->pCtlData->hwndHIA;
        } */
    hef->hwndHIA = hef->pCtlData->hwndHIA;
    if (hef->hwndHIA==NULLHANDLE)
        {
        if (!(pCreate->flStyle & HES_NOAUTOCREATEHIA))
            hef->hwndHIA = HIACreateHanAutomata(hwnd,HEFCID_HIA);
        } else {
        WinPostMsg(hef->hwndHEF,HEM_SETHIA,MPFROMLONG(hef->hwndHIA),0);
        }
    hef->connectedToHIA = FALSE;
    hef->hwndClient = hwnd;
    hef->hchComposing = 0;
    hef->curStx = 0;
    hef->anchorStx = 0;
    hef->selection = FALSE;
    hef->beginCol = 0;
    hef->changed = FALSE;
    hef->skipFocusNotify = FALSE;
    hef->xSize = (pCreate->cx+7)/8;
    hef->outputBuf = malloc(hef->allocsize + hef->xSize);
#ifdef DEBUG
    assert(hef->outputBuf!=NULL);
#endif

    WinSetWindowPtr(hwnd,WINWORD_INSTANCE,hef);

#ifdef DEBUG
//  printf("HEFClient::WM_CREATE\n");
//  printf("HWND = %x\n",hwnd);
//  printf("HEF  = %x\n",hef);
#endif

    return FALSE;
}

MRESULT hefc_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
//HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    return 0L;
}

MRESULT hefc_wmSize(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
USHORT ncx,ncy;
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
char *buf = NULL;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    ncx = SHORT1FROMMP(mp2);
    ncy = SHORT2FROMMP(mp2);
    hef->xSize = (ncx+7)/8;
#ifdef DEBUG
    assert(hef->outputBuf!=NULL);
#endif
    buf = malloc(hef->allocsize + hef->xSize);
#ifdef DEBUG
    assert(buf!=NULL);
#endif
    if (buf!=NULL)
        {
        free(hef->outputBuf);
        hef->outputBuf = buf;
        } else {}

    return 0L;
}

MRESULT hefc_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (SHORT1FROMMP(mp2))
        {
        if( !hef->skipFocusNotify )
            HEFWND_Client_Notify(hwnd,HEN_SETFOCUS,0);
        if (hef->hwndHIA!=NULLHANDLE)
            WinSendMsg(hef->hwndHIA,HIAM_CONNECT,MPFROMLONG(hwnd),MPFROMLONG(HEFCID_HIA));
        WinPostMsg(hwnd,HEMC_REFRESH,0,0);
        } else {

        if (hef->hchComposing!=0)
            if ((hef->hwndHIA!=NULLHANDLE) && (hef->connectedToHIA))
                WinSendMsg(hef->hwndHIA,HIAM_COMPLETEHCH,0,0);
        if (HEFSelectionState(hef))
            {
            HEFEndSelection(hef);
            HEFWND_Client_Notify(hwnd,HEN_ENDSELECTION,0);
            WinPostMsg(hwnd,HEMC_REFRESH,0,0);
            }
        WinShowCursor(hwnd,FALSE);
        if( !hef->skipFocusNotify )
            HEFWND_Client_Notify(hwnd,HEN_KILLFOCUS,0);
        }
    return 0L;
}

MRESULT hefc_wmButton1Click(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
POINTS p;

    p.x = SHORT1FROMMP(mp1);
    p.y = SHORT2FROMMP(mp1);

    if (HEFSelectionState(hef))
        {
        HEFEndSelection(hef);
        HEFWND_Client_Notify(hwnd,HEN_ENDSELECTION,0);
        WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
        }

    hef->curStx = HEFQueryStx(hef,p.x);
    HEFReposition(hef);
    WinPostMsg(hwnd,HEMC_REFRESHCURSOR,0,0);

    return MRFROMLONG(TRUE);
}

MRESULT hefc_wmButton1Down(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    if (WinQueryFocus(HWND_DESKTOP)!=hwnd)
        {
        WinSetFocus(HWND_DESKTOP,hwnd);
        }
    return MRFROMLONG(TRUE);
}

MRESULT hefc_wmButton2Down(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    if (WinQueryFocus(HWND_DESKTOP)!=hwnd)
        {
        WinSetFocus(HWND_DESKTOP,hwnd);
        }
    return MRFROMLONG(TRUE);
}

MRESULT hefc_wmMouseMove(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HPOINTER hptrText;

    hptrText = WinQuerySysPointer(HWND_DESKTOP,SPTR_TEXT,FALSE);
    WinSetPointer(HWND_DESKTOP,hptrText);

    if (HEFSelectionState(hef))
        {
        RECTL rectl;
        POINTL p;

        p.x = SHORT1FROMMP(mp1);
        p.y = SHORT2FROMMP(mp1);
        WinQueryWindowRect(hwnd,&rectl);

        if (WinPtInRect(WinQueryAnchorBlock(hwnd),&rectl,&p))
            {
            LONG btn1State = WinGetKeyState(HWND_DESKTOP,VK_BUTTON1);
            if (btn1State & 0x8000)
                {
                int stx = HEFQueryStx(hef,p.x);
                if (stx!=hef->curStx)
                    {
                    hef->curStx = stx;
                    HEFReposition(hef);
                    WinPostMsg(hwnd,HEMC_REFRESH,0,0);
                    }
                }
            }
        }
    return 0;
}

MRESULT hefc_wmTimer(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
//HAB hab = WinQueryAnchorBlock(hwnd);
    if ( (ULONG)mp1 == HEFCID_TIMER)
        {
        HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
        RECTL rectl;
        POINTL p;
        SWP swp;

        if (!WinQueryPointerPos(HWND_DESKTOP,&p)) return 0;
        if (!WinQueryWindowPos(hwnd,&swp)) return 0;

        WinMapWindowPoints(HWND_DESKTOP,hwnd,&p,1);

//  printf("x,y = %d,%d\t (%d,%d)-(%d,%d)\n",p.x,p.y,swp.x,swp.y,swp.x+swp.cx,swp.y+swp.cy);

        rectl.xLeft = swp.x;
        rectl.xRight = swp.x + swp.cx;
        rectl.yBottom = swp.y;
        rectl.yTop = swp.y + swp.cy;

//  if (!WinPtInRect(hab,&rectl,&p))
        if (!((p.x >= swp.x) && (p.x <= swp.x+swp.cx) &&
            (p.y >= swp.y) && (p.y <= swp.y+swp.cy)))
            {
            int stx = HEFQueryStx(hef,p.x-swp.x);
            if (stx!=hef->curStx)
                {
                hef->curStx = stx;
                HEFReposition(hef);
                WinPostMsg(hwnd,HEMC_REFRESH,0,0);
                }
            }
        #ifdef DEBUG
//      printf(".");
        #endif
        } else WinDefWindowProc(hwnd,WM_TIMER,mp1,mp2);
    return 0;
}

MRESULT hefc_wmButton1MotionStart(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HAB hab = WinQueryAnchorBlock(hwnd);
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
POINTS p;
int stx;

    p.x = SHORT1FROMMP(mp1);
    p.y = SHORT2FROMMP(mp1);
    stx = HEFQueryStx(hef,p.x);
    HEFBeginSelectionHere(hef,stx);
    HEFWND_Client_Notify(hwnd,HEN_BEGINSELECTION,0);
    hef->curStx = stx;
    WinPostMsg(hwnd,HEMC_REFRESH,0,0);

    WinSetCapture(HWND_DESKTOP,hwnd);
    WinStartTimer(hab,hwnd,HEFCID_TIMER,50);
    return MRFROMLONG(TRUE);
}

MRESULT hefc_wmButton1MotionEnd(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HAB hab = WinQueryAnchorBlock(hwnd);
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    HEFReposition(hef);
    WinPostMsg(hwnd,HEMC_REFRESH,0,0);

    WinSetCapture(HWND_DESKTOP,NULLHANDLE);
    WinStopTimer(hab,hwnd,HEFCID_TIMER);

    return MRFROMLONG(TRUE);
}

MRESULT hefc_wmChar ( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
USHORT  fsFlags = SHORT1FROMMP(mp1);
UCHAR   ucVkey  = CHAR3FROMMP(mp2);

ULONG   flStyle = 0;
ULONG   flHIAState = 0;
int oldStx;

BOOL    consumed = FALSE;

#ifdef DEBUG
    assert(hef!=NULL);
//  printf("Client:: WM_CHAR\n");
#endif

    if (hef->hwndHIA!=NULLHANDLE)
        flHIAState = (ULONG) WinSendMsg(hef->hwndHIA,HIAM_QUERYSTATE,0L,0L);
    flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);

    if (fsFlags & KC_KEYUP) return MRFROMLONG(FALSE);
    WinShowCursor(hwnd,FALSE);
    HEFReposition(hef);

    if (hef->connectedToHIA)
        {
        if (!(hef->readonly) &&
            !((fsFlags& KC_VIRTUALKEY)&&(ucVkey==VK_TAB)))
            {
            if(( fsFlags & KC_VIRTUALKEY ) &&
               (( ucVkey == VK_F4 ) || ( ucVkey == VK_F9 )))
                hef->skipFocusNotify = TRUE;

            consumed = (BOOL)WinSendMsg(hef->hwndHIA,WM_CHAR,mp1,mp2);
            hef->skipFocusNotify = FALSE;
            }
        }

    oldStx = hef->curStx;

    if (fsFlags & KC_VIRTUALKEY)
    {
    switch (ucVkey) {
    case VK_HOME:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            hef->curStx = 0;
            HEFReposition(hef);
            consumed=TRUE;
            }
        break;
    case VK_END:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            hef->curStx = strlen(hef->str);
            HEFReposition(hef);
            consumed=TRUE;
            }
        break;
    case VK_LEFT:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HEFMoveLeft(hef);
            consumed=TRUE;
            }
        if (FKC_CTRLONLY(fsFlags))
            {
            HEFPrevWord(hef);
            HEFReposition(hef);
            consumed=TRUE;
            }
        break;
    case VK_RIGHT:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HEFMoveRight(hef);
            consumed=TRUE;
            }
        if (FKC_CTRLONLY(fsFlags))
            {
            HEFNextWord(hef);
            HEFReposition(hef);
            consumed=TRUE;
            }
        break;
    case VK_BACKSPACE:
        if (consumed) break;
        if (FKC_NONE(fsFlags))
            {
            if (hef->readonly) break;
            if (HEFSelectionState(hef))
                {
                HEFDeleteSelection(hef);
                HEFReposition(hef);
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                } else {
                if (hef->curStx>0)
                    {
                    hch_decStx(hef->str,&(hef->curStx));
                    hch_deleteHchStx(hef->str,hef->curStx);
                    HEFSetChanged(hef);
                    HEFReposition(hef);
                    WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                    }
                }
            consumed=TRUE;
            }
        break;
    case VK_DELETE:
        if (consumed) break;
        if (FKC_NONE(fsFlags))
            {
            if (hef->readonly) break;
            if (HEFSelectionState(hef))
                {
                HEFDeleteSelection(hef);
                } else {
                if (hef->curStx < strlen(hef->str))
                    {
                    hch_deleteHchStx(hef->str,hef->curStx);
                    HEFSetChanged(hef);
                    }
                }
            HEFReposition(hef);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
            consumed=TRUE;
            }
        if (FKC_SHIFTONLY(fsFlags))
            {
            if (hef->readonly) break;
            if (HEFSelectionState(hef))
                {
                if (HEFClipboardExport(hef)==0)
                    HEFDeleteSelection(hef);
                HEFReposition(hef);
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                }
            consumed=TRUE;
            }
        break;

    case VK_INSERT:
        if (consumed) break;
        if (FKC_CTRLONLY(fsFlags))
            {
            if (HEFSelectionState(hef))
                HEFClipboardExport(hef);
            consumed=TRUE;
            }
        if (FKC_SHIFTONLY(fsFlags))
            {
            if (hef->readonly) break;
            HEFClipboardImport(hef);
            consumed=TRUE;
            }
        break;
    } // switch

//  printf("1st\t");

    switch (ucVkey) {
    case VK_HOME:
    case VK_END:
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_PAGEUP:
    case VK_PAGEDOWN:
    case VK_PAGEDOWN + 0x90:
        if (FKC_HASSHIFT(fsFlags))
            {
            if (!HEFSelectionState(hef))
                {
                HEFBeginSelectionHere(hef,oldStx);
                } else {
                if (hef->anchorStx==hef->curStx)
                    {
                    HEFEndSelection(hef);
                    }
                }
            WinPostMsg(hwnd,HEMC_REFRESH,0,0);
            } else {
            if (HEFSelectionState(hef))
                {
                HEFEndSelection(hef);
                WinPostMsg(hwnd,HEMC_REFRESH,0,0);
                }
            }
    } // switch
    } // if Virtualkey

//  printf("2nd\n");

    if (!consumed)
        WinPostMsg(WinQueryWindow(hwnd,QW_OWNER),WM_CHAR,mp1,mp2);

    WinSendMsg(hwnd,HEMC_REFRESHCURSOR,0L,0L);

//  printf("HEF:: WM_CHAR end\n");
    return MRFROMLONG(consumed); // msg processed
}

MRESULT hefc_wmControl(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
ULONG   flHIAState = 0;

//  printf("Client:: WM_CONTROL\n");
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (hef->hwndHIA!=NULLHANDLE)
        flHIAState = (ULONG) WinSendMsg(hef->hwndHIA,HIAM_QUERYSTATE,0L,0L);

    switch (SHORT1FROMMP(mp1)) { // control

    case HEFCID_HIA:
        switch (SHORT2FROMMP(mp1)) { // Notification: mp2/HIWORD = Completed hch , mp2/LOWORD = Composing hch

        case HIAN_INSERTHCH:
            if (!(SHORT1FROMMP(mp2) & 0x8000))
            {
                if (HEFSelectionState(hef))
                    {
                    HEFDeleteSelection(hef);
                    WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                    }
                if (!(flHIAState & HIAST_INSERTMODE))
                    if (!(hef->curStx == strlen(hef->str)))
                        {
                        hch_deleteHchStx(hef->str,hef->curStx);
                        HEFSetChanged(hef);
                        }
                HEFInsertHch(hef,SHORT1FROMMP(mp2));
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                HEFReposition(hef);
            }
            break;
        case HIAN_COMPO_BEGIN:
            hef->hchComposing = 0;
            if (HEFSelectionState(hef))
                {
                HEFDeleteSelection(hef);
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                }
            if (!(flHIAState & HIAST_INSERTMODE))
                if (!(hef->curStx == strlen(hef->str)))
                    {
                    hch_deleteHchStx(hef->str,hef->curStx);
                    HEFSetChanged(hef);
                    }
            hef->hchComposing = SHORT2FROMMP(mp2);
            HEFReposition(hef);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
            break;
        case HIAN_COMPO_STEP:
            hef->hchComposing = SHORT2FROMMP(mp2);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
            break;
        case HIAN_COMPO_STEPBACK:
            hef->hchComposing = SHORT2FROMMP(mp2);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
            break;
        case HIAN_COMPO_CANCEL:
            hef->hchComposing = 0;
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
            break;
        case HIAN_COMPO_COMPLETE:
            hef->hchComposing = 0;
            HEFInsertHch(hef,SHORT1FROMMP(mp2));
            HEFReposition(hef);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
            break;
        case HIAN_HANMODECHANGED:
            HEFStatNotify(hef,HEN_CHANGEDHANINPUTMODE,mp2);
            break;
        case HIAN_KBDTYPECHANGED:
            HEFStatNotify(hef,HEN_CHANGEDKBDTYPE,mp2);
            break;
        case HIAN_INSERTMODECHANGED:
            HEFStatNotify(hef,HEN_CHANGEDINSERTMODE,mp2);
            break;
        case HIAN_CONNECT:
            hef->connectedToHIA = (int)mp2;
            break;
        case HIAN_HGHJCONVERT:
        {
            HANCHAR  hch;

/*
            if( SHORT1FROMMP( mp2 ))
                HEFMoveLeft( hef );
*/
            hch = hef->str[ hef->curStx ];

            if( hch < 0x80 )
            {
/*
                if( SHORT1FROMMP( mp2 ))
                    HEFMoveRight( hef );
*/
                break;
            }

            if(( hch >= 0xd9 ) && ( hch <= 0xde ))
                break;

            hch = HCHFROM2CH( hch, hef->str[ hef->curStx + 1 ]);

            if( hch >= 0xe000 )
                hch = hch_hj2hg( hch );
            else
                hch = hjselDlg( HWND_DESKTOP, hwnd, NULLHANDLE, hch );

            if( hch != HCH_SINGLE_SPACE )
            {
                hch_deleteHchStx( hef->str, hef->curStx );
                HEFInsertHch( hef, hch );
                HEFSetChanged( hef );
                HEFReposition( hef );
                WinInvalidateRect( hwnd, NULLHANDLE, FALSE );
            }
            break;
        }


        } // note switch end
        break;
    } // control switch end
    return 0L;
}

MRESULT hefc_wmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HPS hps;
RECTL rectlUpdate;
RECTL rectlEntire;
//POINTL p;
//ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
//char *str;

    hps=WinBeginPaint(hwnd, NULLHANDLE, &rectlUpdate);
#ifdef DEBUG
    assert(hps!=NULLHANDLE);
//  printf("HEF::WM_PAINT\n");
#endif
    if (hps==NULLHANDLE) return 0;

    WinQueryWindowRect(hwnd,&rectlEntire);


//  WinFillRect(hps,&rectlUpdate,hef->bgColor);
    rectlUpdate.yTop = rectlEntire.yTop;
    rectlUpdate.yBottom = 16;
    rectlUpdate.xLeft = 0;
    rectlUpdate.xRight = rectlEntire.xRight;
    WinFillRect(hps,&rectlUpdate,hef->bgColor);

    if (hef->outputBuf==NULL)
        {
        WinEndPaint(hps);
        WinPostMsg(hwnd,HEMC_REFRESHCURSOR,0,0);
        return 0;
        }
    HEFFormatLine(hef,hef->outputBuf,hef->beginCol+hef->xSize);

    GpiSetBackMix(hps,BM_OVERPAINT);

    if (HEFSelectionState(hef))
        {
        int beginStx,endStx;
        if (hef->curStx > hef->anchorStx)
            {
            beginStx = hef->anchorStx;
            endStx = hef->curStx;
            } else {
            beginStx = hef->curStx;
            endStx = hef->anchorStx;
            }

        GpiSetColor(hps,hef->fgColor);
        GpiSetBackColor(hps,hef->bgColor);

        if (beginStx > hef->beginCol)
            HanOutOfs(hps,0,0,hef->outputBuf,hef->beginCol,beginStx - hef->beginCol);
        if (endStx > hef->beginCol)
            HanOutOfs(hps,(endStx-hef->beginCol)*8,0,hef->outputBuf,endStx,hef->beginCol+hef->xSize-endStx);

        GpiSetColor(hps,hef->selfgColor);
        GpiSetBackColor(hps,hef->selbgColor);
        HanOutOfs(hps,(beginStx-hef->beginCol)*8,0,hef->outputBuf,beginStx,endStx-beginStx);
        } else {
        GpiSetColor(hps,hef->fgColor);
        GpiSetBackColor(hps,hef->bgColor);
        HanOutOfs(hps,0,0,hef->outputBuf,hef->beginCol,hef->xSize);
        }

    WinEndPaint(hps);
    WinPostMsg(hwnd,HEMC_REFRESHCURSOR,0,0);

    return 0L;
}

MRESULT hefc_dmDragOver(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG flStyle;
PDRAGINFO pDraginfo = (PDRAGINFO)mp1;
PDRAGITEM pItem = NULL;
USHORT usDrop=DOR_NEVERDROP,usDefaultOp=DO_MOVE;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    flStyle = WinQueryWindowULong(hef->hwndHEF,QWL_STYLE);
    if (!(flStyle & HES_ACCEPTFILEDROP)) return MRFROM2SHORT(DOR_NEVERDROP,0);

do  {
    if (!DrgAccessDraginfo(pDraginfo))
        {
        pDraginfo = NULL;
        break;
        }
    usDrop = DOR_DROP;
    switch (pDraginfo->usOperation)
        {
        case DO_MOVE:
            {
            #ifdef DEBUG
            printf("DO_MOVE : DO_MOVE\n");
            #endif
            usDefaultOp = DO_MOVE;
            } break;
        case DO_COPY:
            {
            #ifdef DEBUG
            printf("DO_COPY : DO_COPY\n");
            #endif
            usDefaultOp = DO_COPY;
            } break;
        case DO_DEFAULT:
            {
            #ifdef DEBUG
            printf("DO_DEFAULT : DO_MOVE\n");
            #endif
            usDefaultOp = DO_MOVE;
            } break;
        case DO_UNKNOWN:
            {
            #ifdef DEBUG
            printf("DO_UNKNOWN : DO_MOVE\n");
            #endif
            usDefaultOp = DO_MOVE;
            } break;
        default:
            {
            usDrop = DOR_NODROPOP;
            #ifdef DEBUG
            printf("DOR_NODROPOP\n");
            #endif
            } break;
        }
    if ((pItem = DrgQueryDragitemPtr(pDraginfo,0))!=NULL)
        {
        if (!DrgVerifyRMF(pItem,"DRM_OS2FILE", NULL))
            {
            usDrop = DOR_NEVERDROP;
            #ifdef DEBUG
            printf("DOR_NEVERDROP\n");
            #endif
            break;
            }
        }
    } while (FALSE);
    if (pDraginfo!=NULL) DrgFreeDraginfo(pDraginfo);
    return MRFROM2SHORT(usDrop,usDefaultOp);

}

MRESULT hefc_dmDrop(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PDRAGINFO pDraginfo = (PDRAGINFO)mp1;
PDRAGITEM pItem = NULL;

    if (DrgAccessDraginfo(pDraginfo))
        {
        HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
        char containerName[CCHMAXPATH];
        char sourceName[CCHMAXPATH];
        char str[CCHMAXPATH];

        if ((pItem = DrgQueryDragitemPtr(pDraginfo,0))==NULL) return 0;
        DrgQueryStrName(pItem->hstrContainerName,CCHMAXPATH,containerName);
        DrgQueryStrName(pItem->hstrSourceName,CCHMAXPATH,sourceName);
        strcpy(str,containerName);
        strcat(str,sourceName);
        #ifdef DEBUG
        printf("%s\n",str);
        #endif
        WinSetWindowText(hef->hwndHEF,str);
        HEFSetChanged(hef);
        WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
        DrgFreeDraginfo(pDraginfo);
        }
    return 0;
}

MRESULT hefc_usermRefresh(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
//  printf("HEFClient::Refresh\n");
    WinInvalidateRect(hwnd,NULL,TRUE);
    WinSendMsg(hwnd,HEMC_REFRESHCURSOR,0L,0L);
    return 0L;
}

MRESULT hefc_usermRefreshCursor( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HWND    hwndHIA = hef->hwndHIA;
RECTL   rectl,rectlClip;
ULONG   shape = 0,xsize = 0;
ULONG   flHIA=0;

#ifdef DEBUG
//  printf("HEFC::REFRESHCURSOR\n");
#endif

    if (hef->hwndHIA!=NULLHANDLE)
        flHIA = (ULONG)WinSendMsg(hwndHIA,HIAM_QUERYSTATE,0L,0L);
        else
        return 0;

    if (WinQueryFocus(HWND_DESKTOP) == hwnd)
        {
        WinQueryWindowRect( hwnd, &rectlClip );
        rectl.yBottom = 0;
        rectl.xLeft = (hef->curStx - hef->beginCol)*8;

        if (hef->hchComposing)
            {
            shape = CURSOR_FRAME;
            xsize = 16;
            } else if (flHIA & HIAST_INSERTMODE)
                {
                if (flHIA & HIAST_HANMODE)
                    {
                    shape = CURSOR_FLASH;
                    xsize = 2;
                    } else {
                    shape = CURSOR_FLASH;
                    xsize = 1;
                    }
                } else {
                if (hch_queryHchType(hef->str,hef->curStx)==HCH_HAN)
                    {
                    shape = CURSOR_FLASH;
                    xsize = 16;
                    } else {
                    shape = CURSOR_FLASH;
                    xsize = 8;
                    }
                }
        WinCreateCursor( hwnd, rectl.xLeft, rectl.yBottom,
            xsize,16,
            shape, &rectlClip );

        WinShowCursor(hwnd,TRUE);

        } else {
        if (mp1) WinSetFocus(HWND_DESKTOP,hwnd);
        }
    return MRFROMLONG(0L);
}

void HEFWND_Client_Notify(HWND hwnd,USHORT notifCode,MPARAM mp2)
{
HEF *hef = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HWND hwndOwner;
USHORT usId;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    hwndOwner = WinQueryWindow(hef->hwndHEF,QW_OWNER);
    usId = WinQueryWindowUShort(hef->hwndHEF,QWS_ID);

    WinPostMsg(hwndOwner,WM_CONTROL,
        MPFROM2SHORT(usId,notifCode),
        mp2);
}

void HEFStatNotify(HEF *hef,USHORT notifCode,MPARAM mp2)
{
ULONG flStyle;
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    flStyle = WinQueryWindowULong(hef->hwndHEF,QWL_STYLE);
    if (flStyle & HES_STATUSNOTIFY)
        HEFWND_Client_Notify(hef->hwndClient,notifCode,mp2);
}

void HEFBeginSelectionHere(HEF *hef,int stx)
{
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    hef->selection = TRUE;
    hef->anchorStx = stx;
    HEFWND_Client_Notify(hef->hwndClient,HEN_BEGINSELECTION,0);
}

void HEFEndSelection(HEF *hef)
{
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    hef->selection = FALSE;
    HEFWND_Client_Notify(hef->hwndClient,HEN_ENDSELECTION,0);
}

void HEFDeleteSelection(HEF *hef)
{
char *str;
int beginStx,endStx;
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (!HEFSelectionState(hef)) return;
    if (hef->curStx > hef->anchorStx)
        {
        beginStx = hef->anchorStx;
        endStx = hef->curStx;
        } else {
        beginStx = hef->curStx;
        endStx = hef->anchorStx;
        }
    str = malloc(strlen(hef->str)+1);
    strncpy(str,hef->str,beginStx);
    strcpy(str+beginStx,hef->str+endStx);
    strcpy(hef->str,str);
    free(str);
    hef->selection = FALSE;
    hef->curStx = beginStx;
    HEFWND_Client_Notify(hef->hwndClient,HEN_ENDSELECTION,0);
    HEFSetChanged(hef);
}

int HEFSelectionSize(HEF *hef)
{
int beginStx,endStx;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (!HEFSelectionState(hef)) return 0;

    if (hef->anchorStx > hef->curStx)
        {
        endStx = hef->anchorStx;
        beginStx = hef->curStx;
        } else {
        endStx = hef->curStx;
        beginStx = hef->anchorStx;
        }
    return (endStx-beginStx);
}

char *HEFGetSelection(HEF *hef,char *buf)
{
int beginStx,endStx;
int size;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (!HEFSelectionState(hef)) return NULL;
    if (hef->anchorStx > hef->curStx)
        {
        endStx = hef->anchorStx;
        beginStx = hef->curStx;
        } else {
        endStx = hef->curStx;
        beginStx = hef->anchorStx;
        }
    size = HEFSelectionSize(hef);
    if (size==0) return NULL;
    strncpy(buf,hef->str+beginStx,size);
    buf[size] = 0;
    return buf;
}

int HEFClipboardExport(HEF *hef)
{
HAB hab;
ULONG bufsize;
BYTE  *buf;
ULONG fSuccess;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (hef->unreadable) return -1;

    if (!HEFSelectionState(hef)) return -1;
    hab = WinQueryAnchorBlock(hef->hwndHEF);

    bufsize = HEFSelectionSize(hef) + 1;

    if (WinOpenClipbrd(hab))
        {
        if (!(fSuccess = DosAllocSharedMem((PVOID)&buf,NULL,
                bufsize,PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE)))
            {
            HEFGetSelection(hef,buf);
//          strcpy(buf,hef->str);

            if (hef->pCtlData->han_type == HEF_HAN_KS)
                hch_sy2ksstr(buf);

            WinEmptyClipbrd(hab);

            fSuccess = WinSetClipbrdData(hab,(ULONG)buf,
                    CF_TEXT,CFI_POINTER);

            DosFreeMem(buf);
            WinCloseClipbrd(hab);
            }
        }
    return 0;
}

int HEFClipboardImport(HEF *hef)
{
HAB hab;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

//  printf("HEF::ClipboardImport \n");
    hab = WinQueryAnchorBlock(hef->hwndHEF);
    if (HEFSelectionState(hef))
        HEFDeleteSelection(hef);

    if (WinOpenClipbrd(hab))
        {
        BYTE *clipbuf = (BYTE*) WinQueryClipbrdData(hab,CF_TEXT);
        #ifdef DEBUG
        assert(clipbuf!=NULL);
        #endif
        if (clipbuf!=NULL)
            {
            int ret;
            int size = strlen(clipbuf)+1;
            char *buf = malloc(size);
            #ifdef DEBUG
                assert(buf!=NULL);
            #endif
            if (buf==NULL)
                {
                WinCloseClipbrd(hab);
                return -1;
                }
            strcpy(buf,clipbuf);
            if (hef->pCtlData->han_type == HEF_HAN_KS)
                hch_ks2systr(buf);
            ret = HEFInsert(hef,buf);
            free(buf);
            if (ret<0)
                {
                WinCloseClipbrd(hab);
                return -1;
                }
            hef->curStx += ret;
            }
        WinCloseClipbrd(hab);
        }
    HEFReposition(hef);
    WinPostMsg(hef->hwndHEF,HEM_REFRESH,0,0);
    HEFSetChanged(hef);
//    printf("// HMLEClipboardImport()\n");
    return 0;
}

void HEFReposition(HEF *hef)
{
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (hef->curStx<hef->beginCol)
        {
        while (hef->curStx < hef->beginCol)
            if (hef->beginCol>5)
                hef->beginCol -= 5;
                else
                hef->beginCol = 0;
        WinPostMsg(hef->hwndHEF,HEM_REFRESH,0,0);
        }
    if (hef->curStx >= hef->beginCol+hef->xSize-1)
        {
        while (hef->curStx >= hef->beginCol+hef->xSize-1)
            hef->beginCol+=5;
        if (hef->beginCol > strlen(hef->str)+1)
            hef->beginCol = strlen(hef->str)+1;
        WinPostMsg(hef->hwndHEF,HEM_REFRESH,0,0);
        }
}

void HEFMoveLeft(HEF *hef)
{
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (hef->curStx>0) hch_decStx(hef->str,&(hef->curStx));
    HEFReposition(hef);
}

void HEFMoveRight(HEF *hef)
{
#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (hef->curStx<strlen(hef->str)) hch_incStx(hef->str,&(hef->curStx));
    HEFReposition(hef);
}

void HEFNextWord(HEF* hef)
{
    while ((hef->str[hef->curStx]!='\0')&&
        (strchr(HEF_DEFAULT_SEPARATORS,hef->str[hef->curStx])==NULL)) hef->curStx++;
    while ((hef->str[hef->curStx]!='\0')&&
        (strchr(HEF_DEFAULT_SEPARATORS,hef->str[hef->curStx])!=NULL)) hef->curStx++;
    return;
}

void HEFPrevWord(HEF *hef)
{
    while ((hef->curStx>0)&&
        (strchr(HEF_DEFAULT_SEPARATORS,hef->str[hef->curStx-1])!=NULL)) hef->curStx--;
    while ((hef->curStx>0)&&
        (strchr(HEF_DEFAULT_SEPARATORS,hef->str[hef->curStx-1])==NULL)) hef->curStx--;
    return;
}

void HEFFormatLine(HEF *hef,char *str,int fixedLen)
{
int i;

    if (hef->unreadable)
        {
        for (i=strlen(hef->str);i>0;i--)
            str[i-1]='*';
        str[strlen(hef->str)] = 0;
        if (hef->hchComposing)
            strcat(str,"**");
        } else {
        strcpy(str,hef->str);
        if (hef->hchComposing)
            hch_insertHchStx(str,hef->hchComposing,hef->curStx);
        }

    fixedLen++;
    for (i=strlen(str);i<=fixedLen;i++)
        str[i]=' ';
    hch_alignStx(str,&fixedLen);
    str[fixedLen]=0;

}

int HEFInsert(HEF *hef,const char* str)
{
int count;
char *buf;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (str==NULL) return 0;
    count = strlen(str);

    buf = malloc(hef->allocsize);
#ifdef DEBUG
    assert(buf!=NULL);
#endif
    if (buf==NULL) return -1;

    if (strlen(hef->str)+count+1 > hef->allocsize)
        return -1;
//      count = hef->allocsize - strlen(hef->str)-1;
    strncpy(buf,hef->str,hef->curStx); buf[hef->curStx] = 0;
    strncat(buf,str,count);
    strcat(buf,hef->str+hef->curStx);
    strcpy(hef->str,buf);

    free(buf);

    HEFSetChanged(hef);
    return count;
}

int HEFInsertHch(HEF *hef,HANCHAR hch)
{
int ret;

#ifdef DEBUG
    assert(hef!=NULL);
#endif

    if (hch==0) return 0;

    if (ISHCH(hch))
        {
        if (strlen(hef->str)+2 >= hef->allocsize) return -1;
        } else {
        if (strlen(hef->str)+1 >= hef->allocsize) return -1;
        }
//  printf("<%d>",hef->allocsize);
    ret = hch_insertHchStx(hef->str,hch,hef->curStx);
    hef->curStx += ret;
    HEFSetChanged(hef);
    return ret;
}

void HEFSetChanged(HEF *hef)
{
    hef->changed = TRUE;
    HEFWND_Notify(hef->hwndHEF,HEN_CHANGE,0);
}

int HEFQueryStx(HEF *hef,int x)
{
int col;
int stx;

    col = x/8;
    if (x%8 > 3) col++;

    if (col<0) col=-1;
    if (col>hef->xSize) col = hef->xSize;

    stx = hef->beginCol + col;
    if (stx<0) stx = 0;
    if (stx>strlen(hef->str)) stx = strlen(hef->str);

    hch_alignStx(hef->str,&stx);
    return stx;
}


