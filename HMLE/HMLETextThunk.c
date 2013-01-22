#include <string.h>
#include "HMLE_internal.h"

HMLETextThunk* HMLECreateTextThunk(UCHAR *str)
{
HMLETextThunk* textThunk = NULL;

	textThunk = malloc(sizeof(HMLETextThunk));
	textThunk->len = strlen(str);
	textThunk->size = textThunk->len+1;
	textThunk->str = malloc(textThunk->size);
	crlf2lf(textThunk->str,str);
	
	return textThunk;
}

void HMLEDestroyTextThunk(HMLETextThunk *this)
{
	if (this==NULL) return;

	free(this->str);
	free(this);
}

void HMLETextThunkPresetSize(HMLETextThunk *this,int newsize)
{
UCHAR *newstr;
	if (this==NULL) return;
	if (newsize <= this->size) return;

	newstr = malloc(newsize);
	if (newstr==NULL) return;
	memcpy(newstr,this->str,this->len+1);
	free(this->str);
	
	this->str = newstr;
	this->size = newsize;
}

void HMLETextThunkAddStr(HMLETextThunk *this,const UCHAR *str)
{
int newsize;
UCHAR *newstr;
int addlen;

	if (this==NULL) return;

	addlen = strlen(str);
	newsize = this->len + addlen + 1;
	if (newsize > this->size)
		{
		newstr = malloc(newsize);
		memcpy(newstr,this->str,this->len+1);
		free(this->str);
		this->str = newstr;
		this->size = newsize;
		}
	memcpy(this->str+this->len,str,addlen+1);
	this->len += addlen;
}

void HMLETextThunkAddLine(HMLETextThunk *this,const UCHAR *str)
{
int newsize;
UCHAR *newstr=NULL;
int addlen;

	if (this==NULL) return;
	
	addlen = strlen(str);
	newsize = this->len + addlen + 1 + 1;
	if (newsize > this->size)
		{
		newstr = malloc(newsize);
		memcpy(newstr,this->str,this->len+1);
		free(this->str);
		this->str = newstr;
		this->size = newsize;
		}
	memcpy(this->str+this->len,str,addlen);
	this->len += addlen;
	this->str[this->len] = '\n';
	this->len ++;
	this->str[this->len] = '\0';
}

UCHAR *HMLETextThunkQueryStr(HMLETextThunk *this)
{
	return this->str;
}

int HMLETextThunkQueryNumberOfLines(HMLETextThunk *this)
{
int i=0;
UCHAR *pch=NULL;

	if (this==NULL) return -1;
	
	pch = this->str;
	while (*pch) if (*(pch++)=='\n') i++;
	i++; // for '\0' terminated line (last line)
	return i;
}

UCHAR* HMLETextThunkQueryLine(HMLETextThunk *this,int lineNumber)
{
int ln=0;
UCHAR *pch=NULL;

	if (this==NULL) return NULL;

	if (lineNumber==0) return this->str;
	pch = this->str;
	while (*pch)
		{
		if (*pch=='\n')
			{
			ln++;
			if (ln==lineNumber) return (pch+1);
			}
		pch++;
		}
	return NULL;
}

int HMLETextThunkQueryLineLen(HMLETextThunk *this,int lineNumber)
{
int ln=0;
UCHAR *pch=NULL;
UCHAR *base=NULL;

	if (this==NULL) return -1;
	
	pch = this->str;
	base=pch;
	while (*pch)
		{
		if (*pch=='\n')
			{
			if (ln==lineNumber) return ((int)pch-(int)base);
			ln++;
			if (ln==lineNumber) base=pch+1;
			}
		pch++;
		}
	if (ln<lineNumber) return -1;
	return ((int)pch-(int)base);
}

UCHAR* HMLETextThunkGetLine(HMLETextThunk *this,int lineNumber,UCHAR *pchBuf)
{
int ln=0;
char *pch=NULL;
char *org=pchBuf;

	if (this==NULL) return NULL;
	
	pch = this->str;
	while (*pch)
		{
		if (*pch=='\n')
			{
			if (ln==lineNumber)
				{
				*pchBuf = 0;
				return org;
				}
			ln++;
			} else if (ln==lineNumber) *(pchBuf++) = *pch;
		pch++;
		}
	if (ln<lineNumber) return NULL;
	*pchBuf = 0;
	return org;
}

int HMLETextThunkQueryLFStringLen(HMLETextThunk *this)
{
	if (this==NULL) return -1;
	return strlen(this->str);
}

int HMLETextThunkQueryCRLFStringLen(HMLETextThunk *this)
{
	if (this==NULL) return -1;
	return strlen(this->str)+countlf(this->str);
}

UCHAR* HMLETextThunkGetLFString(HMLETextThunk *this,UCHAR *buf)
{
	if (this==NULL) return NULL;
	if (buf==NULL) return NULL;
	strcpy(buf,this->str);
	return buf;
}

UCHAR* HMLETextThunkGetCRLFString(HMLETextThunk *this,UCHAR *buf)
{
	if (this==NULL) return NULL;
	if (buf==NULL) return NULL;
	lf2crlf(buf,this->str);
	return buf;
}

void HMLETextThunkRewind(HMLETextThunk *this)
{
	if (this==NULL) return;

	this->curLine = this->str;
}

UCHAR* HMLETextThunkNextLine(HMLETextThunk *this)
{
	if (this==NULL) return NULL;

	while (*(this->curLine))
		{
		if (*(this->curLine)=='\n') return (++(this->curLine));
		this->curLine++;
		}
	return this->curLine;
}

int HMLETextThunkQueryCurLineLen(HMLETextThunk *this)
{
	if (this==NULL) return -1;

	return (PSLFstrlen(this->curLine));
}

UCHAR* HMLETextThunkQueryCurLine(HMLETextThunk *this)
{
	if (this==NULL) return NULL;
	
	return (this->curLine);
}

UCHAR* HMLETextThunkGetCurLine(HMLETextThunk *this,UCHAR *pchBuf)
{
	if (this==NULL) return NULL;
	
	return PSLFstrcpy(pchBuf,this->curLine);
}
