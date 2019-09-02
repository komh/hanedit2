#ifndef __HEF_internal_h__
#define __HEF_internal_h__

#define INCL_PM
#define INCL_DOSMEMMGR
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#include <assert.h>
#include "../etclib/debugutil.h"
#endif

#include "../hanlib/han.h"
#include "HEF.h"

#define WC_HEF_CLIENT           ("HanEntryField:Client")

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

#define WINWORD_INSTANCE    0

#define HEFID_CLIENT        (TID_USERMAX-1)

#define HEFCID_TIMER        (TID_USERMAX-1)
#define HEFCID_HIA          (TID_USERMAX-2)

#define HEF_BORDERWIDTH     1

typedef struct tagHEF {
    HWND hwndHEF;
    HWND hwndClient;
    HWND hwndHIA;
    int  connectedToHIA;
    HEFCTLDATA *pCtlData;
    HANCHAR hchComposing;
    UCHAR *str;
    int allocsize;
    int anchorStx;
    int curStx;
    BOOL selection;
    int beginCol;
    int fgColor;
    int bgColor;
    int selfgColor;
    int selbgColor;
    BOOL changed;
    BOOL readonly;
    BOOL unreadable;

    BOOL runningHIA;

    int xSize;
    char *outputBuf;
} HEF;

MRESULT APIENTRY HEFWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT APIENTRY HEFClientWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// Client

#define HEMC_REFRESH                (WM_USER+1)
#define HEMC_REFRESHCURSOR          (WM_USER+2)

#define HEFSelectionState(hef)      ((hef)->selection)

void HEFWND_Notify(HWND hwnd,USHORT notifCode,MPARAM mp2);
void HEFWND_Client_Notify(HWND hwnd,USHORT notifCode,MPARAM mp2);
void HEFStatNotify(HEF *hef,USHORT notifCode,MPARAM mp2);

void HEFBeginSelectionHere(HEF *hef,int stx);
void HEFEndSelection(HEF *hef);
void HEFDeleteSelection(HEF *hef);
int HEFClipboardExport(HEF *hef);
int HEFClipboardImport(HEF *hef);
void HEFReposition(HEF *hef);

char *HEFGetSelection(HEF *hef,char *buf);
int HEFSelectionSize(HEF *hef);
int HEFInsert(HEF *hef,const char* str);
int HEFInsertHch(HEF *hef,HANCHAR hch);

void HEFSetChanged(HEF *hef);

void HEFMoveLeft(HEF *hef);
void HEFMoveRight(HEF *hef);
void HEFPrevWord(HEF *hef);
void HEFNextWord(HEF *hef);

void HEFFormatLine(HEF *hef,char *str,int fixedLen);

int HEFQueryStx(HEF *hef,int x);

#endif // __HEF_internal_h__
