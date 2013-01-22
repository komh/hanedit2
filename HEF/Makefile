.SUFFIXES:	.c .o .obj .rc .res
CC = gcc
CFLAGS = -Wall -Wno-format -Wno-parentheses -O -DDEBUG -g
CFLAGS2 = -Zomf -Zsys -Wall -Wno-format -Wno-parentheses -O3 -s

CP = cp
RM = rm
RC = rc

.c.o:
	$(CC) $*.c $(CFLAGS) -c -o $@
.c.obj:
	$(CC) $*.c $(CFLAGS2) -c -o $@
.rc.res:
	rc -r $*.rc

HAN_DIR = ../hanlib
HAN_H =\
	$(HAN_DIR)/han.h\
	$(HAN_DIR)/hch.h\
	$(HAN_DIR)/hch/hch.h\
	$(HAN_DIR)/hout.h\
	$(HAN_DIR)/hin.h
HAN_A = $(HAN_DIR)/han.a
HAN_LIB = $(HAN_DIR)/han.lib

ETC_DIR = ../etclib
ETC_H = \
	$(ETC_DIR)/textutil.h\
	$(ETC_DIR)/debugutil.h
ETC_O = $(ETC_H:.h=.o)
ETC_OBJ = $(ETC_H:.h=.obj)

HCHLB_DIR = ../hchlb
HCHLB_H =\
	$(HCHLB_DIR)/hchlbdlg.h\
	$(HCHLB_DIR)/hchlb.h\
	$(HCHLB_DIR)/hjseldlg.h\
	$(HCHLB_DIR)/scseldlg.h
HCHLB_RC = \
	$(HCHLB_DIR)/hchlbdlg.h\
	$(HCHLB_DIR)/hchlbdlg.rc\
	$(HCHLB_DIR)/hanjasel.dlg\
	$(HCHLB_DIR)/scsel.dlg
HCHLBDLG_RES = $(HCHLB_DIR)/hchlbdlg.res

HEF_O = \
	HEF.o\
	HEFClient.o
HEF_OBJ = $(HEF_O:.o=.obj)

#working
debug: HEF.a HEFTest.exe
release: HEF.lib
all: debug release

HEF.a:	$(HEF_O)
	ar cvr HEF.a $?
HEF.lib: $(HEF_OBJ)
	emxomfar cvr HEF.lib $?
HEFTest.exe:	HEF.a HEFTest.o $(ETC_O) $(HAN_A) $(HCHLBDLG_RES)
	-$(CC) $(CFLAGS) HEFTest.o HEF.a $(ETC_O) $(HAN_A) $(HCHLBDLG_RES)


HEF.o:			HEF.h HEF_internal.h $(ETC_H) $(HAN_H) $(HCHLB_H)
HEFClient.o:	HEF.h HEF_internal.h $(ETC_H) $(HAN_H) $(HCHLB_H)
HEFTest.o:		HEF.h $(HAN_H) $(ETC_H)

HEF.obj:		HEF.h HEF_internal.h $(ETC_H) $(HAN_H)
HEFClient.obj:	HEF.h HEF_internal.h $(ETC_H) $(HAN_H)

$(HCHLBDLG_RES) :
	$(MAKE) -C $(HCHLB_DIR) hchlbdlg.res

touch:
	touch *.c *.h

clean:
	-$(RM) *.o *.obj *.a *.lib *.res *.exe *.bak core *~*
