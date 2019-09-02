#include "textutil.h"

int countlf(char* str)
{
char *pch = str;
int count = 0;
char prev = 0;

	while (*pch)
		{
		if ((*pch=='\n') && (prev!='\r')) count++;
		prev = *(pch++);
		}
	return count;
}

int countcrlf(char* str)
{
char *pch = str;
int count = 0;

	while (*pch)
		if (*(pch++) == '\r')
		if (*(pch++) == '\n') count++;
	return count;
}

int lf2crlf(char *dst,char *org)
{
int count = 0;
char prev = 0;

	while (*org != 0)
		{
		if ((*org == '\n') && (prev != '\r'))
			{
			*(dst++) = '\r';
			*(dst++) = '\n';
			prev = *(org++);
			count ++;
			} else {
			prev = *(dst++) = *(org++);
			}
		} *dst = 0;
	return count;
}

int crlf2lf(char *dst,char *org)
{
int count = 0;

	while (*org)
		{
		if (*org == '\r')
			{
			org++;
			switch (*org)
			{
			case '\n':
				{
				*(dst++) = '\n';
				count++;
				} break;
			case '\r':
				{
				org++;
				if (*org=='\n')
					{
					*(dst++) = '\n';
					} else {
					*(dst++) = '\r';
					*(dst++) = '\r';
					*(dst++) = *org;
					}
				} break;
			default:
				{
				*(dst++) = '\r';
				*(dst++) = *org;
				} break;
			}
			org++;
			} else {
			*(dst++) = *(org++);
			}
		} *dst = 0;
	return count;
}

int crlf2lf_old(char *dst,char *org)
{
int count = 0;

	while (*org)
		{
		if (*org == '\r')
			{
			org++;
			if (*org == '\n')
				{
				*(dst++) = '\n';
				count++;
				} else {
				*(dst++) = '\r';
				*(dst++) = *org;
				}
			org++;
			} else {
			*(dst++) = *(org++);
			}
		} *dst = 0;
	return count;
}

int PSLFstrlen(PSLF str) // \n,\0 = terminal characters.
{
int count = 0;
	while ((*str)&&(*str!='\n'))
		{
		count++;
		str++;
		}
	return count;
}

int isPSLF(char* str)
{
	if (str[PSLFstrlen(str)]=='\n')
		return 1;
	return 0;
}

PSLF PSLFstrcpy(PSLF dst,PSLF src)
{
char *pch = dst;
	while ((*src)&&(*src!='\n'))
		*(pch++)=*(src++);
	*pch='\n';
	return dst;
}

PSLF PSLFstrncpy(PSLF dst,PSLF src,int count)
{
PSLF pch = dst;
	while ((*src)&&(*src!='\n')&&(pch<dst+count))
		*(pch++)=*(src++);
	*pch='\n';
	return dst;
}

PSZ PSLFtoPSZ(PSLF strLF)
{
int len;
	len = PSLFstrlen(strLF);
	strLF[len] = 0;
	return strLF;
}

PSLF PSZtoPSLF(PSZ strZ)
{
	*(strZ+strlen(strZ))='\n';
	return strZ;
}

PSLF copyNextPSLF(PSLF dst,PSZ src)
{
	while ((*src) && (*src!='\n'))
		*(dst++) = *(src++);
	if (*src)
		{
		*dst='\n';
		return src+1;
		} else {
		*dst='\0';
		return src;
		}
}


