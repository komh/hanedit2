#include <string.h>
#include "HMLE_internal.h"
#include "../hchlb/hchlb.h"

static MRESULT hmlec_wmChar(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmCreate(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmDestroy(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmPaint(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmSetFocus(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmSize(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmControl(HWND,MPARAM,MPARAM);

static MRESULT hmlec_wmMouseMove(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmlec_wmButton1Down(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmButton2Down(HWND,MPARAM,MPARAM);
static MRESULT hmlec_wmButton1Click(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmlec_wmButton1MotionStart(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmlec_wmButton1MotionEnd(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmlec_wmTimer(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmlec_dmDragOver(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hmlec_dmDrop(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hmlec_usermRefreshCursor(HWND,MPARAM,MPARAM);

static void HMLEWNDC_Notify(HWND hwnd,USHORT notifCode,MPARAM mp2);
static void HMLEWNDC_Scroll(HWND hwnd,int xstep,int ystep);
static void HMLEWNDC_VScroll(HWND hwnd,int step);
static void HMLEWNDC_HScroll(HWND hwnd,int step);

MRESULT APIENTRY HMLEClientWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
    case HMLMC_REFRESHCURSOR:   return hmlec_usermRefreshCursor(hwnd,mp1,mp2);

    case WM_CHAR:               return hmlec_wmChar( hwnd, mp1, mp2);
    case WM_CONTROL:            return hmlec_wmControl( hwnd, mp1, mp2);
    case WM_CREATE:             return hmlec_wmCreate( hwnd, mp1, mp2 );
    case WM_DESTROY:            return hmlec_wmDestroy( hwnd, mp1, mp2 );
    case WM_MOUSEMOVE:          return hmlec_wmMouseMove(hwnd,mp1,mp2);
    case WM_PAINT:              return hmlec_wmPaint( hwnd, mp1, mp2 );
    case WM_SETFOCUS:           return hmlec_wmSetFocus(hwnd,mp1,mp2);
    case WM_SIZE:               return hmlec_wmSize( hwnd, mp1, mp2 );
    case WM_BUTTON1DOWN:        return hmlec_wmButton1Down( hwnd, mp1, mp2);
    case WM_BUTTON2DOWN:        return hmlec_wmButton2Down( hwnd, mp1, mp2);
    case WM_BUTTON3DOWN:        return hmlec_wmButton2Down( hwnd, mp1, mp2);
    case WM_BUTTON1CLICK:       return hmlec_wmButton1Click(hwnd,mp1,mp2);
    case WM_BUTTON1MOTIONSTART: return hmlec_wmButton1MotionStart(hwnd,mp1,mp2);
    case WM_BUTTON1MOTIONEND:   return hmlec_wmButton1MotionEnd(hwnd,mp1,mp2);
    case WM_TIMER:              return hmlec_wmTimer(hwnd,mp1,mp2);
    case WM_QUERYCONVERTPOS:    return MRFROMSHORT( QCP_NOCONVERT );

    case DM_DRAGOVER:           return hmlec_dmDragOver(hwnd,mp1,mp2);
    case DM_DROP:               return hmlec_dmDrop(hwnd,mp1,mp2);

    default:                    return WinDefWindowProc( hwnd, msg, mp1, mp2);
    }
}

MRESULT hmlec_wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PCREATESTRUCT pCreate = (PCREATESTRUCT) mp2;
HMLE *hmle = (HMLE*)mp1;

    hmle->runningHIA = FALSE;
    hmle->xSize = (pCreate->cx+7)/8;
    hmle->ySize = (pCreate->cy+15)/16;

    if( hmle->doc->wordWrapSizeAuto )
        hmle->doc->wordWrapSize = hmle->xSize - 2;

    if (hmle->hwndHIA==NULLHANDLE)
        {
        if (!(pCreate->flStyle & HMLS_NOAUTOCREATEHIA))
            hmle->hwndHIA = HIACreateHanAutomata(hwnd,HMLECID_HIA);
        } else {
        WinPostMsg(hmle->hwndHMLE,HMLM_SETHIA,MPFROMLONG(hmle->hwndHIA),0);
        }
    WinSetWindowPtr(hwnd,WINWORD_INSTANCE,hmle);

    return MRFROMLONG(FALSE);
}

MRESULT hmlec_wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
/*HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (hmle==NULL) return 0L;
    if (hmle->doc) HMLEDestroyDoc(hmle->doc);
*/
    return 0L;
}

MRESULT hmlec_wmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if( hmle->runningHIA )
        return 0;

    if (SHORT1FROMMP(mp2))
        {
        HMLEWNDC_Notify(hwnd,HMLN_SETFOCUS,0);
        if (hmle->hwndHIA!=NULLHANDLE)
            if (!WinSendMsg(hmle->hwndHIA,HIAM_CONNECT,MPFROMLONG(hwnd),MPFROMLONG(HMLECID_HIA)))
                {
                #ifdef DEBUG
                    printf("HMLE : FAILED to connect\n");
                    assert(FALSE);
                #endif
                }
        WinPostMsg(hwnd,HMLM_REFRESHSCROLLBAR,0L,0L);
        WinPostMsg(hmle->hwndClient,HMLMC_REFRESHCURSOR,0,0);
        WinInvalidateRect(hwnd,NULL,TRUE);
        } else {
        if (hmle->doc->hchComposing!=0)
            if ((hmle->hwndHIA!=NULLHANDLE) && (hmle->connectedToHIA))
                WinSendMsg(hmle->hwndHIA,HIAM_COMPLETEHCH,0,0);
/*      if (HMLEDocQueryMarkingState(hmle->doc))
            {
            HMLEDocEndSelection(hmle->doc);
            HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
            WinPostMsg(hwnd,HMLMC_REFRESH,0,0);
            } */
        WinShowCursor(hwnd,FALSE);
        HMLEWNDC_Notify(hwnd,HMLN_KILLFOCUS,0);
        }
    return 0L;
}

MRESULT hmlec_wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
USHORT ncx,ncy;

    ncx = SHORT1FROMMP(mp2);
    ncy = SHORT2FROMMP(mp2);

    hmle->xSize = (ncx+7)/8;
    hmle->ySize = (ncy+15)/16;

    if( hmle->doc->wordWrapSizeAuto )
       hmle->doc->wordWrapSize = hmle->xSize - 2;

    WinPostMsg(hmle->hwndHMLE,HMLM_REFRESH,0L,0L);

    return MRFROMLONG(0L);
}

MRESULT hmlec_wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//HAB hab = WinQueryAnchorBlock(hwnd);

HPS         hps;
RECTL       rectlEntire;
//RECT      rectlUpdate;
RECTL       rectlLine;
//RECTL     rectlTemp;

int         left=0,right=0;
HMLELine    *lineptr;
int         lineN;
LONG        linepos;
char        *str;

ULONG       selection = FALSE;

HMLEIpt curIpt,anchorIpt;
HMLEIpt *beginIpt = NULL,*endIpt = NULL;

//  printf("Client:: WM_PAINT..");

#ifdef DEBUG
    assert(hmle!=NULL);
#endif

    hps=WinBeginPaint(hwnd, NULLHANDLE, NULL /* &rectlUpdate*/ );
#ifdef DEBUG
    assert(hps!=NULLHANDLE);
#endif

    WinQueryWindowRect(hwnd,&rectlEntire);

    linepos = rectlEntire.yTop-16;
    lineptr = HMLEDocQueryLine(hmle->doc,hmle->beginLineN);
    lineN = hmle->beginLineN;

    str = malloc(hmle->doc->editBufSize+hmle->xSize+10);

    if (HMLEDocQueryMarkingState(hmle->doc))
        {
        HMLEDocQueryCurIpt(hmle->doc,&curIpt);
        HMLEDocQueryAnchorIpt(hmle->doc,&anchorIpt);
        beginIpt = HMLEIptBefore(&curIpt,&anchorIpt);
        endIpt = HMLEIptAfter(&curIpt,&anchorIpt);
        if ((hmle->beginLineN > beginIpt->ln)&&(hmle->beginLineN<=endIpt->ln))
            selection = TRUE;
        }

    GpiSetBackMix(hps,BM_OVERPAINT);

    while ((lineptr!=NULL) && (linepos >= -15L))
        {
//      HMLEDocFormatLine(hmle->doc,lineptr,str);
        HMLEDocFormatLine2(hmle->doc,lineptr,str,hmle->beginColN+hmle->xSize);
        #ifdef DEBUG
//      printf("<%d>,%d,%d\n",strlen(str),hmle->beginColN,hmle->xSize);
        #endif

        rectlLine.xLeft = rectlEntire.xLeft;
        rectlLine.xRight = rectlEntire.xRight;
        rectlLine.yTop = linepos+16;
        rectlLine.yBottom = linepos;

        if (HMLEDocQueryMarkingState(hmle->doc))
            {
            if (lineN == beginIpt->ln) selection = TRUE;
            if (selection)
                {
                left=hmle->beginColN;
                right=hmle->beginColN+hmle->xSize;

                if (lineN == beginIpt->ln)
                    {
                    left = HMLEDocStx2Col(hmle->doc,lineptr,beginIpt->stx);
                    if (left>hmle->beginColN)
                        {
                        GpiSetColor(hps,hmle->fgColor);
                        GpiSetBackColor(hps,hmle->bgColor);
                        HanOutOfs(hps,0,linepos,str,
                            hmle->beginColN,left-hmle->beginColN);
                        }
                    }
                if (lineN == endIpt->ln)
                    {
                    right = HMLEDocStx2Col(hmle->doc,lineptr,endIpt->stx);
                    if (right<hmle->beginColN+hmle->xSize)
                        {
                        GpiSetColor(hps,hmle->fgColor);
                        GpiSetBackColor(hps,hmle->bgColor);
                        HanOutOfs(hps,(right-hmle->beginColN)<<3,linepos,str,
                            right,hmle->beginColN+hmle->xSize - right);
                        }
                    }
                if (right>left)
                    {
                    if (left<hmle->beginColN) left=hmle->beginColN;
                    if (right>hmle->beginColN+hmle->xSize) right=hmle->beginColN+hmle->xSize;
                    GpiSetColor(hps,hmle->selfgColor);
                    GpiSetBackColor(hps,hmle->selbgColor);
                    HanOutOfs(hps,(left-hmle->beginColN)<<3,linepos,str,
                        left,right-left);
                    }
                } else {
                GpiSetColor(hps,hmle->fgColor);
                GpiSetBackColor(hps,hmle->bgColor);
                HanOutOfs(hps,0,linepos,str,hmle->beginColN,hmle->xSize);
                }
            if (lineN == endIpt->ln) selection = FALSE;
            } else {
            GpiSetColor(hps,hmle->fgColor);
            GpiSetBackColor(hps,hmle->bgColor);
            HanOutOfs(hps,0,linepos,str,hmle->beginColN,hmle->xSize);
            }


        linepos -= 16;
        lineptr = lineptr->nextLine;
        lineN ++;
        }
    if ((lineptr==NULL) && (linepos >= -15L))
        {
        rectlLine.xLeft = rectlEntire.xLeft;
        rectlLine.xRight = rectlEntire.xRight;
        rectlLine.yTop = linepos + 16;
        rectlLine.yBottom = 0;
        WinFillRect(hps,&rectlLine,hmle->bgColor);
        }

/*  GpiSetColor(hps,CLR_RED);
    GpiMove(hps,(PPOINTL)&rectlUpdate);
    GpiLine(hps,((PPOINTL)&rectlUpdate)+1); */

    free(str);
    WinEndPaint( hps );

//  printf("..paint ok\n");

    return MRFROMLONG(0L);
}

MRESULT hmlec_usermRefreshCursor( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HWND        hwndHIA = hmle->hwndHIA;
RECTL       rectl,rectlClip;
ULONG       shape = 0,xsize = 0;

//  printf("HMLEClient:: HMLMC_REFRESHCURSOR\n");

    if (hwndHIA==NULLHANDLE) return MRFROMLONG(-1);

    if(WinQueryFocus(HWND_DESKTOP) == hwnd )
        {
        ULONG flHIA = (ULONG)WinSendMsg(hwndHIA,HIAM_QUERYSTATE,0L,0L);
        WinQueryWindowRect( hwnd, &rectlClip );
        rectl.yBottom = rectlClip.yTop-16 - (HMLEQueryRelY(hmle)*16);
        rectl.xLeft = HMLEQueryRelX(hmle)*8;

        if (hmle->doc->hchComposing)
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
                if ( *(hmle->doc->curLine->str + hmle->doc->curStx) & 0x80)
                    {
                    shape = CURSOR_FLASH;
                    xsize = 16;
                    } else {
                    if ( *(hmle->doc->curLine->str + hmle->doc->curStx) == '\t')
                        {
                        shape = CURSOR_FLASH;
                        xsize = (HMLEDocQueryTabspaceStx(hmle->doc,hmle->doc->curLine,hmle->doc->curStx))<<3;
                        } else {
                        shape = CURSOR_FLASH;
                        xsize = 8;
                        }
                    }
                }
        WinCreateCursor( hwnd, rectl.xLeft, rectl.yBottom,
            xsize,16,
            shape, &rectlClip );

        WinShowCursor(hwnd,TRUE);

        } else if ( mp1 ) WinSetFocus(HWND_DESKTOP,hwnd);

    return MRFROMLONG(0L);
}

MRESULT hmlec_wmButton1Down(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    if (WinQueryFocus(HWND_DESKTOP)!=hwnd)
        {
        WinSetFocus(HWND_DESKTOP,hwnd);
        }
    return MRFROMLONG(TRUE);
}

MRESULT hmlec_wmButton2Down(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    if (WinQueryFocus(HWND_DESKTOP)!=hwnd)
        {
        WinSetFocus(HWND_DESKTOP,hwnd);
        }
    return MRFROMLONG(TRUE);
}

MRESULT hmlec_wmButton1Click(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
POINTS p;
HMLEIpt ipt;

    p.x = SHORT1FROMMP(mp1);
    p.y = SHORT2FROMMP(mp1);

    if (HMLEDocQueryMarkingState(hmle->doc)!=0)
        {
        HMLEDocEndSelection(hmle->doc);
        HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
        WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
        }
    HMLEQueryIpt(hmle,p.x,p.y,&ipt);
    if( HMLEDocQueryNumberOfLines( hmle->doc ) < ( ipt.ln + 1 ))
        ipt.ln = HMLEDocQueryNumberOfLines( hmle->doc ) - 1;
    else
    {
        HMLELine *line = HMLEDocQueryLine( hmle->doc, ipt.ln );

        if( HMLELineQueryLen( line ) < ( ipt.stx + 1 ))
            ipt.stx = HMLELineQueryLen( line );
    }
    HMLEDocSetCurIpt(hmle->doc,&ipt);

    HMLEStatNotify(hmle,HMLN_NOTIFYCURSORPOS,
        MPFROM2SHORT(ipt.ln,HMLEDocColFromStx(hmle->doc,ipt.stx)));
    WinPostMsg(hwnd,HMLMC_REFRESHCURSOR,0,0);

    return MRFROMLONG(TRUE);
}

MRESULT hmlec_wmButton1MotionStart(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
POINTS p;
HMLEIpt ipt;

    p.x = SHORT1FROMMP(mp1);
    p.y = SHORT2FROMMP(mp1);
    HMLEQueryIpt(hmle,p.x,p.y,&ipt);
    HMLEDocBeginSelectionHere(hmle->doc,&ipt);
    HMLEWNDC_Notify(hwnd,HMLN_BEGINSELECTION,0);
    HMLEDocSetCurIpt(hmle->doc,&ipt);
    WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
    WinPostMsg(hwnd,HMLMC_REFRESHCURSOR,0,0);

    WinSetCapture(HWND_DESKTOP,hwnd);

    WinStartTimer(WinQueryAnchorBlock(hwnd),hwnd,HMLECID_TIMER,50);

    return MRFROMLONG(TRUE);
}

MRESULT hmlec_wmMouseMove(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG flStyle = WinQueryWindowULong(hmle->hwndHMLE,QWL_STYLE);
POINTS p;
HMLEIpt ipt;
HMLEIpt curIpt;
HPOINTER hptrText;

    p.x = SHORT1FROMMP(mp1);
    p.y = SHORT2FROMMP(mp1);

    hptrText = WinQuerySysPointer(HWND_DESKTOP,SPTR_TEXT,FALSE);
    WinSetPointer(HWND_DESKTOP,hptrText);

    if (HMLEDocQueryMarkingState(hmle->doc)!=0)
        {
        RECTL rectl;
        POINTL p2;
        WinQueryWindowRect(hwnd,&rectl);

        p2.x = p.x;
        p2.y = p.y;

        if (WinPtInRect(WinQueryAnchorBlock(hwnd),&rectl,&p2))
            {
            LONG btn1State = WinGetKeyState(HWND_DESKTOP,VK_BUTTON1);
            if (btn1State & 0x8000)
                {
                HMLEQueryIpt(hmle,p.x,p.y,&ipt);
                HMLEDocQueryCurIpt(hmle->doc,&curIpt);
                if (HMLEIptCompare(&ipt,&curIpt)!=0)
                    {
                    HMLEDocSetCurIpt(hmle->doc,&ipt);
                    WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                    WinPostMsg(hwnd,HMLMC_REFRESHCURSOR,0,0);
                    }
                }
            }
        }
    if (flStyle & HMLS_AUTOFOCUS)
    if (WinQueryFocus(HWND_DESKTOP)!=hmle->hwndHMLE)
        WinSetFocus(HWND_DESKTOP,hmle->hwndHMLE);

    return MRFROMLONG(TRUE);
}

MRESULT hmlec_wmButton1MotionEnd(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    HMLERepositionPage(hmle);
    WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
    WinPostMsg(hwnd,HMLMC_REFRESHCURSOR,0,0);

    WinSetCapture(HWND_DESKTOP,NULLHANDLE);

    WinStopTimer(WinQueryAnchorBlock(hwnd),hwnd,HMLECID_TIMER);

    return MRFROMLONG(TRUE);
}

MRESULT hmlec_wmTimer(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    if ( (ULONG)mp1 == HMLECID_TIMER)
        {
        HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
        POINTL p;
        SWP swp;

        if (!WinQueryPointerPos(HWND_DESKTOP,&p)) return 0;
        if (!WinQueryWindowPos(hwnd,&swp)) return 0;

        WinMapWindowPoints(HWND_DESKTOP,hwnd,&p,1);

        if (!((p.x >= swp.x) && (p.x <= swp.x+swp.cx) &&
            (p.y >= swp.y) && (p.y <= swp.y+swp.cy)))
            {
            HMLEIpt ipt,curIpt;

            HMLEQueryIpt(hmle,p.x-swp.x,p.y-swp.y,&ipt);
            HMLEDocQueryCurIpt(hmle->doc,&curIpt);
            if (HMLEIptCompare(&ipt,&curIpt)!=0)
                {
                HMLEDocSetCurIpt(hmle->doc,&ipt);
                HMLERepositionPage(hmle);
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                WinPostMsg(hwnd,HMLMC_REFRESHCURSOR,0,0);
                }
            #ifdef DEBUG
//          printf(".");
            #endif
            }
        } else WinDefWindowProc(hwnd,WM_TIMER,mp1,mp2);
    return 0;
}

MRESULT hmlec_wmChar ( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
USHORT  fsFlags = SHORT1FROMMP(mp1);
UCHAR   ucVkey  = CHAR3FROMMP(mp2);

ULONG   flStyle;
ULONG   flHIAState;
HMLEIpt oldIpt,resultIpt;

BOOL    consumed = FALSE;

//  printf("Client:: WM_CHAR\n");
#ifdef DEBUG
    assert(hmle!=NULL);
#endif
    flHIAState = (ULONG) WinSendMsg(hmle->hwndHIA,HIAM_QUERYSTATE,0L,0L);
    flStyle = WinQueryWindowULong(hmle->hwndHMLE,QWL_STYLE);

    if (fsFlags & KC_KEYUP) return MRFROMLONG(FALSE);
    WinShowCursor(hwnd,FALSE);
    HMLERepositionPage(hmle);

    if ((flStyle & HMLS_IGNORETAB) &&
        (fsFlags & KC_VIRTUALKEY) &&
        (ucVkey == VK_TAB))
        return WinDefWindowProc(hwnd,WM_CHAR,mp1,mp2);

    if (!(hmle->readonly))
    {
        hmle->runningHIA = TRUE;
        consumed = (BOOL)WinSendMsg(hmle->hwndHIA,WM_CHAR,mp1,mp2);
        hmle->runningHIA = FALSE;
    }

    HMLEDocQueryCurIpt(hmle->doc,&oldIpt);

    if (fsFlags & KC_VIRTUALKEY)
    {
    switch (ucVkey) {
    case VK_HOME:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurStxTo(hmle->doc,0);
            HMLERepositionHoriz(hmle);
            consumed=TRUE;
            }
        if (FKC_HASCTRL(fsFlags) && FKC_EXALT(fsFlags))
            {
            HMLEDocMoveCurLineTo(hmle->doc,0);
            HMLEDocMoveCurStxTo(hmle->doc,0);
            HMLERepositionPage(hmle);
            consumed=TRUE;
            }
        break;
    case VK_END:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurStxTo(hmle->doc,HMLELineQueryLen(hmle->doc->curLine));
            HMLERepositionHoriz(hmle);
            consumed=TRUE;
            }
        if (FKC_HASCTRL(fsFlags) && FKC_EXALT(fsFlags))
            {
            HMLEDocMoveCurLineTo(hmle->doc,HMLEDocQueryNumberOfLines(hmle->doc)-1);
            HMLEDocMoveCurStxTo(hmle->doc,HMLELineQueryLen(hmle->doc->curLine));
            HMLERepositionPage(hmle);
            consumed=TRUE;
            }
        break;
    case VK_LEFT:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurStxLeft(hmle->doc,1);
            HMLERepositionPage(hmle);
            consumed=TRUE;
            }
        if (FKC_HASCTRL(fsFlags)&&(FKC_EXALT(fsFlags)))
            {
            HMLEDocMovePrevWord(hmle->doc);
            HMLERepositionPage(hmle);
            consumed=TRUE;
            }
        if( FKC_HASALT( fsFlags ))
        {
            HMLEColLeft( hmle );
            HMLEDocMoveCurStxLeft( hmle->doc, 1 );
            HMLERepositionPage( hmle );
            consumed = TRUE;
        }
        break;
    case VK_RIGHT:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurStxRight(hmle->doc,1);
            HMLERepositionPage(hmle);
            consumed=TRUE;
            }
        if (FKC_HASCTRL(fsFlags)&&(FKC_EXALT(fsFlags)))
            {
            HMLEDocMoveNextWord(hmle->doc);
            HMLERepositionPage(hmle);
            consumed=TRUE;
            }
        if( FKC_HASALT( fsFlags ))
        {
            HMLEColRight( hmle );
            HMLEDocMoveCurStxRight( hmle->doc, 1 );
            HMLERepositionPage( hmle );
            consumed = TRUE;
        }
        break;
    case VK_UP:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurLinePrev(hmle->doc);
            while (HMLELineQueryLineNumber(hmle->doc->curLine)<hmle->beginLineN)
                HMLELineUp(hmle);
            HMLERepositionHoriz(hmle);
            consumed=TRUE;
            }
        if (FKC_HASALT(fsFlags))
            {
            HMLELineUp(hmle);
            HMLEDocMoveCurLinePrev(hmle->doc);
            HMLERepositionHoriz(hmle);
            consumed=TRUE;
            }
        break;
    case VK_DOWN:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurLineNext(hmle->doc);
            while (HMLELineQueryLineNumber(hmle->doc->curLine)
                    >= hmle->beginLineN+hmle->ySize-1)
                HMLELineDown(hmle);
            HMLERepositionHoriz(hmle);
            consumed=TRUE;
//          printf("vk_down ok\n");
            }
        if (FKC_HASALT(fsFlags))
            {
            HMLELineDown(hmle);
            HMLEDocMoveCurLineNext(hmle->doc);
            HMLERepositionHoriz(hmle);
            consumed=TRUE;
            }
        break;
    case VK_PAGEUP:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurLine(hmle->doc,-(hmle->ySize-1));
            HMLEPageUp(hmle);
            consumed=TRUE;
            }
        break;
    case VK_PAGEDOWN:
    case VK_PAGEDOWN + 0x90:
        if (consumed) break;
        if (FKC_NONE(fsFlags) || FKC_SHIFTONLY(fsFlags))
            {
            HMLEDocMoveCurLine(hmle->doc,hmle->ySize-1);
            HMLEPageDown(hmle);
            consumed=TRUE;
            }
        break;
    case VK_BACKSPACE:
        if (consumed) break;
        if (FKC_NONE(fsFlags))
            {
            HMLEIpt curIpt,beginIpt;
            if (hmle->readonly) break;
            if (HMLEDocQueryMarkingState(hmle->doc) == 0)
                {
                beginIpt.ln=0;beginIpt.stx=0;
                HMLEDocQueryCurIpt(hmle->doc,&curIpt);
                if (HMLEIptCompare(&curIpt,&beginIpt)>0)
                    {
                    HMLEDocMoveCurStxLeft(hmle->doc,1);
                    HMLEDocDeleteHch(hmle->doc);
                    HMLERepositionPage(hmle);
                    WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                    }
                } else {
                HMLEDocDeleteSelection(hmle->doc);
                HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
                HMLERepositionPage(hmle);
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                }
            HMLESetChange(hmle);
            consumed=TRUE;
            }
        break;
    case VK_DELETE:
        if (consumed) break;
        if (FKC_NONE(fsFlags))
            {
            if (hmle->readonly) break;
            if (HMLEDocQueryMarkingState(hmle->doc) == 0)
                HMLEDocDeleteHch(hmle->doc);
                else
                {
                HMLEDocDeleteSelection(hmle->doc);
                HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
                }
            HMLERepositionPage(hmle);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);

            HMLESetChange(hmle);
            consumed=TRUE;
            }
        if (FKC_SHIFTONLY(fsFlags))
            {
            if (hmle->readonly) break;
            HMLEClipboardExport(hmle);
            HMLEDocDeleteSelection(hmle->doc);
            HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
            HMLERepositionPage(hmle);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);

            HMLESetChange(hmle);
            consumed=TRUE;
            }
        break;

    case VK_INSERT:
        if (consumed) break;
        if (FKC_CTRLONLY(fsFlags))
            {
            HMLEClipboardExport(hmle);
            consumed=TRUE;
            }
        if (FKC_SHIFTONLY(fsFlags))
            {
            if (hmle->readonly) break;
            HMLEClipboardImport(hmle);
            HMLESetChange(hmle);
            consumed=TRUE;
            }
        break;

    case VK_ENTER:
    case VK_NEWLINE:
        if (consumed) break;
        if (hmle->readonly) break;
        if (FKC_NONE(fsFlags))
            {
            if (HMLEDocQueryMarkingState(hmle->doc) != 0)
                {
                HMLEDocDeleteSelection(hmle->doc);
                HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
                }
            HMLEDocSplitCurLine(hmle->doc);
            if( hmle->autoIndent )
            {
                int stx;
                HMLELine *line = hmle->doc->curLine->prevLine;

                for( stx = 0;
                     ( line->str[ stx ] == ' ' ) || ( line->str[ stx ] == '\t' );
                     stx ++ )
                {
                    HMLEDocInsertHch( hmle->doc, ' ');

                    if( line->str[ stx ] == '\t' )
                    {
                        int i;

                        for( i = 1;
                             i < HMLEDocQueryTabspaceStx( hmle->doc, line, stx );
                             i ++ )
                            HMLEDocInsertHch( hmle->doc, ' ');
                    }
                }
            }

            HMLERepositionPage(hmle);
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);

            HMLESetChange(hmle);
            consumed=TRUE;
            }
        break;

    } // switch

