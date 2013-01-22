#ifndef __HMLE_internal_h__
#define __HMLE_internal_h__

#define INCL_PM
#define INCL_DOSMEMMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#include <assert.h>
#include "../etclib/debugutil.h"
#endif

#include "../hanlib/han.h"
#include "../etclib/textutil.h"
#include "HMLE.h"
#include "HMLEIpt.h"
#include "HMLELine.h"
#include "HMLETextThunk.h"
#include "HMLEDoc.h"

#define WC_HMLE_Client  ("HanMLE:Client")

#define HMLMC_REFRESHCURSOR     (WM_USER+1)
#define HMLMC_REFRESH           (WM_USER+2)

#define HMLECID_HIA             (TID_USERMAX-1)
#define HMLECID_TIMER           (TID_USERMAX-2)

#define HMLE_DEFAULT_LINESIZE       4096
#define HMLE_BORDERSIZE             1

#define WINWORD_INSTANCE            0

#define FKC_NONE(fl)        (!(fl & (KC_ALT|KC_CTRL|KC_SHIFT)))
#define FKC_EXALT(fl)       (!(fl & KC_ALT) && (fl & (KC_SHIFT|KC_CTRL)))
#define FKC_EXCTRL(fl)      (!(fl & KC_CTRL) && (fl & (KC_ALT|KC_SHIFT)))
#define FKC_EXSHIFT(fl)     (!(fl & KC_SHIFT) && (fl & (KC_ALT|KC_CTRL)))
#define FKC_HASALT(fl)      (fl & KC_ALT)
#define FKC_HASCTRL(fl)     (fl & KC_CTRL)
#define FKC_HASSHIFT(fl)    (fl & KC_SHIFT)
#define FKC_ALTONLY(fl)     ((fl & KC_ALT) && !(fl & (KC_SHIFT|KC_CTRL)))
#define FKC_CTRLONLY(fl)    ((fl & KC_CTRL) && !(fl & (KC_ALT|KC_SHIFT)))
#define FKC_SHIFTONLY(fl)   ((fl & KC_SHIFT) && !(fl & (KC_ALT|KC_CTRL)))

typedef struct tagHMLE {
    HMLECTLDATA *pCtlData;
    HWND hwndHMLE;
    HWND hwndClient;
    HWND hwndHIA;
    HMLEDoc *doc;
//  HMLEIpt btnDownIpt;

    PVOID bufImportExport;
    ULONG ieBufSize;

    HMLEFILEDROPNOTIF *dropFileNotif;

    int han_type;
    int eol_type;
    LONG fgColor;
    LONG bgColor;
    LONG selfgColor;
    LONG selbgColor;

    BOOL autoIndent;
    BOOL readonly;
    BOOL connectedToHIA;

    int xSize,ySize;
    int beginLineN;
    int beginColN;
} HMLE;

MRESULT APIENTRY HMLEWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT APIENTRY HMLEClientWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

void HMLERepositionPage(HMLE *hmle);
void HMLERepositionHoriz(HMLE *hmle);
void HMLERepositionVert(HMLE *hmle);

void HMLEUpdateCurLine(HMLE *hmle);

void HMLEClipboardExport(HMLE *hmle);
void HMLEClipboardImport(HMLE *hmle);

void HMLEStatNotify(HMLE *hmle,USHORT notifCode,MPARAM mp2);
void HMLESetChange(HMLE *hmle);

void HMLEQueryIpt(HMLE *hmle,int x,int y,HMLEIpt *ipt);

void HMLESetPage(HMLE *hmle,int beginLineN);
void HMLELineUp(HMLE *hmle);
void HMLELineDown(HMLE *hmle);
void HMLEPageUp(HMLE *hmle);
void HMLEPageDown(HMLE *hmle);

int HMLEQueryXsize(HMLE *hmle);
int HMLEQueryYsize(HMLE *hmle);
int HMLEQueryRelX(HMLE *hmle);
int HMLEQueryRelY(HMLE *hmle);

#endif // __HMLE_internal_h__
