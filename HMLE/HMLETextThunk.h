#ifndef __HMLETEXTTHUNK_H__
#define __HMLETEXTTHUNK_H__

typedef struct tagHMLETextThunk HMLETextThunk;
struct tagHMLETextThunk {
	ULONG size;
	ULONG len;
	UCHAR *str;
	UCHAR *curLine;
};

HMLETextThunk* HMLECreateTextThunk(UCHAR *str);
void HMLEDestroyTextThunk(HMLETextThunk *this);
void HMLETextThunkPresetSize(HMLETextThunk *this,int size);

int HMLETextThunkQueryNumberOfLines(HMLETextThunk *this);
UCHAR* HMLETextThunkQueryStr(HMLETextThunk *this);
void HMLETextThunkAddStr(HMLETextThunk *this,const UCHAR *str);
void HMLETextThunkAddLine(HMLETextThunk *this,const UCHAR *str);

int HMLETextThunkQueryLineLen(HMLETextThunk *this,int lineNumber);
UCHAR* HMLETextThunkQueryLine(HMLETextThunk *this,int lineNumber);
UCHAR* HMLETextThunkGetLine(HMLETextThunk *this,int lineNumber,UCHAR *pchBuf);

void HMLETextThunkRewind(HMLETextThunk *this);
UCHAR* HMLETextThunkNextLine(HMLETextThunk *this);
int HMLETextThunkQueryCurLineLen(HMLETextThunk *this);
UCHAR* HMLETextThunkQueryCurLine(HMLETextThunk *this);
UCHAR* HMLETextThunkGetCurLine(HMLETextThunk *this,UCHAR *pchBuf);

int HMLETextThunkQueryLFStringLen(HMLETextThunk *this);
int HMLETextThunkQueryCRLFStringLen(HMLETextThunk *this);
UCHAR* HMLETextThunkGetLFString(HMLETextThunk *this,UCHAR *buf);
UCHAR* HMLETextThunkGetCRLFString(HMLETextThunk *this,UCHAR *buf);

#endif // __HMLETEXTTHUNK_H__