//  printf("first switch end\n");

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
            if (HMLEDocQueryMarkingState(hmle->doc)==0)
                {
                HMLEDocBeginSelectionHere(hmle->doc,&oldIpt);
                HMLEWNDC_Notify(hwnd,HMLN_BEGINSELECTION,0);
                } else {
                HMLEIpt anchorIpt,curIpt;
                HMLEDocQueryCurIpt(hmle->doc,&curIpt);
                HMLEDocQueryAnchorIpt(hmle->doc,&anchorIpt);
                if ((curIpt.ln==anchorIpt.ln)&&(curIpt.stx==anchorIpt.stx))
                    {
                    HMLEDocEndSelection(hmle->doc);
                    HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
                    }
                }
            WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
            } else {
            if (HMLEDocQueryMarkingState(hmle->doc)!=0)
                {
                HMLEDocEndSelection(hmle->doc);
                HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                }
            }
    } // switch
    } // if Virtualkey

//  printf("second switch end\n");

    if (!consumed)
        {
        HWND hwndOwner = WinQueryWindow(hmle->hwndHMLE,QW_OWNER);
        WinPostMsg(hwndOwner,WM_CHAR,mp1,mp2);
        } else {
//      HMLEDocCheckPacking(doc);
//      printf("Client:: WM_CHAR end\n");

        HMLEDocQueryCurIpt(hmle->doc,&resultIpt);
        HMLEStatNotify(hmle,HMLN_NOTIFYCURSORPOS,
                       MPFROM2SHORT(resultIpt.ln, HMLEDocColFromStx(hmle->doc,resultIpt.stx)));
        }

    WinSendMsg(hwnd,HMLMC_REFRESHCURSOR,0L,0L);

    return MRFROMLONG(consumed); // msg processed
}

