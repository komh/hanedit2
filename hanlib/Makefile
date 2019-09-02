.SUFFIXES : .c .o .obj
CC = gcc
CFLAGS = -O3 -Wall -Wno-parentheses -Wno-format -DDEBUG -g
CFLAGS2 = -Zomf -Zsys -s -O3 -Wall -Wno-parentheses -Wno-format

ifdef KIME
CFLAGS := $(CFLAGS) -D__KIME__
CFLAGS2 := $(CFLAGS2) -D__KIME__
endif

RM = rm
CP = cp

.c.o:
	$(CC) $*.c $(CFLAGS) -c -o $@
.c.obj:
	$(CC) $*.c $(CFLAGS2) -c -o $@

HIN_C = hin.c ha2.c ha3.c hinbuf.c
HIN_O = $(HIN_C:.c=.o)
HIN_OBJ = $(HIN_C:.c=.obj)
HOUT_C = hout.c
HOUT_O = $(HOUT_C:.c=.o)
HOUT_OBJ = $(HOUT_C:.c=.obj)
HCH_C = \
	hch/hch_calcStx.c\
	hch/hch_deleteHch.c\
	hch/hch_getNextHch.c\
	hch/hch_insertHch.c\
	hch/hch_queryHchType.c\
	hch/hch_setHchToPch.c\
	hch/hch_strlwr.c\
	hch/hch_syks.c\
	hch/hch_syks_hanja.c\
	hch/hch_hanja.c

HCH_O = $(HCH_C:.c=.o)
HCH_OBJ = $(HCH_C:.c=.obj)
HEADERS = han.h hch.h hch/hch.h hin.h hin_internal.h hout.h font.h

HCHLB_H =\
	../hchlb/hchlb.h\
	../hchlb/scseldlg.h\
	../hchlb/hjseldlg.h

HCHLB_O = \
	../hchlb/hchlb.o\
	../hchlb/hchlbClient.o\
	../hchlb/hchcol.o\
	../hchlb/hjseldlg.o\
	../hchlb/scseldlg.o

HCHLB_OBJ = $(HCHLB_O:.o=.obj)

#targets
debug: han.a
release: han.lib
all: han.a han.lib

#hanlib
han.a:	$(HCH_O) $(HIN_O) $(HOUT_O) $(HCHLB_O)
	ar cvr han.a $?
han.lib: $(HCH_OBJ) $(HIN_OBJ) $(HOUT_OBJ) $(HCHLB_OBJ)
	emxomfar cvr han.lib $?

#hin
hin.o:	hch.h hin.h hin_internal.h $(HCHLB_H)
ha2.o:	hch.h hin.h hin_internal.h
ha3.o:	hch.h hin.h hin_internal.h

#hout
hout.o: hch.h hout.h

#hch
hch/hch_calcStx.o:	hch.h
hch/hch_deleteHch.o:	hch.h
hch/hch_getNextHch.o:	hch.h
hch/hch_insertHch.o:	hch.h
hch/hch_queryHchType.o: hch.h
hch/hch_setHchToPch.o:	hch.h
hch/hch_strlwr.o:	hch.h
hch/hch_syks.o: 	hch.h
hch/hch_syks_hanja.o:	hch.h
hch/hch_hanja.o: hch.h



touch:
	touch *.c *.h
	touch $(HCH_C) hch/hch.h

clean:
	-$(RM) *.o *.obj *.a *.lib core *.bak *~
	cd hch
	-$(RM) $(HCH_O) $(HCH_OBJ) hch/*.bak


