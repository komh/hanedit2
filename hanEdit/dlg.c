#define INCL_PM
#include <os2.h>
#include "HMLEdit.h"

extern HWND hwndHMLE;
extern HWND hwndHIA;

static void dlgFind_wmInitDlg(HWND hwnd);
static void dlgFind_Find(HWND hwnd);
static void dlgFind_ReplaceAll(HWND hwnd);
static void dlgFind_Replace(HWND hwnd,int findNext);
static void dlgFind_SetFound(HWND hwnd,int found);
static int dlgFind_SetButtonFocus(HWND hwnd);

static int dlgFind_found = FALSE;

MRESULT EXPENTRY HEFindDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch (msg)
    {
    case WM_INITDLG:
        dlgFind_wmInitDlg(hwnd);
        break;
/*
    case WM_CHAR:
        if (dlgFind_SetButtonFocus(hwnd))
            return 0;
        break;
*/
    case WM_CONTROL:
        {
        switch (SHORT1FROMMP(mp1))
        {
        case IDHEF_FIND:
            switch( SHORT2FROMMP( mp1 ))
            {
                case HEN_CHANGE :
                    dlgFind_SetButtonFocus( hwnd );
                    break;

                case HEN_SETFOCUS :
                    WinSendMsg( WinWindowFromID( hwnd, IDHEF_FIND ),
                                HEM_SETSEL, MPFROM2SHORT( 0, -1 ), 0 );
                    WinSendMsg( WinWindowFromID( hwnd, IDHEF_FIND ), HEM_REFRESH, 0, 0 );

                    dlgFind_SetButtonFocus(hwnd);
                    break;
            }
            break;
        case IDHEF_REPLACE:
            switch( SHORT2FROMMP( mp1 ))
            {
                case HEN_CHANGE :
                    dlgFind_SetButtonFocus( hwnd );
                    break;

                case HEN_SETFOCUS :
                    WinSendMsg( WinWindowFromID( hwnd, IDHEF_REPLACE ),
                                HEM_SETSEL, MPFROM2SHORT( 0, -1 ), 0 );
                    WinSendMsg( WinWindowFromID( hwnd, IDHEF_REPLACE ), HEM_REFRESH, 0, 0 );

                    dlgFind_SetButtonFocus(hwnd);
                    break;
            }
            break;
        } // control switch
        } break;
    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case IDB_FIND:
            dlgFind_Find(hwnd);
            return 0;
            break;
        case IDB_REPLACE:
            dlgFind_Replace(hwnd,FALSE);
            return 0;
            break;
        case IDB_REPLACEFIND:
            dlgFind_Replace(hwnd,TRUE);
            return 0;
            break;
        case IDB_REPLACEALL:
            dlgFind_ReplaceAll(hwnd);
            return 0;
            break;
        case DID_OK:
            {
            HWND hwndHIA = (HWND)WinSendMsg(hwndHMLE,HMLM_QUERYHIA,0,0);
            WinSendMsg(hwndHIA,HIAM_CONNECT,0,0);
            #ifdef DEBUG
/*              HWND hwndFind = WinWindowFromID(hwnd,IDHEF_FIND);
                fprintf(stderr,"\n<HEF>\n");
                WinSendMsg(hwndFind,HEM_DUMP,(MPARAM)stderr,0);
                fprintf(stderr,"\n<HMLE>\n");
                WinSendMsg(hwndHMLE,HMLM_DUMP,(MPARAM)stderr,0);
*/          #endif
            }
            WinDismissDlg(hwnd,DID_OK);
            return 0;
            break;
        default:
//          printf("here\n");
//          return WinDefDlgProc(hwnd,msg,mp1,mp2);
        }
    } /* endswitch */
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