MRESULT hmlec_wmControl(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
//ULONG flStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
ULONG   flHIAState;

    flHIAState = (ULONG) WinSendMsg(hmle->hwndHIA,HIAM_QUERYSTATE,0L,0L);

    switch (SHORT1FROMMP(mp1)) { // control

    case HMLECID_HIA:
        switch (SHORT2FROMMP(mp1)) { // Notification

        case HIAN_INSERTHCH:
            if (!ISHCH(SHORT1FROMMP(mp2)))
            {
                if (HMLEDocQueryMarkingState(hmle->doc)!=0)
                    {
                    HMLEDocDeleteSelection(hmle->doc);
                    HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
                    WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                    }

                if (!(flHIAState & HIAST_INSERTMODE))
                    if (!(hmle->doc->curStx == HMLELineQueryLen(hmle->doc->curLine)))
                        HMLEDocDeleteHch(hmle->doc);

                HMLEDocInsertHch(hmle->doc,SHORT1FROMMP(mp2));

                HMLEUpdateCurLine(hmle);
                HMLERepositionHoriz(hmle);

                HMLESetChange(hmle);
            }
            break;
        case HIAN_COMPO_BEGIN:
            hmle->doc->hchComposing = 0;
            if (HMLEDocQueryMarkingState(hmle->doc)!=0)
                {
                HMLEDocDeleteSelection(hmle->doc);
                HMLEWNDC_Notify(hwnd,HMLN_ENDSELECTION,0);
                WinInvalidateRect(hwnd,NULLHANDLE,FALSE);
                }
            if (!(flHIAState & HIAST_INSERTMODE))
                if (!(hmle->doc->curStx == HMLELineQueryLen(hmle->doc->curLine)))
                    HMLEDocDeleteHch(hmle->doc);
            hmle->doc->hchComposing = SHORT2FROMMP(mp2);
            HMLEUpdateCurLine(hmle);
            HMLERepositionHoriz(hmle);
            HMLESetChange(hmle);
            break;
        case HIAN_COMPO_STEP:
            hmle->doc->hchComposing = SHORT2FROMMP(mp2);
            HMLEUpdateCurLine(hmle);
            break;
        case HIAN_COMPO_STEPBACK:
            hmle->doc->hchComposing = SHORT2FROMMP(mp2);
            HMLEUpdateCurLine(hmle);
            break;
        case HIAN_COMPO_CANCEL:
            hmle->doc->hchComposing = 0;
            HMLEUpdateCurLine(hmle);
            break;
        case HIAN_COMPO_COMPLETE:
            hmle->doc->hchComposing = 0;
            if ( SHORT1FROMMP( mp2 )) HMLESetChange(hmle);
            HMLEDocInsertHch(hmle->doc, SHORT1FROMMP( mp2 ));
            HMLEUpdateCurLine(hmle);
            HMLERepositionHoriz(hmle);
            //doc->hchComposing = SHORT2FROMMP(mp2);
            break;
        case HIAN_HANMODECHANGED:
            HMLEStatNotify(hmle,HMLN_CHANGEDHANINPUTMODE,mp2);
            break;
        case HIAN_KBDTYPECHANGED:
            HMLEStatNotify(hmle,HMLN_CHANGEDKBDTYPE,mp2);
            break;
        case HIAN_INSERTMODECHANGED:
            HMLEStatNotify(hmle,HMLN_CHANGEDINSERTMODE,mp2);
            break;
        case HIAN_CONNECT:
            hmle->connectedToHIA = (int)mp2;
            #ifdef DEBUG
            printf("HIAN_CONNECT:");
            if (hmle->connectedToHIA) printf("TRUE\n"); else printf("FALSE\n");
            #endif
            break;

        case HIAN_HGHJCONVERT:
        {
            HMLELine *line = hmle->doc->curLine;
            HANCHAR  hch;

/*
            if( SHORT1FROMMP( mp2 ))
                HMLEDocMoveCurStxLeft( hmle->doc, 1 );
*/

            hch = line->str[ hmle->doc->curStx ];

            if( hch < 0x80 )
            {
/*
                if( SHORT1FROMMP( mp2 ))
                    HMLEDocMoveCurStxRight( hmle->doc, 1 );
*/

                break;
            }

            if(( hch >= 0xd9 ) && ( hch <= 0xde ))
                break;

            hch = HCHFROM2CH( hch, line->str[ hmle->doc->curStx + 1 ]);

            if( hch >= 0xe000 )
                hch = hch_hj2hg( hch );
            else
                hch = hjselDlg( HWND_DESKTOP, hwnd, NULLHANDLE, hch );

            if( hch != HCH_SINGLE_SPACE )
            {
                HMLEDocDeleteHch( hmle->doc );
                HMLEDocInsertHch( hmle->doc, hch );
                HMLEUpdateCurLine( hmle );
                HMLERepositionHoriz( hmle );
                HMLESetChange( hmle );
            }
            break;
        }
        } // note switch end
        break;
    } // control switch end
    return 0L;
}

