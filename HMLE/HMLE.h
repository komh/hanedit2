#ifndef __HMLE_H__
#define __HMLE_H__

#if defined (__cplusplus)
extern "C" {
#endif

#define WC_HMLE         ("HanMLE")

#define HMLM_NEWDOC                 (WM_USER+1)
#define HMLM_CHECKDOC               (WM_USER+2)
#define HMLM_SETIMPORTEXPORT        (WM_USER+3)
#define HMLM_QUERYIMPORTEXPORT      (WM_USER+4)
#define HMLM_IMPORT                 (WM_USER+5)
#define HMLM_EXPORT                 (WM_USER+6)
#define HMLM_QUERYTEXTLENGTH        (WM_USER+7)
#define HMLM_QUERYFORMATTEXTLENGTH  (WM_USER+8)
#define HMLM_QUERYSELTEXT           (WM_USER+9)

#define HMLM_SETEOLTYPE             (WM_USER+10)
#define HMLM_QUERYEOLTYPE           (WM_USER+11)
#define HMLM_SETTEXTFORMAT          (WM_USER+10)
#define HMLM_QUERYTEXTFORMAT        (WM_USER+11)

#define HMLM_SETHANTYPE             (WM_USER+12)
#define HMLM_QUERYHANTYPE           (WM_USER+13)
#define HMLM_SETCHANGED             (WM_USER+14)
#define HMLM_QUERYCHANGED           (WM_USER+15)
#define HMLM_SETREADONLY            (WM_USER+16)
#define HMLM_QUERYREADONLY          (WM_USER+17)
#define HMLM_SETSEL                 (WM_USER+18)
#define HMLM_QUERYSEL               (WM_USER+19)
#define HMLM_SETFIRSTCHAR           (WM_USER+20)
#define HMLM_QUERYFIRSTCHAR         (WM_USER+21)
#define HMLM_SETTABSTOP             (WM_USER+22)
#define HMLM_QUERYTABSTOP           (WM_USER+23)
#define HMLM_SETAUTOINDENT          (WM_USER+24)
#define HMLM_QUERYAUTOINDENT        (WM_USER+25)

#define HMLM_SETTEXTCOLOR           (WM_USER+30)    // tested
#define HMLM_QUERYTEXTCOLOR         (WM_USER+31)
#define HMLM_SETBACKCOLOR           (WM_USER+32)    // tested
#define HMLM_QUERYBACKCOLOR         (WM_USER+33)
#define HMLM_SETSELTEXTCOLOR        (WM_USER+34)    // tested
#define HMLM_QUERYSELTEXTCOLOR      (WM_USER+35)
#define HMLM_SETSELBACKCOLOR        (WM_USER+36)    // tested
#define HMLM_QUERYSELBACKCOLOR      (WM_USER+37)

#define HMLM_COPY                   (WM_USER+40)
#define HMLM_CUT                    (WM_USER+41)
#define HMLM_PASTE                  (WM_USER+42)
#define HMLM_CLEAR                  (WM_USER+43)

#define HMLM_INSERT                 (WM_USER+44)    // incomplete : return value
#define HMLM_DELETE                 (WM_USER+45)    // incomplete : return value

#define HMLM_REFRESH                (WM_USER+50)    // verified
#define HMLM_REFRESHSCROLLBAR       (WM_USER+51)    // tested
#define HMLM_QUERYHIA               (WM_USER+52)
#define HMLM_SETHIA                 (WM_USER+53)

#define HMLM_SEARCH                 (WM_USER+60)    // incomplete

#define HMLM_SETWRAP                (WM_USER+70)
#define HMLM_QUERYWRAP              (WM_USER+71)

#define HMLM_DUMP                   (WM_USER+100)

#define HMLE_EOL_CRLF       0
#define HMLE_EOL_LF         1

#define HMLE_HAN_KS     0
#define HMLE_HAN_SY     1           // sangyong johab

#define HMLE_ENG        (HCH_ENG)
#define HMLE_HAN        (HCH_HAN)

#define HMLE_INSERT     (HAN_INSERT)
#define HMLE_OVERWRITE  (HAN_OVERWRITE)

#define HMLE_DEFAULT_MAXLINESIZE    4096

// --------------------------------------------------------------------------
// Define the control styles
// --------------------------------------------------------------------------
#define HMLS_WORDWRAP           0x0001
#define HMLS_BORDER             0x0002
#define HMLS_VSCROLL            0x0004
#define HMLS_HSCROLL            0x0008
#define HMLS_READONLY           0x0010
#define HMLS_IGNORETAB          0x0020
#define HMLS_DISABLEUNDO        0x0040
#define HMLS_DEBUG              0x0080

#define HMLS_STATUSNOTIFY       0x0100
#define HMLS_AUTOFOCUS          0x0200
#define HMLS_NOAUTOCREATEHIA    0x0400
#define HMLS_ACCEPTFILEDROP     0x0800

// --------------------------------------------------------------------------
// Define the WM_CONTROL notification codes
// --------------------------------------------------------------------------
#define HMLN_OVERFLOW               1
#define HMLN_SETFOCUS               2
#define HMLN_KILLFOCUS              3
#define HMLN_CHANGEDHANINPUTMODE    4
#define HMLN_CHANGEDKBDTYPE         5
#define HMLN_CHANGEDINSERTMODE      6
#define HMLN_CHANGED                7
#define HMLN_CLPBDFAIL              8
#define HMLN_BEGINSELECTION         9
#define HMLN_ENDSELECTION           10
#define HMLN_FILEDROP               11
#define HMLN_NOTIFYCURSORPOS        12

// --------------------------------------------------------------------------
// Define the HMLM_QUERYSEL query mode values
// --------------------------------------------------------------------------
#define HMLFQS_MINMAXSEL            0
#define HMLFQS_MINSEL               1
#define HMLFQS_MAXSEL               2
#define HMLFQS_ANCHORSEL            3
#define HMLFQS_CURSORSEL            4

// --------------------------------------------------------------------------
// Define the control data structure
// --------------------------------------------------------------------------

typedef struct tagHMLECTLDATA {
    USHORT  cb;
    ULONG   eol_type;
    ULONG   han_type;
    BOOL    autoIndent;
    ULONG   maxLineSize;
    int     wordWrapSize;
    HWND    hwndHIA;
} HMLECTLDATA, *PHMLECTLDATA;

typedef struct tagHMLEFILEDROPNOTIF {
    ULONG cb;
    USHORT usOperation;
    PSZ fullPath;
} HMLEFILEDROPNOTIF,*PHMLEFILEDROPNOTIF;

BOOL EXPENTRY RegisterHanMLEControl(HAB hab);

#if defined (__cplusplus)
}
#endif

#endif // __HMLE_H__