int dlgFind_SetButtonFocus(HWND hwnd)
{
HWND hwndBtnFind = WinWindowFromID(hwnd,IDB_FIND);
HWND hwndBtnReplace = WinWindowFromID(hwnd,IDB_REPLACE);
HWND hwndBtnReplaceFind = WinWindowFromID(hwnd,IDB_REPLACEFIND);
HWND hwndBtnReplaceAll = WinWindowFromID(hwnd,IDB_REPLACEALL);
HWND hwndBtnClose = WinWindowFromID(hwnd,DID_OK);
HWND hwndFocus;

HWND hwndFind = WinWindowFromID(hwnd,IDHEF_FIND);
int findLen=0;
char *findStr = NULL;

    findLen = WinQueryWindowTextLength(hwndFind);
//  findLen = (int)WinSendMsg(hwndFind,HEM_QUERYTEXTLENGTH,0,0);
    findStr = malloc(findLen+1);
#ifdef DEBUG
    assert(findStr!=NULL);
#endif
    if (findStr==NULL) return 0;
    WinQueryWindowText(hwndFind,findLen+1,findStr);
//  WinSendMsg(hwndFind,HEM_QUERYTEXT,findStr,0);

    hwndFocus = WinQueryFocus(HWND_DESKTOP);
    if ((hwndFocus!=hwndBtnFind)&&
        (hwndFocus!=hwndBtnReplace)&&
        (hwndFocus!=hwndBtnReplaceFind)&&
        (hwndFocus!=hwndBtnReplaceAll)&&
        (hwndFocus!=hwndBtnClose))
        {
        if (strlen(findStr)>0)
            {
            WinEnableWindow(hwndBtnFind,TRUE);
            WinSendMsg(hwndBtnFind,BM_SETDEFAULT,MPFROMLONG(TRUE),0);
            WinSendMsg(hwndBtnClose,BM_SETDEFAULT,MPFROMLONG(FALSE),0);
            } else {
            WinEnableWindow(hwndBtnFind,FALSE);
            dlgFind_SetFound(hwnd,FALSE);
            WinSendMsg(hwndBtnFind,BM_SETDEFAULT,MPFROMLONG(FALSE),0);
            WinSendMsg(hwndBtnClose,BM_SETDEFAULT,MPFROMLONG(TRUE),0);
            }
        return TRUE;
        }
    return FALSE;
}

void dlgFind_wmInitDlg(HWND hwnd)
{
HWND hwndBtnFind = WinWindowFromID(hwnd,IDB_FIND);
HWND hwndFind = WinWindowFromID(hwnd,IDHEF_FIND);
HWND hwndReplace = WinWindowFromID(hwnd,IDHEF_REPLACE);
ULONG han_type = (ULONG)WinSendMsg(hwndHMLE,HMLM_QUERYHANTYPE,0,0);

#ifdef DEBUG
//  WinSendMsg(hwndFind,HEM_DUMP,(MPARAM)stderr,0);
//  WinSendMsg(hwndHMLE,HMLM_DUMP,(MPARAM)stderr,0);
#endif

    WinSendMsg(hwndFind,HEM_SETHIA,MPFROMLONG(hwndHIA),0);
    WinSendMsg(hwndReplace,HEM_SETHIA,MPFROMLONG(hwndHIA),0);

    WinSendMsg(hwndFind,HEM_SETHANTYPE,MPFROMLONG(han_type),0);
    WinSendMsg(hwndReplace,HEM_SETHANTYPE,MPFROMLONG(han_type),0);

    WinEnableWindow(hwndBtnFind,FALSE);
    dlgFind_SetFound(hwnd,FALSE);
}

void dlgFind_SetFound(HWND hwnd,int found)
{
HWND hwndBtnReplace = WinWindowFromID(hwnd,IDB_REPLACE);
HWND hwndBtnReplaceFind = WinWindowFromID(hwnd,IDB_REPLACEFIND);
HWND hwndBtnReplaceAll = WinWindowFromID(hwnd,IDB_REPLACEALL);

    WinEnableWindow(hwndBtnReplace,found);
    WinEnableWindow(hwndBtnReplaceFind,found);
    WinEnableWindow(hwndBtnReplaceAll,found);
    dlgFind_found = found;
}