void HMLEWNDC_VScroll(HWND hwnd,int step)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
RECTL rectlEntire;

    WinQueryWindowRect(hwnd,&rectlEntire);
    if (step==0) return;

    if (step > 0)
        {
        if (step > hmle->ySize) step = hmle->ySize;
        WinScrollWindow(hwnd,0,(step<<4),
            NULL,&rectlEntire,NULLHANDLE,NULL,SW_INVALIDATERGN);
        }
    if (step < 0)
        {
        if (-step > hmle->ySize) step = - (hmle->ySize);
        WinScrollWindow(hwnd,0,-((-step)<<4),
            NULL,&rectlEntire,NULLHANDLE,NULL,SW_INVALIDATERGN);
        }
}

void HMLEWNDC_HScroll(HWND hwnd,int step)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
RECTL rectlEntire;

    WinQueryWindowRect(hwnd,&rectlEntire);
    if (step==0) return;

    if (step > 0)
        {
        if (step > hmle->xSize) step = hmle->xSize;
        WinScrollWindow(hwnd,(step<<3),0,
            NULL,&rectlEntire,NULLHANDLE,NULL,SW_INVALIDATERGN);
        }
    if (step < 0)
        {
        if (-step > hmle->xSize) step = - (hmle->xSize);
        WinScrollWindow(hwnd,-((-step)<<3),0,
            NULL,&rectlEntire,NULLHANDLE,NULL,SW_INVALIDATERGN);
        }
}

