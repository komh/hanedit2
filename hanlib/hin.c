#include <ctype.h>

#include "hin.h"
#include "hin_internal.h"

#define FKC_NONE(fl)         (!(fl & (KC_ALT|KC_CTRL|KC_SHIFT)))

#define FKC_EXALT(fl)        (!(fl & KC_ALT) && (fl & (KC_SHIFT|KC_CTRL)))
#define FKC_EXCTRL(fl)       (!(fl & KC_CTRL) && (fl & (KC_ALT|KC_SHIFT)))
#define FKC_EXSHIFT(fl)      (!(fl & KC_SHIFT) && (fl & (KC_ALT|KC_CTRL)))

#define FKC_HASALT(fl)          (fl & KC_ALT)
#define FKC_HASCTRL(fl)         (fl & KC_CTRL)
#define FKC_HASSHIFT(fl)        (fl & KC_SHIFT)

#define FKC_ALTONLY(fl)      ((fl & KC_ALT) && !(fl & (KC_SHIFT|KC_CTRL)))
#define FKC_CTRLONLY(fl)     ((fl & KC_CTRL) && !(fl & (KC_ALT|KC_SHIFT)))
#define FKC_SHIFTONLY(fl)    ((fl & KC_SHIFT) && !(fl & (KC_ALT|KC_CTRL)))

#define HIA_cbWINDOWDATA        4
#define WINWORD_INSTANCE        0

MRESULT APIENTRY HIA_WndProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);

