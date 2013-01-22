#ifndef __HCHLB_H__
#define __HCHLB_H__

#include <os2.h>

#include "../hanlib/han.h"

#include "hchlbdlg.h"
#include "hjseldlg.h"
#include "scseldlg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WC_HCHLB        "HanCharListBox"

BOOL RegisterHanCharListBoxControl( HAB hab );

// HanCharListBox control user messages
#define HCHLM_QUERYCOUNT        ( WM_USER + 1 )
#define HCHLM_DELETEALL         ( WM_USER + 2 )
#define HCHLM_DELETE            ( WM_USER + 3 )
#define HCHLM_INSERT            ( WM_USER + 5 )
#define HCHLM_QUERYHCH          ( WM_USER + 6 )
#define HCHLM_QUERYSELECTION    ( WM_USER + 7 )
#define HCHLM_QUERYTOPINDEX     ( WM_USER + 8 )
#define HCHLM_SETTOPINDEX       ( WM_USER + 9 )
#define HCHLM_SEARCHHCH         ( WM_USER + 10 )
#define HCHLM_SELECTITEM        ( WM_USER + 11 )
#define HCHLM_QUERYVERTINT      ( WM_USER + 12 )
#define HCHLM_SETVERTINT        ( WM_USER + 13 )
#define HCHLM_QUERYHORZINT      ( WM_USER + 14 )
#define HCHLM_SETHORZINT        ( WM_USER + 15 )
#define HCHLM_REFRESH           ( WM_USER + 16 )

// HanCharListBox control window notification codes
#define HCHLN_ENTER                 0x0001
#define HCHLN_KILLFOCUS             0x0002
#define HCHLN_SETFOCUS              0x0004
#define HCHLN_SELECT                0x0008

// HanCharListBox control window values related to user messages.
#define HCHLIT_INDEXERROR           ( -3 )
#define HCHLIT_MEMERROR             ( -2 )
#define HCHLIT_ERROR                ( -1 )

// Search mode values
#define HCHLSS_FORWARD              0
#define HCHLSS_BACKWARD             1

// index values
#define HCHLIT_FIRST                0
#define HCHLIT_END                  ( -1 )

#define HCHLIT_NONE                 ( -1 )

#ifdef __cplusplus
}
#endif

#endif