void HMLEWNDC_Scroll(HWND hwnd,int xstep,int ystep)
{
RECTL rectlEntire;

    WinQueryWindowRect(hwnd,&rectlEntire);
    if ((xstep==0) && (ystep==0)) return;

    WinScrollWindow(hwnd,(xstep*8),(ystep*16),
        NULL,&rectlEntire,NULLHANDLE,NULL,SW_INVALIDATERGN);
}

void HMLEQueryIpt(HMLE *hmle,int x,int y,HMLEIpt *ipt)
{
int relColN;
int relLineN;
HMLELine *line;
RECTL rectl;

    WinQueryWindowRect(hmle->hwndClient,&rectl);
    relColN = x/8;
    relLineN = (rectl.yTop-y)/16;
    if (x%8 > 3) relColN++;
//  printf("(%d,%d)\n",relColN,relLineN);

    if (relColN<0) relColN=-1;
    if (relLineN<0) relLineN=-1;
    if (relColN>=hmle->xSize) relColN = hmle->xSize;
    if (relLineN>=hmle->ySize) relLineN = hmle->ySize;

//  printf("<%d,%d>\n",relColN,relLineN);

    ipt->ln = relLineN + hmle->beginLineN;
    if (ipt->ln < 0)
        ipt->ln = 0;
    if (relColN+hmle->beginColN < 0)
        relColN = 0;

    line = HMLEDocQueryLine(hmle->doc,ipt->ln);
    ipt->stx = HMLEDocCol2Stx(hmle->doc,line,relColN+hmle->beginColN);
    hch_alignStx(HMLELineQueryStr(line,0),&(ipt->stx));
}

