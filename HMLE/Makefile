.SUFFIXES:	.c .o .obj .rc .res

CC = gcc
CFLAGS = -Wall -Wno-format -Wno-parentheses -O -DDEBUG -g
CFLAGS2 = -Zomf -Zsys -Wall -Wno-format -Wno-parentheses -O3 -s

CP = cp
RM = rm

.c.o:
	$(CC) $*.c $(CFLAGS) -c -o $@
.c.obj:
	$(CC) $*.c $(CFLAGS2) -c -o $@
.rc.res:
	rc -r $*.rc

HAN_DIR = ../hanlib
HAN_A = $(HAN_DIR)/han.a
HAN_LIB = $(HAN_DIR)/han.lib
HAN_H =\
	$(HAN_DIR)/han.h\
	$(HAN_DIR)/hch.h\
	$(HAN_DIR)/hch/hch.h\
	$(HAN_DIR)/hout.h\
	$(HAN_DIR)/hin.h

HCHLB_DIR = ../hchlb
HCHLB_H =\
	$(HCHLB_DIR)/hchlb.h\
	$(HCHLB_DIR)/hjseldlg.h
HCHLBDLG_RES = $(HCHLB_DIR)/hchlbdlg.res

ETC_DIR = ../etclib
ETC_H = \
	$(ETC_DIR)/textutil.h\
	$(ETC_DIR)/debugutil.h
ETC_O = $(ETC_H:.h=.o)
ETC_OBJ = $(ETC_H:.h=.obj)

#hmle
HMLE_O = \
	HMLEDoc.o \
	HMLELine.o \
	HMLEIpt.o \
	HMLETextThunk.o \
	HMLEClient.o \
	HMLE.o
HMLE_OBJ = $(HMLE_O:.o=.obj)

#working
debug: HMLE.a HMLE.exe
release: HMLE.lib
all: HMLE.a HMLE.lib

HMLE.exe:	HMLETest.o HMLE.a $(HCHLBDLG_RES)
	$(CC) $(CFLAGS) -o HMLE.exe HMLETest.o HMLE.a $(HAN_A) $(ETC_O) $(HCHLBDLG_RES)

HMLE.a: $(HMLE_O)
	ar cvr HMLE.a $?
HMLE.lib: $(HMLE_OBJ)
	emxomfar cvr HMLE.lib $?

HMLETest.o: 	HMLE.h $(HAN_H) $(ETC_H)

$(HCHLBDLG_RES) :
	$(MAKE) -C $(HCHLB_DIR) hchlbdlg.res

HMLETextThunk.o:	HMLE_internal.h $(HAN_H) $(ETC_H) HMLETextThunk.h
HMLELine.o: 	HMLE_internal.h $(HAN_H) $(ETC_H) HMLELine.h
HMLEDoc.o:		HMLE_internal.h $(HAN_H) $(ETC_H) HMLEDoc.h
HMLEIpt.o:		HMLE_internal.h $(HAN_H) $(ETC_H) HMLEIpt.h
HMLEClient.o:		HMLE_internal.h $(HAN_H) $(HCHLB_H) $(ETC_H) HMLE.h
HMLE.o: 		HMLE_internal.h $(HAN_H) $(HCHLB_H) $(ETC_H) HMLE.h

HMLETextThunk.obj:	HMLE_internal.h $(HAN_H) $(ETC_H) HMLETextThunk.h
HMLELine.obj:		HMLE_internal.h $(HAN_H) $(ETC_H) HMLELine.h
HMLEDoc.obj:		HMLE_internal.h $(HAN_H) $(ETC_H) HMLEDoc.h
HMLEIpt.obj:		HMLE_internal.h $(HAN_H) $(ETC_H) HMLEIpt.h
HMLEClient.obj: 	HMLE_internal.h $(HAN_H) $(HCHLB_H) $(ETC_H) HMLE.h
HMLE.obj:		HMLE_internal.h $(HAN_H) $(HCHLB_H) $(ETC_H) HMLE.h

touch:
	touch *.c *.h

clean:
	-$(RM) *.o *.obj *.a *.lib *.exe *.bak core *~*
