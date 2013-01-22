#ifndef _HIN_H_
#define _HIN_H_

#define INCL_PM
#include <os2.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define WCOBJ_HIA       ("HanInputAutomata")

#define HIAM_CONNECT            (WM_USER+0)
#define HIAM_REGISTERNOTIFY     (WM_USER+90)
#define HIAM_UNREGISTERNOTIFY   (WM_USER+91)

#define HIAM_COMPLETEHCH        (WM_USER+1)
#define HIAM_CANCELBUF          (WM_USER+2)
#define HIAM_CANCELKEY          (WM_USER+3)

#define HIAM_CHANGEHANMODE      (WM_USER+10)
#define HIAM_CHANGEINSERTMODE   (WM_USER+11)
#define HIAM_CHANGEKBDTYPE      (WM_USER+12)

#define HIAM_SETKBDTYPE         (WM_USER+13)
#define HIAM_SETHANMODE         (WM_USER+14)
#define HIAM_SETINSERTMODE      (WM_USER+15)

#define HIAM_QUERYWORKINGHCH    (WM_USER+20)
#define HIAM_QUERYSTATE         (WM_USER+21)

#define HIAM_QUERYHANJAKEYCHECKPROC         ( WM_USER + 22 )
#define HIAM_SETHANJAKEYCHECKPROC           ( WM_USER + 23 )

#define HIAM_QUERYSPECIALCHARKEYCHECKPROC   ( WM_USER + 24 )
#define HIAM_SETSPECIALCHARKEYCHECKPROC     ( WM_USER + 25 )

#define HIAN_COMPO_BEGIN        0x0000
#define HIAN_COMPO_STEP         0x0001
#define HIAN_COMPO_COMPLETE     0x0002
#define HIAN_COMPO_STEPBACK     0x0003
#define HIAN_COMPO_CANCEL       0x0004
#define HIAN_INSERTHCH          0x0005
#define HIAN_HANMODECHANGED     0x0006
#define HIAN_INSERTMODECHANGED  0x0007
#define HIAN_KBDTYPECHANGED     0x0008
#define HIAN_CONNECT            0x0009
#define HIAN_HGHJCONVERT        0X000A

#define HIAST_HANMODE       0x00000001
#define HIAST_INSERTMODE    0x00000002
#define HIAST_KBD2          0x00000000
#define HIAST_KBD390        0x00000004
#define HIAST_KBD3FINAL     0x00000008
#define HIAST_KBDRESERVED   0x00000010
#define HIAST_KBDMASK       0x0000001C
#define HIAST_KSMODE        0x00000020

#define HIAST_COMPO_NONE    0x00000000
#define HIAST_COMPO_FIRST   0x00010000
#define HIAST_COMPO_DFIRST  0x00020000
#define HIAST_COMPO_MID     0x00030000
#define HIAST_COMPO_DMID    0x00040000
#define HIAST_COMPO_LAST    0x00050000
#define HIAST_COMPO_DLAST   0x00060000
#define HIAST_COMPO_END1    0x00070000
#define HIAST_COMPO_END2    0x00080000

/* INSERT MODE */
#define HAN_OVERWRITE   0
#define HAN_INSERT  1

/* KBD TYPE */
#define HAN_KBD_2       0
#define HAN_KBD_390     1
#define HAN_KBD_3FINAL  2

BOOL RegisterHanAutomataClass(HAB hab);
HWND HIACreateHanAutomata(HWND hwndOwner,ULONG uId);

#if defined (__cplusplus)
}
#endif

#endif // _HIN_H_