void HMLERepositionVert(HMLE *hmle)
{
int ln;

    ln = HMLELineQueryLineNumber(hmle->doc->curLine);

    if (ln<hmle->beginLineN)
        HMLESetPageLine(hmle,ln);
    if (ln>hmle->beginLineN+hmle->ySize-2)
        HMLESetPageLine(hmle,ln-hmle->ySize+2);
}

void HMLERepositionHoriz(HMLE *hmle)
{
int col,beginColN,oldColN;

    col = HMLEDocStx2Col(hmle->doc,hmle->doc->curLine,hmle->doc->curStx);
    oldColN = beginColN = hmle->beginColN;
    while (col < beginColN)
        beginColN -= 5;
    while (col > beginColN+hmle->xSize-2)
        beginColN += 5;
    if (beginColN<0) beginColN=0;

    hmle->beginColN = beginColN;
    HMLEWNDC_HScroll(hmle->hwndClient,oldColN-beginColN);
    WinPostMsg(hmle->hwndHMLE,HMLM_REFRESHSCROLLBAR,0,0);
}

void HMLERepositionPage(HMLE *hmle)
{
int col,beginColN,oldColN;
int ln,beginLineN,endLineN,oldLineN;

    beginLineN = hmle->beginLineN;
    ln = HMLELineQueryLineNumber(hmle->doc->curLine);
    if (ln<hmle->beginLineN) beginLineN = ln;
    if (ln>hmle->beginLineN+hmle->ySize-2) beginLineN = ln-hmle->ySize+2;
    endLineN = HMLEDocQueryNumberOfLines(hmle->doc)-1 - hmle->ySize +2;
    if (endLineN < 0) endLineN = 0;
    if (beginLineN<0) beginLineN = 0;
    if (beginLineN>endLineN) beginLineN = endLineN;
    oldLineN = hmle->beginLineN;
    hmle->beginLineN = beginLineN;

    col = HMLEDocStx2Col(hmle->doc,hmle->doc->curLine,hmle->doc->curStx);
    oldColN = beginColN = hmle->beginColN;
    while (col < beginColN)
        beginColN -= 5;
    while (col > beginColN+hmle->xSize-2)
        beginColN += 5;
    if (beginColN<0) beginColN=0;
    hmle->beginColN = beginColN;

    HMLEWNDC_Scroll(hmle->hwndClient,oldColN-beginColN,beginLineN-oldLineN);
    WinPostMsg(hmle->hwndHMLE,HMLM_REFRESHSCROLLBAR,0,0);
}

