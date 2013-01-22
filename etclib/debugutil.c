#include "debugutil.h"

void dumpbuf(FILE *stream,char *buf)
{
int idx;

	assert(stream!=NULL);

	if (buf==NULL)
		{
		fprintf(stream,"NULL\n");
		return;
		}

	idx = 0;
	do	{
		if (idx%8==0)
			fprintf(stream,"\n%8x:",idx+(int)buf);
		fprintf(stream,"%2X ",buf[idx]);
		} while (buf[idx++]);
	fprintf(stream,"\n");
}

void dump(FILE *stream,const char* name,int size,void *buf)
{
int idx = 0;

	assert(stream!=NULL);
	assert(name!=NULL);
	assert(size>0);
	assert(buf!=NULL);
	
	fprintf(stream,"\n[%20s] %8x - %8x",name,buf,buf+size-1);
	for (idx=0;idx<size;idx++)
		{
		if (idx%8==0)
			fprintf(stream,"\n%8x:",idx+(int)buf);
		fprintf(stream,"%3X",(((unsigned char*)buf)[idx]));
		}
	fprintf(stream,"\n");
}

void dump32(FILE *stream,const char* name,int size,void *buf)
{
int idx = 0;

	assert(stream!=NULL);
	assert(name!=NULL);
	assert(size>0);
	assert(buf!=NULL);
	
	fprintf(stream,"\n[%20s] %8x - %8x",name,buf,buf+size-1);
	for (idx=0;idx<size;idx+=4)
		{
		if (idx%16==0)
			fprintf(stream,"\n%8x:",idx+(int)buf);
		fprintf(stream,"%9X",(((unsigned long*)buf)[idx/4]));
		}
	fprintf(stream,"\n");
}


