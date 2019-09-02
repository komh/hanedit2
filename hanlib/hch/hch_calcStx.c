#include "hch.h"

int hch_decStx(unsigned char *str,int *pstx)
{
	if (*pstx == 0)
		return 0L;
	if (*pstx == 1)
		{
		(*pstx) --;
		return 1L;
		}
	if (hch_queryHchType(str,(*pstx)-2) == HCH_HAN_LEAD)
		{
		(*pstx) -= 2;
		return 2L;
		} else {
		(*pstx) --;
		return 1L;
		}
}

int hch_incStx(unsigned char *str,int *pstx)
{
	if (*pstx >= strlen(str))
		return 0L;
	if (hch_queryHchType(str,*pstx) == HCH_HAN)
		{
		(*pstx) += 2;
		return 2L;
		} else {
		(*pstx) ++;
		return 1L;
		}
}

int hch_alignStx(unsigned char *str,int *pstx)
{
	if (hch_queryHchType(str,*pstx) == HCH_HAN_TAIL)
		{
		(*pstx)--;
		return 1;
		}
	return 0;
}