void HMLEUpdateCurLine(HMLE *hmle)
{
RECTL rectlUpdate;

    WinQueryWindowRect(hmle->hwndClient,&rectlUpdate);

    rectlUpdate.yTop -= (HMLEQueryRelY(hmle)<<4);

    if( hmle->doc->wordWrap )
    {
        if(( hmle->doc->curLine->prevLine != NULL ) &&
           ( hmle->doc->curLine->prevLine->wordWrapped ))
        rectlUpdate.yTop += 16;
    }
    else
        rectlUpdate.yBottom = rectlUpdate.yTop - 16;

    WinInvalidateRect(hmle->hwndClient,&rectlUpdate,FALSE);
}

void HMLEClipboardExport(HMLE *hmle)
{
HAB hab;

HMLETextThunk *textThunk;
ULONG bufsize;
BYTE  *buf;

ULONG fSuccess;

    hab = WinQueryAnchorBlock(hmle->hwndClient);

    if (HMLEDocQueryMarkingState(hmle->doc)==0) return;

    textThunk = HMLEDocGetSelection(hmle->doc);
    bufsize = HMLETextThunkQueryCRLFStringLen(textThunk)+1;

    if (WinOpenClipbrd(hab))
        {
        if (!(fSuccess = DosAllocSharedMem((PVOID)&buf,NULL,
                bufsize,PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE)))
            {
            HMLETextThunkGetCRLFString(textThunk,buf);

//            if (hmle->han_type == HMLE_HAN_KS)
                hch_sy2ksstr(buf);

            WinEmptyClipbrd(hab);

            fSuccess = WinSetClipbrdData(hab,(ULONG)buf,
                    CF_TEXT,CFI_POINTER);

            DosFreeMem(buf);
            WinCloseClipbrd(hab);
            }
        }
    HMLEDestroyTextThunk(textThunk);
}

void HMLEClipboardImport(HMLE *hmle)
{
HAB hab;
HMLETextThunk *textThunk;
BYTE  *buf;
BYTE  *clipbuf;

    hab = WinQueryAnchorBlock(hmle->hwndClient);

    if (HMLEDocQueryMarkingState(hmle->doc)!=0)
        {
        HMLEDocDeleteSelection(hmle->doc);
        HMLEWNDC_Notify(hmle->hwndClient,HMLN_ENDSELECTION,0);
        }

    if (WinOpenClipbrd(hab))
        {
        clipbuf = (BYTE*) WinQueryClipbrdData(hab,CF_TEXT);
        if (clipbuf!=NULL)
            {
            textThunk = HMLECreateTextThunk(clipbuf);
            if (textThunk)
                {
                buf = HMLETextThunkQueryStr(textThunk);
//                if (hmle->han_type == HMLE_HAN_KS)
                hch_ks2systr(buf);
                HMLEDocInsertTextThunk(hmle->doc,textThunk);
                    if (hmle->doc->errno==HMLEDOC_ERROR_INSERTIONTRUNCATED)
                        HMLEWNDC_Notify(hmle->hwndClient,HMLN_OVERFLOW,0);
                HMLESetChange(hmle);
                HMLEDestroyTextThunk(textThunk);
                }
            }
        WinCloseClipbrd(hab);
        }
    HMLERepositionPage(hmle);
    WinInvalidateRect(hmle->hwndClient,NULL,FALSE);
    WinPostMsg(hmle->hwndClient,HMLMC_REFRESHCURSOR,0,0);
//    printf("// HMLEClipboardImport()\n");
}

void HMLEWNDC_Notify(HWND hwnd,USHORT notifCode,MPARAM mp2)
{
HWND    hwndOwner = WinQueryWindow(hwnd,QW_OWNER);
USHORT  usIdOwner = WinQueryWindowUShort(hwndOwner,QWS_ID);
HWND    hwndOwnerOwner = WinQueryWindow(hwndOwner,QW_OWNER);

    WinPostMsg(hwndOwnerOwner,WM_CONTROL,
            MPFROM2SHORT(usIdOwner,notifCode),
            mp2);
}