static MRESULT hia_wmCreate(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_wmChar(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hia_usermConnect(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermRegisterNotify(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermUnregisterNotify(HWND hwnd,MPARAM mp1,MPARAM mp2);

static MRESULT hia_usermCompleteHch(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermCancelBuf(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermCancelKey(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermChangeHanMode(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermChangeInsertMode(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermChangeKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermSetKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermSetHanMode(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermSetInsertMode(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermQueryWorkingHch(HWND hwnd,MPARAM mp1,MPARAM mp2);
static MRESULT hia_usermQueryState(HWND hwnd,MPARAM mp1,MPARAM mp2);

static void HIA_NotifyToList(HIA* hia,USHORT notifCode,MPARAM mp2);
static void HIA_NotifyToConnected(HIA* hia,USHORT notifCode,MPARAM mp2);

static USHORT hia_convertkey (ULONG kbdtype,USHORT key);
static ULONG hia_iskeypadkey(UCHAR ucScancode);

static unsigned char kbdtable[3][96] = {
{ // kbdtype 2
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,  /*  !"#$%&' */
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,  /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,  /* 01234567 */
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,  /* 89:;<=>? */
  0x40, 0x88, 0xBA, 0x90, 0x8D, 0x86, 0x87, 0x94,  /* @ABCDEFG */
  0xAD, 0xA5, 0xA7, 0xA3, 0xBD, 0xBB, 0xB4, 0xA6,  /* HIJKLMNO */
  0xAC, 0x8A, 0x83, 0x84, 0x8C, 0xAB, 0x93, 0x8F,  /* PQRSTUVW */
  0x92, 0xB3, 0x91, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,  /* XYZ[\]^_ */
  0x60, 0x88, 0xBA, 0x90, 0x8D, 0x85, 0x87, 0x94,  /* `abcdefg */
  0xAD, 0xA5, 0xA7, 0xA3, 0xBD, 0xBB, 0xB4, 0xA4,  /* hijklmno */
  0xAA, 0x89, 0x82, 0x84, 0x8B, 0xAB, 0x93, 0x8E,  /* pqrstuvw */
  0x92, 0xB3, 0x91, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F   /* xyz{|}~  */
},{ // kbdtype 3 390
  0x20, 0xD8, 0x22, 0x23, 0x24, 0x25, 0x26, 0x92,  /*  !"#$%&' */
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0xAD,  /* ()*+,-./ */
  0x91, 0xDD, 0xD6, 0xD3, 0xB3, 0xBA, 0xA5, 0xAC,  /* 01234567 */
  0xBC, 0xB4, 0x3A, 0x89, 0x32, 0x3D, 0x33, 0x3F,  /* 89:;<=>? */
  0x40, 0xC8, 0x21, 0xCB, 0xCA, 0xDA, 0xC3, 0x2F,  /* @ABCDEFG */
  0x27, 0x38, 0x34, 0x35, 0x36, 0x31, 0x30, 0x39,  /* HIJKLMNO */
  0x3E, 0xDC, 0xA6, 0xC7, 0x3B, 0x37, 0xD0, 0xDB,  /* PQRSTUVW */
  0xD4, 0x3C, 0xD9, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,  /* XYZ[\]^_ */
  0x60, 0xD7, 0xB4, 0xAA, 0xBD, 0xAB, 0xA3, 0xBB,  /* `abcdefg */
  0x84, 0x88, 0x8D, 0x82, 0x8E, 0x94, 0x8B, 0x90,  /* hijklmno */
  0x93, 0xD5, 0xA4, 0xC5, 0xA7, 0x85, 0xAD, 0xC9,  /* pqrstuvw */
  0xC2, 0x87, 0xD1, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F   /* xyz{|}~  */
},{ // kbdtype 3 Final
  0x20, 0xC3, 0x22, 0xD8, 0xCF, 0xCE, 0x26, 0x92,  /*  !"#$%&' */
  0x27, 0x7E, 0x2A, 0x2B, 0x2C, 0x29, 0x2E, 0xAD,  /* ()*+,-./ */
  0x91, 0xDD, 0xD6, 0xD3, 0xB3, 0xBA, 0xA5, 0xAC,  /* 01234567 */
  0xBC, 0xB4, 0x34, 0x89, 0x3C, 0x3E, 0x3E, 0x21,  /* 89:;<=>? */
  0xCA, 0xC8, 0x3F, 0xDA, 0xCC, 0xC6, 0xCB, 0xA6,  /* @ABCDEFG */
  0x30, 0x37, 0x31, 0x32, 0x33, 0x22, 0x2D, 0x38,  /* HIJKLMNO */
  0x39, 0xDC, 0xC7, 0xC7, 0xCD, 0x36, 0xC4, 0xDB,  /* PQRSTUVW */
  0xD4, 0x35, 0xD9, 0x28, 0x3A, 0x3C, 0x3D, 0x3B,  /* XYZ[\]^_ */
  0x2A, 0xD7, 0xB4, 0xAA, 0xBD, 0xAB, 0xA3, 0xBB,  /* `abcdefg */
  0x84, 0x88, 0x8D, 0x82, 0x8E, 0x94, 0x8B, 0x90,  /* hijklmno */
  0x93, 0xD5, 0xA4, 0xC5, 0xA7, 0x85, 0xAD, 0xC9,  /* pqrstuvw */
  0xC2, 0x87, 0xD1, 0x25, 0x5C, 0x2F, 0x7E, 0x7F   /* xyz{|}~  */
}};

USHORT hia_convertkey (ULONG kbdtype,USHORT key)
{
    return (kbdtable[kbdtype][key-32] & 0xFF);
}

ULONG hia_iskeypadkey(UCHAR ucScancode)
{
    if (((ucScancode >= 71) && (ucScancode <= 83)) ||
        (ucScancode == 92) || (ucScancode == 90) ||
        (ucScancode == 55)) return TRUE;
    return FALSE;
}

BOOL RegisterHanAutomataClass(HAB hab)
{
    return WinRegisterClass(hab,WCOBJ_HIA,HIA_WndProc,0,HIA_cbWINDOWDATA);
}

HWND HIACreateHanAutomata(HWND hwndOwner,ULONG uId)
{
    return WinCreateWindow(HWND_OBJECT,WCOBJ_HIA,NULL,0,0,0,0,0,hwndOwner,
                HWND_TOP,uId,NULL,NULL);
}

MRESULT APIENTRY HIA_WndProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
   switch (msg) {
   case WM_CREATE:              return hia_wmCreate(hwnd,mp1,mp2);
   case WM_DESTROY:             return hia_wmDestroy(hwnd,mp1,mp2);
   case WM_CHAR:                return hia_wmChar(hwnd,mp1,mp2);

   case HIAM_CONNECT:           return hia_usermConnect(hwnd,mp1,mp2);
   case HIAM_REGISTERNOTIFY:    return hia_usermRegisterNotify(hwnd,mp1,mp2);
   case HIAM_UNREGISTERNOTIFY:  return hia_usermUnregisterNotify(hwnd,mp1,mp2);

   case HIAM_COMPLETEHCH:       return hia_usermCompleteHch(hwnd,mp1,mp2);
   case HIAM_CANCELBUF:         return hia_usermCancelBuf(hwnd,mp1,mp2);
   case HIAM_CANCELKEY:         return hia_usermCancelKey(hwnd,mp1,mp2);
   case HIAM_CHANGEHANMODE:     return hia_usermChangeHanMode(hwnd,mp1,mp2);
   case HIAM_CHANGEINSERTMODE:  return hia_usermChangeInsertMode(hwnd,mp1,mp2);
   case HIAM_CHANGEKBDTYPE:     return hia_usermChangeKbdType(hwnd,mp1,mp2);
   case HIAM_SETKBDTYPE:        return hia_usermSetKbdType(hwnd,mp1,mp2);
   case HIAM_SETHANMODE:        return hia_usermSetHanMode(hwnd,mp1,mp2);
   case HIAM_SETINSERTMODE:     return hia_usermSetInsertMode(hwnd,mp1,mp2);
   case HIAM_QUERYWORKINGHCH:   return hia_usermQueryWorkingHch(hwnd,mp1,mp2);
   case HIAM_QUERYSTATE:        return hia_usermQueryState(hwnd,mp1,mp2);
   default:                     return WinDefWindowProc(hwnd,msg,mp1,mp2);
   }
}

MRESULT hia_wmCreate(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = NULL;
HIABuf  *hiabuf = NULL;
HIANotifWnd *notifList = NULL;
ULONG notifListAllocSize = HIA_NOTIFLIST_DEFAULT_SIZE;
HWND    hwndOwner = WinQueryWindow(hwnd,QW_OWNER);
USHORT  Id      = WinQueryWindowUShort(hwnd,QWS_ID);
int i;

    if (DosAllocMem((PPVOID)&hiabuf,sizeof(HIABuf),fALLOC))
        return MRFROMLONG(TRUE);
#ifdef DEBUG
    assert(hiabuf!=NULL);
#endif
    HIABufClear(hiabuf);

    if (DosAllocMem((PPVOID)&notifList,sizeof(HIANotifWnd)*notifListAllocSize,fALLOC))
        return MRFROMLONG(TRUE);
#ifdef DEBUG
    assert(notifList!=NULL);
#endif
    for (i=0;i<notifListAllocSize;i++)
        {
        notifList[i].hwnd = NULLHANDLE;
        notifList[i].id = 0;
        }

    if (DosAllocMem((PPVOID)&hia,sizeof(HIA),fALLOC))
        return MRFROMLONG(TRUE);
#ifdef DEBUG
    assert(hia!=NULL);
#endif

    hia->hwndHIA = hwnd;
    hia->inbuf = hiabuf;
    hia->kbdtype = HAN_KBD_2;
    hia->hanmode = HCH_ENG;
    hia->insertmode = HAN_INSERT;
    hia->hcode = HCH_JSY;

    hia->notifListAllocSize = notifListAllocSize;
    hia->notifList = notifList;
    hia->notifList[0].hwnd = hwndOwner;
    hia->notifList[0].id = Id;
    hia->responseTo = &(hia->notifList[0]);

    if (!WinSetWindowPtr(hwnd,WINWORD_INSTANCE,(PVOID)hia))
        return MRFROMLONG(TRUE);

    return 0L;
}

MRESULT hia_wmDestroy(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    DosFreeMem(hia->notifList);
    DosFreeMem(hia->inbuf);
    DosFreeMem(hia);

    return 0L;
}

MRESULT hia_wmChar(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
USHORT  fsFlags = SHORT1FROMMP(mp1);
UCHAR   ucVkey  = CHAR3FROMMP(mp2);
UCHAR   ucChar  = CHAR1FROMMP(mp2);
UCHAR   ucScancode = CHAR4FROMMP(mp1);

HIA     *hia    = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

USHORT ckey;

//  printf("HIA:: WM_CHAR\n");

    if (fsFlags & KC_KEYUP) return 0L;
    if (fsFlags & KC_VIRTUALKEY)
        {
        switch (ucVkey) {
        case VK_SPACE:
            WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
            if (FKC_SHIFTONLY(fsFlags))
                WinSendMsg(hwnd,HIAM_CHANGEHANMODE,0L,0L);
            else if (FKC_CTRLONLY(fsFlags))
                WinSendMsg(hwnd,HIAM_CHANGEKBDTYPE,0L,0L);
            else if (FKC_NONE(fsFlags))
                HIA_NotifyToConnected(hia,HIAN_INSERTHCH,MPFROM2SHORT(' ',0));
            return MRFROMLONG(TRUE);    // consumed
            break;
        case VK_TAB:
            WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
            if (FKC_NONE(fsFlags))
                {
                HIA_NotifyToConnected(hia,HIAN_INSERTHCH,MPFROM2SHORT('\t',0));
                return MPFROMLONG(TRUE);    // consumed
                } else {
                return MPFROMLONG(FALSE);
                }
            break;
        case VK_HOME:
        case VK_END:
        case VK_LEFT:
        case VK_UP:
        case VK_DOWN:
        case VK_PAGEUP:
        case VK_PAGEDOWN:
        case VK_DELETE:
            if ((fsFlags & KC_CHAR) && (FKC_NONE(fsFlags))) break;  // considering keypad
            if (hia->inbuf->newpos != HIABUF_NONE)
                WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
            return MPFROMLONG(FALSE);
            break;
        case VK_ENTER:
        case VK_NEWLINE:
            if (hia->inbuf->newpos != HIABUF_NONE)
                WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
            return MPFROMLONG(FALSE);   // not consumed
            break;
        case VK_RIGHT:
            if ((fsFlags & KC_CHAR) && (FKC_NONE(fsFlags))) break;   // considering keypad
            if (hia->inbuf->newpos != HIABUF_NONE)
                {
                WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
                return MRFROMLONG(TRUE);
                } else {
                return MRFROMLONG(FALSE);   // NOT consumed
                }
            break;
        case VK_BACKSPACE:
            if (hia->inbuf->newpos != HIABUF_NONE)
                {
                WinSendMsg(hwnd,HIAM_CANCELKEY,0L,0L);
                return MRFROMLONG(TRUE);
                } else {
                return MRFROMLONG(FALSE);
                }
            break;
        case VK_INSERT:
            if ((fsFlags & KC_CHAR) && (FKC_NONE(fsFlags))) break;   // considering keypad
            if (hia->inbuf->newpos != HIABUF_NONE)
                WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
            if (FKC_NONE(fsFlags))
                {
                WinSendMsg(hwnd,HIAM_CHANGEINSERTMODE,0L,0L);
                return MRFROMLONG(TRUE);
                } else {
                return MRFROMLONG(FALSE);
                }
            break;
        }   // switch
        } // Virtualkey

    if ((fsFlags & KC_CTRL) || (fsFlags & KC_ALT)) return FALSE;
    if ((ucChar < 32) || (ucChar > 127))
        return FALSE;   // not consumed

    if (hia->hanmode == HCH_ENG)
        {
        HIA_NotifyToConnected(hia,HIAN_INSERTHCH,MPFROM2SHORT(ucChar,0));
        return MRFROMLONG(TRUE);
        }

    if ((hia_iskeypadkey(ucScancode))&&(fsFlags & KC_CHAR))
        {
        WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
        HIA_NotifyToConnected(hia,HIAN_INSERTHCH,MPFROM2SHORT(ucChar,0));
        return MRFROMLONG(TRUE);
        }

    if (WinGetKeyState(HWND_DESKTOP,VK_CAPSLOCK))
        {
        if (fsFlags & KC_SHIFT)
            {
            ucChar = toupper(ucChar);
            } else {
            ucChar = tolower(ucChar);
            }
        }
    ckey = hia_convertkey(hia->kbdtype,ucChar);
    {
    HANCHAR completedHch;
    HANCHAR workingHch;
    if (ckey & 0x80)    // if hangul key
        {
        ULONG oldbufState = hia->inbuf->newpos;
        if (hia->kbdtype == HAN_KBD_2)
            completedHch = HIAAutomata2(hia,ckey);
            else
            completedHch = HIAAutomata3(hia,ckey);
        workingHch = SHORT1FROMMR(WinSendMsg(hwnd,HIAM_QUERYWORKINGHCH,0L,0L));

        if (completedHch)       // if hch completed
            {
            HIA_NotifyToConnected(hia,HIAN_COMPO_COMPLETE,MPFROM2SHORT(completedHch,workingHch));
            HIA_NotifyToConnected(hia,HIAN_INSERTHCH,MPFROM2SHORT(completedHch,workingHch));
            if (workingHch)     // begin composition of new hch
                HIA_NotifyToConnected(hia,HIAN_COMPO_BEGIN,MPFROM2SHORT(completedHch,workingHch));

            } else {    // if hch not completed

            if (workingHch)     // if exist composing hch
            if (oldbufState)
                HIA_NotifyToConnected(hia,HIAN_COMPO_STEP,
                    MPFROM2SHORT(0,workingHch));
                else
                HIA_NotifyToConnected(hia,HIAN_COMPO_BEGIN,
                    MPFROM2SHORT(0,workingHch));
            }

        return MRFROMLONG(TRUE);    // consumed

        } else {  // !@#$%^&*() ,etc

        WinSendMsg(hwnd,HIAM_COMPLETEHCH,0L,0L);
        HIA_NotifyToConnected(hia,HIAN_INSERTHCH,
            MPFROM2SHORT(ckey,0));
        return MRFROMLONG(TRUE);    // consumed

        }
        }
    return MRFROMLONG(FALSE);    // not consumed
}

MRESULT hia_usermCompleteHch(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HANCHAR hch;

//  printf("HIAM_COMPLETEHCH\n");

    if (hia->inbuf->newpos == HIABUF_NONE) return 0L;
    hch = HIABufPeekHch(hia->inbuf);
    HIA_NotifyToConnected(hia,HIAN_COMPO_COMPLETE,MPFROM2SHORT(hch,0));
    HIA_NotifyToConnected(hia,HIAN_INSERTHCH,MPFROM2SHORT(hch,0));
    HIABufClear(hia->inbuf);

    return MRFROM2SHORT(hch,0);
}

MRESULT hia_usermCancelBuf(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG   temp;

//  printf("HIAM_CANCELBUF\n");

    temp = hia->inbuf->newpos;
    HIABufClear(hia->inbuf);
    HIA_NotifyToConnected(hia,HIAN_COMPO_CANCEL,0);
    return MRFROMLONG(temp);
}

MRESULT hia_usermCancelKey(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HANCHAR hch;

//  printf("HIAM_CANCELKEY\n");

    HIABufPop(hia->inbuf);
    hch = HIABufPeekHch(hia->inbuf);
    if (hia->inbuf->newpos)
        HIA_NotifyToConnected(hia,HIAN_COMPO_STEPBACK,
            MPFROM2SHORT(0,hch));
        else
        HIA_NotifyToConnected(hia,HIAN_COMPO_CANCEL,0);
    return MRFROM2SHORT(hch,0);
}

MRESULT hia_usermChangeHanMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

//  printf("HIAM_CHANGEHANMODE\n");

    if (hia->hanmode == HCH_HAN)
        hia->hanmode = HCH_ENG;
        else
        hia->hanmode = HCH_HAN;
    HIA_NotifyToList(hia,HIAN_HANMODECHANGED,MPFROMLONG(hia->hanmode));

    return MRFROMLONG(TRUE);
}

MRESULT hia_usermChangeInsertMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

//  printf("HIAM_CHANGEINSERTMODE\n");

    if (hia->insertmode == HAN_INSERT)
        hia->insertmode = HAN_OVERWRITE;
        else
        hia->insertmode = HAN_INSERT;

    HIA_NotifyToList(hia,HIAN_INSERTMODECHANGED,MPFROMLONG(hia->insertmode));
    return MRFROMLONG(TRUE);
}

MRESULT hia_usermChangeKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA     *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

//  printf("HIAM_CHANGEKBDTYPE\n");

    switch (hia->kbdtype) {
    case HAN_KBD_2:         hia->kbdtype = HAN_KBD_390; break;
    case HAN_KBD_390:       hia->kbdtype = HAN_KBD_3FINAL;  break;
    case HAN_KBD_3FINAL:    hia->kbdtype = HAN_KBD_2;       break;
    default:                hia->kbdtype = HAN_KBD_2;       break;
    }

    HIA_NotifyToList(hia,HIAN_KBDTYPECHANGED,MPFROMLONG(hia->kbdtype));
    return MRFROMLONG(TRUE);
}

MRESULT hia_usermSetKbdType(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA     *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    hia->kbdtype = (ULONG)mp1;
    HIA_NotifyToList(hia,HIAN_KBDTYPECHANGED,MPFROMLONG(hia->kbdtype));
    return MRFROMLONG(TRUE);
}

MRESULT hia_usermSetHanMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA     *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    hia->hanmode = (ULONG)mp1;
    if ((hia->hanmode != HCH_HAN) && (hia->hanmode != HCH_ENG))
        hia->hanmode = HCH_ENG;
    HIA_NotifyToList(hia,HIAN_HANMODECHANGED,MPFROMLONG(hia->hanmode));
    return MRFROMLONG(TRUE);
}

MRESULT hia_usermSetInsertMode(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA     *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    hia->insertmode = (ULONG)mp1;
    if ((hia->insertmode != HAN_INSERT) && (hia->insertmode != HAN_OVERWRITE))
        hia->hanmode = HAN_INSERT;
    HIA_NotifyToList(hia,HIAN_INSERTMODECHANGED,MPFROMLONG(hia->insertmode));
    return MRFROMLONG(TRUE);
}

MRESULT hia_usermQueryWorkingHch(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
HANCHAR hch;

//  printf("HIAM_QUERYWORKINGHCH\n");

    hch = HIABufPeekHch(hia->inbuf);

    return MRFROMSHORT(hch);
}

MRESULT hia_usermQueryState(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
ULONG   flState = 0;

//  printf("HIAM_QUERYSTATE\n");

    flState = (hia->inbuf->newpos) << 16;

    if (hia->hanmode == HCH_HAN)
        flState |= HIAST_HANMODE;

    if (hia->insertmode == HAN_INSERT)
        flState |= HIAST_INSERTMODE;

    if (hia->kbdtype == HAN_KBD_390)
        flState |= HIAST_KBD390;
    else if (hia->kbdtype == HAN_KBD_3FINAL)
        flState |= HIAST_KBD3FINAL;


    if (hia->hcode == HCH_WKS)
        flState |= HIAST_KSMODE;

    return MRFROMLONG(flState);
}

MRESULT hia_usermConnect(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);

    if (mp1!=NULL)
        {
        int i;

        for (i=0;i<hia->notifListAllocSize;i++)
            if (hia->notifList[i].hwnd == (HWND)mp1)
                {
                HIA_NotifyToConnected(hia,HIAN_CONNECT,MPFROMLONG(FALSE));
                hia->responseTo = &(hia->notifList[i]);
                HIA_NotifyToConnected(hia,HIAN_CONNECT,MPFROMLONG(TRUE));
                return MRFROMLONG(TRUE);
                }
        return MRFROMLONG(FALSE);

        } else {

        if (hia->responseTo != &(hia->notifList[0]))
            {
            HIA_NotifyToConnected(hia,HIAN_CONNECT,MPFROMLONG(FALSE));
            hia->responseTo = &(hia->notifList[0]);
            HIA_NotifyToConnected(hia,HIAN_CONNECT,MPFROMLONG(TRUE));
            }
        }

    return MRFROMLONG(TRUE);
}

MRESULT hia_usermRegisterNotify(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int i;

    if (mp1==NULL) return MRFROMLONG(FALSE);

    for (i=0;i<hia->notifListAllocSize;i++)
        if (hia->notifList[i].hwnd==NULLHANDLE)
            {
            hia->notifList[i].hwnd = (HWND)mp1;
            hia->notifList[i].id = SHORT1FROMMP(mp2);
            return MRFROMLONG(TRUE);
            }
    return MRFROMLONG(FALSE);
}

MRESULT hia_usermUnregisterNotify(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HIA *hia = WinQueryWindowPtr(hwnd,WINWORD_INSTANCE);
int i;

    if (mp1==NULL) return MRFROMLONG(FALSE);

    for (i=0;i<hia->notifListAllocSize;i++)
        if (hia->notifList[i].hwnd==(HWND)mp1)
            {
            hia->notifList[i].hwnd = NULLHANDLE;
            hia->notifList[i].id = 0;
            return MRFROMLONG(TRUE);
            }
    return MRFROMLONG(FALSE);
}

void HIA_NotifyToList(HIA* hia,USHORT notifCode,MPARAM mp2)
{
int i;

    for (i=0;i<hia->notifListAllocSize;i++)
        if (hia->notifList[i].hwnd != NULLHANDLE)
            WinPostMsg(hia->notifList[i].hwnd,WM_CONTROL,
                MPFROM2SHORT(hia->notifList[i].id,notifCode),mp2);
}

void HIA_NotifyToConnected(HIA *hia,USHORT notifCode,MPARAM mp2)
{
    WinSendMsg(hia->responseTo->hwnd,WM_CONTROL,
        MPFROM2SHORT(hia->responseTo->id,notifCode),mp2);
}