void dlgFind_Find(HWND hwnd)
{
HWND hwndFind = WinWindowFromID(hwnd,IDHEF_FIND);
HWND hwndCaseSens = WinWindowFromID(hwnd,IDB_CASESENSITIVE);

MLE_SEARCHDATA searchData;
ULONG ulStyle = 0;
int findLen=0;
char *findStr = NULL;

    findLen = WinQueryWindowTextLength(hwndFind);
    findStr = malloc(findLen+1);
#ifdef DEBUG
    assert(findStr!=NULL);
#endif
    if (findStr==NULL) return;
    WinQueryWindowText(hwndFind,findLen+1,findStr);

    searchData.cb = sizeof(searchData);
    searchData.pchFind = findStr;
    searchData.pchReplace = NULL;
    searchData.iptStart = -1;
    searchData.iptStop = -1;
    ulStyle = MLFSEARCH_SELECTMATCH;
    if (WinSendMsg(hwndCaseSens,BM_QUERYCHECK,0,0))
        ulStyle |= MLFSEARCH_CASESENSITIVE;
    dlgFind_SetFound(hwnd,(int) WinSendMsg(hwndHMLE,HMLM_SEARCH,MPFROMLONG(ulStyle),&searchData));
    free(findStr);
}

void dlgFind_ReplaceAll(HWND hwnd)
{
HWND hwndFind = WinWindowFromID(hwnd,IDHEF_FIND);
HWND hwndCaseSens = WinWindowFromID(hwnd,IDB_CASESENSITIVE);
HWND hwndReplace = WinWindowFromID(hwnd,IDHEF_REPLACE);

MLE_SEARCHDATA searchData;
ULONG ulStyle = 0;
int findLen=0,replaceLen=0;
char *findStr = NULL,*replaceStr = NULL;
int replaceCount = 0;

    findLen = WinQueryWindowTextLength(hwndFind);
    findStr = malloc(findLen+1);
#ifdef DEBUG
    assert(findStr!=NULL);
#endif
    if (findStr==NULL) return;
    WinQueryWindowText(hwndFind,findLen+1,findStr);

    replaceLen = WinQueryWindowTextLength(hwndReplace);
    replaceStr = malloc(replaceLen+1);
#ifdef DEBUG
    assert(replaceStr!=NULL);
#endif
    if (replaceStr==NULL) return;
    WinQueryWindowText(hwndReplace,replaceLen+1,replaceStr);

    searchData.cb = sizeof(searchData);
    searchData.pchFind = findStr;
    searchData.pchReplace = replaceStr;
    searchData.iptStart = -1;
    searchData.iptStop = -1;
    ulStyle = MLFSEARCH_SELECTMATCH | MLFSEARCH_CHANGEALL;
    if (WinSendMsg(hwndCaseSens,BM_QUERYCHECK,0,0))
        ulStyle |= MLFSEARCH_CASESENSITIVE;
    replaceCount = (int) WinSendMsg(hwndHMLE,HMLM_SEARCH,MPFROMLONG(ulStyle),&searchData);
    dlgFind_SetFound(hwnd,FALSE);
    free(findStr);
}

void dlgFind_Replace(HWND hwnd,int findNext)
{
HWND hwndReplace = WinWindowFromID(hwnd,IDHEF_REPLACE);

int replaceLen=0;
char *replaceStr = NULL;
USHORT minIpt,maxIpt;
ULONG ret;

    if (!dlgFind_found)
        {
        dlgFind_Find(hwnd);
        return;
        }
    ret = (ULONG)WinSendMsg(hwndHMLE,HMLM_QUERYSEL,MPFROMLONG(HMLFQS_MINMAXSEL),0);
    minIpt = ret >> 16;
    maxIpt = ret & 0xFFFF;

    if (minIpt==maxIpt) return;

    replaceLen = WinQueryWindowTextLength(hwndReplace);
    replaceStr = malloc(replaceLen+1);
#ifdef DEBUG
    assert(replaceStr!=NULL);
#endif
    if (replaceStr==NULL) return;
    WinQueryWindowText(hwndReplace,replaceLen+1,replaceStr);
    WinSendMsg(hwndHMLE,HMLM_INSERT,replaceStr,0);

    free(replaceStr);
    if (findNext)
        dlgFind_Find(hwnd);
        else
        dlgFind_SetFound(hwnd,FALSE);
    return;
}