void HMLEStatNotify(HMLE *hmle,USHORT notifCode,MPARAM mp2)
{
ULONG flStyle = WinQueryWindowULong(hmle->hwndHMLE,QWL_STYLE);

    if (flStyle & HMLS_STATUSNOTIFY)
        HMLEWNDC_Notify(hmle->hwndClient,notifCode,mp2);
}

void HMLESetChange(HMLE *hmle)
{
    hmle->doc->changed = TRUE;
    HMLEWNDC_Notify(hmle->hwndClient,HMLN_CHANGED,0);
}

void HMLESetPageLine(HMLE *hmle,int beginLineN)
{
LONG oldLineN;
LONG endLineN;

#ifdef DEBUG
    assert(hmle!=NULL);
#endif

    endLineN = HMLEDocQueryNumberOfLines(hmle->doc)-1 - hmle->ySize +2;
    if (endLineN < 0) endLineN = 0;
    if (beginLineN<0) beginLineN = 0;
    if (beginLineN>endLineN) beginLineN = endLineN;

    oldLineN = hmle->beginLineN;
    hmle->beginLineN = beginLineN;
    HMLEWNDC_VScroll(hmle->hwndClient,beginLineN-oldLineN);
    WinPostMsg(hmle->hwndHMLE,HMLM_REFRESHSCROLLBAR,0,0);
}

void HMLELineUp(HMLE *hmle)
{
    HMLESetPageLine(hmle,hmle->beginLineN-1);
}

void HMLELineDown(HMLE *hmle)
{
    HMLESetPageLine(hmle,hmle->beginLineN+1);
}

void HMLEPageUp(HMLE *hmle)
{
    HMLESetPageLine(hmle,hmle->beginLineN - hmle->ySize+1);
}

void HMLEPageDown(HMLE *hmle)
{
    HMLESetPageLine(hmle,hmle->beginLineN + hmle->ySize-1);
}

void HMLESetPageCol(HMLE *hmle,int beginColN)
{
LONG oldColN;
LONG endColN;

    endColN = HMLEDocQueryMaxCols(hmle->doc) - 1 - hmle->xSize + 2;
    if (endColN < 0) endColN = 0;
    if (beginColN<0) beginColN = 0;
    if (beginColN>endColN) beginColN = endColN;

    oldColN = hmle->beginColN;
    hmle->beginColN = beginColN;
    HMLEWNDC_HScroll(hmle->hwndClient,oldColN-beginColN);
    WinPostMsg(hmle->hwndHMLE,HMLM_REFRESHSCROLLBAR,0,0);
}

void HMLEColLeft(HMLE *hmle)
{
    HMLESetPageCol(hmle,hmle->beginColN-1);
}

void HMLEColRight(HMLE *hmle)
{
    HMLESetPageCol(hmle,hmle->beginColN+1);
}

void HMLEPageLeft(HMLE *hmle)
{
    HMLESetPageCol(hmle,hmle->beginColN - hmle->xSize / 2);
}

void HMLEPageRight(HMLE *hmle)
{
    HMLESetPageCol(hmle,hmle->beginColN + hmle->xSize / 2);
}

int HMLEQueryXsize(HMLE *hmle)
{
RECTL rectl;
int xSize;

    WinQueryWindowRect(hmle->hwndClient,&rectl);
    xSize = (rectl.xRight-rectl.xLeft+1);
    return (xSize+7)/8;
}

int HMLEQueryYsize(HMLE *hmle)
{
RECTL rectl;
int ySize;

    WinQueryWindowRect(hmle->hwndClient,&rectl);
    ySize = (rectl.yTop-rectl.yBottom+1);
    return (ySize+15)/16;
}

int HMLEQueryRelX(HMLE *hmle)
{
int curCol = HMLEDocStx2Col(hmle->doc,hmle->doc->curLine,hmle->doc->curStx);
    return (curCol - hmle->beginColN);
}

int HMLEQueryRelY(HMLE *hmle)
{
int curLineN = HMLELineQueryLineNumber(hmle->doc->curLine);
    return (curLineN - hmle->beginLineN);
}

static MRESULT hmlec_dmDragOver(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG flStyle;
PDRAGINFO pDraginfo = (PDRAGINFO)mp1;
PDRAGITEM pItem = NULL;
USHORT usDrop=DOR_NEVERDROP,usDefaultOp=DO_MOVE;

#ifdef DEBUG
    assert(hmle!=NULL);
#endif
    flStyle = WinQueryWindowULong(hmle->hwndHMLE,QWL_STYLE);
    if (!(flStyle & HMLS_ACCEPTFILEDROP)) return MRFROM2SHORT(DOR_NEVERDROP,0);

do  {
    if (!DrgAccessDraginfo(pDraginfo))
        {
        pDraginfo = NULL;
        break;
        }
    usDrop = DOR_DROP;
    switch (pDraginfo->usOperation)
        {
        case DO_COPY:
            {
            usDefaultOp = DO_COPY;
            } break;
        case DO_MOVE:
        case DO_DEFAULT:
            {
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

static MRESULT hmlec_dmDrop(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PDRAGINFO pDraginfo = (PDRAGINFO)mp1;
PDRAGITEM pItem = NULL;

    if (DrgAccessDraginfo(pDraginfo))
        {
        HMLE *hmle = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
        char containerName[CCHMAXPATH];
        char sourceName[CCHMAXPATH];

        if ((pItem = DrgQueryDragitemPtr(pDraginfo,0))==NULL) return 0;
        DrgQueryStrName(pItem->hstrContainerName,CCHMAXPATH,containerName);
        DrgQueryStrName(pItem->hstrSourceName,CCHMAXPATH,sourceName);
        strcpy(hmle->dropFileNotif->fullPath,containerName);
        strcat(hmle->dropFileNotif->fullPath,sourceName);
        #ifdef DEBUG
        printf("%s\n",hmle->dropFileNotif->fullPath);
        #endif
        hmle->dropFileNotif->usOperation = pDraginfo->usOperation;

        HMLEWNDC_Notify(hwnd,HMLN_FILEDROP,hmle->dropFileNotif);

        DrgFreeDraginfo(pDraginfo);
        }
    return 0;
}
