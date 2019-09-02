#ifndef __HEF_H__
#define __HEF_H__

#if defined (__cplusplus)
extern "C" {
#endif

// incomplete
// not tested
// not verified
// verified
// bugfixed 0
// bugfixed 1
// ...

#define WC_HEF         ("HanEntryField")

#define HEM_SETHIA					(WM_USER+1)		// not tested
#define HEM_QUERYHIA				(WM_USER+2)		// not tested
#define HEM_QUERYTEXTLENGTH			(WM_USER+7)		// not tested
#define HEM_QUERYTEXT				(WM_USER+8)		// not tested

#define HEM_SETTEXTLIMIT			(WM_USER+10)	// not tested
#define HEM_QUERYTEXTLIMIT			(WM_USER+11)	// not tested

#define HEM_SETHANTYPE				(WM_USER+12)	// not tested
#define HEM_QUERYHANTYPE			(WM_USER+13)	// not tested
#define HEM_SETCHANGED				(WM_USER+14)	// not tested
#define HEM_QUERYCHANGED			(WM_USER+15)	// not tested
#define HEM_SETREADONLY				(WM_USER+16)	// not tested
#define HEM_QUERYREADONLY			(WM_USER+17)	// not tested
#define HEM_SETSEL					(WM_USER+18)	// not tested
#define HEM_QUERYSEL				(WM_USER+19) 	// not tested
#define HEM_SETFIRSTCHAR			(WM_USER+20)	// not tested
#define HEM_QUERYFIRSTCHAR			(WM_USER+21)	// not tested

#define HEM_SETTEXTCOLOR			(WM_USER+30)	// not tested
#define HEM_QUERYTEXTCOLOR			(WM_USER+31)	// not tested
#define HEM_SETBACKCOLOR			(WM_USER+32)	// not tested
#define HEM_QUERYBACKCOLOR			(WM_USER+33)	// not tested
#define HEM_SETSELTEXTCOLOR			(WM_USER+34)	// not tested
#define HEM_QUERYSELTEXTCOLOR		(WM_USER+35)	// not tested
#define HEM_SETSELBACKCOLOR			(WM_USER+36)	// not tested
#define HEM_QUERYSELBACKCOLOR		(WM_USER+37)	// not tested

#define HEM_COPY					(WM_USER+40)	// incomplete
#define HEM_CUT						(WM_USER+41)	// incomplete
#define HEM_PASTE					(WM_USER+42)	// not tested
#define HEM_CLEAR					(WM_USER+43)	// not tested

#define HEM_REFRESH					(WM_USER+50)	// not tested

#define HEM_DUMP					(WM_USER+100)	// not verified

#define HEF_HAN_KS		0
#define HEF_HAN_SY		1			// sangyong johab

#define HEF_DEFAULT_LINESIZE	4096

// --------------------------------------------------------------------------
// Define the control styles
// --------------------------------------------------------------------------
#define HES_LEFT			0x0000			// incomplete
#define HES_CENTER			0x0001
#define HES_RIGHT			0x0002
#define HES_AUTOSCROLL		0x0004
#define HES_MARGIN			0x0008			// incomplete
#define HES_AUTOTAB			0x0010
#define HES_READONLY		0x0020			// not verified
#define HES_COMMAND			0x0040
#define HES_UNREADABLE		0x0080			// not verified
#define HES_AUTOSIZE		0x0200

#define HES_STATUSNOTIFY	0x0400			// not tested
#define HES_AUTOFOCUS		0x0800
#define HES_NOAUTOCREATEHIA	0x1000
#define HES_ACCEPTFILEDROP	0x2000

// --------------------------------------------------------------------------
// Define the WM_CONTROL notification codes
// --------------------------------------------------------------------------
#define HEN_SETFOCUS				0x0001	// not tested
#define HEN_KILLFOCUS				0x0002	// not tested
#define HEN_CHANGE					0x0004	// not verified
#define HEN_SCROLL					0x0008
#define HEN_MEMERROR				0x0010
#define HEN_OVERFLOW				0x0020
#define HEN_INSERTMODETOGGLE		0x0040

#define HEN_CHANGEDHANINPUTMODE		0x0080
#define HEN_CHANGEDKBDTYPE			0x0100
#define HEN_CHANGEDINSERTMODE		0x0200

#define HEN_BEGINSELECTION			0x0400	// not tested
#define HEN_ENDSELECTION			0x0800	// not tested



// --------------------------------------------------------------------------
// Define the HEM_QUERYSEL query mode values
// --------------------------------------------------------------------------
#define HMLFQS_MINMAXSEL			0
#define HMLFQS_MINSEL				1
#define HMLFQS_MAXSEL				2
#define HMLFQS_ANCHORSEL			3
#define HMLFQS_CURSORSEL			4

// --------------------------------------------------------------------------
// Define the control data structure
// --------------------------------------------------------------------------

typedef struct tagHEFCTLDATA {
	USHORT cb;
	ULONG han_type;
	HWND hwndHIA;
} HEFCTLDATA, *PHEFCTLDATA;

BOOL EXPENTRY RegisterHanEntryFieldControl(HAB hab);

#if defined (__cplusplus)
}
#endif

#endif // __HEF_H__
