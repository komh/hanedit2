#ifndef _HST_H_
#define _HST_H_

#if defined (__cplusplus)
extern "C" {
#endif

#define WC_HST         ("HanStaticText")

#define HSTM_QUERYTEXTLENGTH		(WM_USER+7)

#define HSTM_SETTEXTCOLOR			(WM_USER+30)	// tested
#define HSTM_QUERYTEXTCOLOR			(WM_USER+31)
#define HSTM_SETBACKCOLOR			(WM_USER+32)	// tested
#define HSTM_QUERYBACKCOLOR			(WM_USER+33)

#define HSTM_REFRESH				(WM_USER+50)	// verified

#define HSTM_SETHANDLE				(WM_USER+60)
#define HSTM_QUERYHANDLE			(WM_USER+61)

// --------------------------------------------------------------------------
// Define the control styles
// --------------------------------------------------------------------------

#define HSTS_TEXT					0x0001
#define HSTS_AUTOSIZE				0x0040

// --------------------------------------------------------------------------
// Define the WM_CONTROL notification codes
// --------------------------------------------------------------------------

typedef struct tagHSTCTLDATA {
	USHORT cb;
} HSTCTLDATA, *PHSTCTLDATA;

BOOL EXPENTRY RegisterHanStaticTextControl(HAB hab);

#if defined (__cplusplus)
}
#endif

#endif // _HST_H_
