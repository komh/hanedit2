#include "hch.h"

/* HANCHAR hch_sy2ks_hanja(HANCHAR hch)
{
unsigned char lo = (unsigned char)(hch&0xFF);
unsigned char hi = (unsigned char)(hch>>8);

	if (lo<0x7F)
		return HCHFROM2CH(0xE0+((hi-0xE0)<<1)-0x16,lo+0x70);
	if (lo>0xA0)
		return HCHFROM2CH(0xE0+((hi-0xE0)<<1)-0x15,lo);
	return HCHFROM2CH(0xE0+((hi-0xE0)<<1)-0x16,lo+0x5E);
} */

HANCHAR hch_sy2ks_hanja(HANCHAR hch)
{
unsigned char lo = (unsigned char)(hch&0xFF);
unsigned char hi = (unsigned char)(hch>>8);
HANCHAR ret;

	if (lo<0x7F)
		ret = HCHFROM2CH(0xE0+((hi-0xE0)<<1)-0x16,lo+0x70);
	else if (lo>0xA0)
		ret = HCHFROM2CH(0xE0+((hi-0xE0)<<1)-0x15,lo);
	else ret = HCHFROM2CH(0xE0+((hi-0xE0)<<1)-0x16,lo+0x5E);

	lo = (ret&0xFF);
	hi = (ret>>8);

	if (lo<0xA1) return 0xCAA1;
	if (lo>0xFE) return 0xCAA1;
	if (hi<0xCA) return 0xCAA1;
	if (hi>0xFD) return 0xCAA1;
	return ret;
}
HANCHAR hch_ks2sy_hanja(HANCHAR hch)
{
unsigned char lo = (unsigned char)(hch&0xFF);
unsigned char hi = (unsigned char)(hch>>8);

	if (hi&0x01) // CB CD ...
		return HCHFROM2CH(0xE0+((hi+0x15-0xE0)>>1),lo);
	if (lo<0xEF)
		return HCHFROM2CH(0xE0+((hi+0x16-0xE0)>>1),lo-0x70);
	return HCHFROM2CH(0xE0+((hi+0x16-0xE0)>>1),lo-0x5E);
}

