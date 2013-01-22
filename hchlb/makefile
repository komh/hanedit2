.SUFFIXES:	.c .o .obj .rc .res .def
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
HAN_H =\
	$(HAN_DIR)/han.h\
	$(HAN_DIR)/hch.h\
	$(HAN_DIR)/hch/hch.h\
	$(HAN_DIR)/hout.h\
	$(HAN_DIR)/hin.h
HAN_A = $(HAN_DIR)/han.a
HAN_LIB = $(HAN_DIR)/han.lib

HCHLB_O = \
	hchlb.o\
	hchlbClient.o\
	hchcol.o\
	hjseldlg.o\
	scseldlg.o

HCHLB_OBJ = $(HCHLB_O:.o=.obj)

#working
debug: $(HAN_A) hchlbtest.exe
release: $(HAN_LIB)
all: debug release

hchlbtest.exe:	  hchlbtest.o $(HAN_A) hchlbdlg.res
	-$(CC) $(CFLAGS) hchlbtest.o $(HAN_A) hchlbdlg.res

$(HAN_A) : $(HCHLB_O)
	ar cvr $(HAN_A) $?

$(HAN_LIB): $(HCHLB_OBJ)
	emxomfar cvr $(HAN_LIB) $?

hchlbtest.o   : $(HAN_H) hchlb.h hjseldlg.h hchlbdlg.h
hchlbcol.o	  : hchcol.h
hchlb.o 	  : $(HAN_H) hchlb.h hchlb_internal.h hchcol.h
hchlbClient.o : $(HAN_H) hchlb.h hchlb_internal.h hchcol.h
hjseldlg.o	  : $(HAN_H) hjseldlg.h hchlbdlg.h
scseldlg.o	  : $(HAN_H) scseldlg.h hchlbdlg.h

hchlbcol.obj	: hchcol.h
hchlb.obj		: $(HAN_H) hchlb.h hchlb_internal.h hchcol.h
hchlbClient.obj : $(HAN_H) hchlb.h hchlb_internal.h hchcol.h
hjseldlg.obj	: $(HAN_H) hjseldlg.h hchlbdlg.h
scseldlg.obj	: $(HAN_H) scseldlg.h hchlbdlg.h

hchlbdlg.res : hchlbdlg.rc hanjasel.dlg scsel.dlg hchlbdlg.h

touch:
	touch *.c *.h

clean:
	-$(RM) *.o *.obj *.a *.lib *.res *.exe *.bak *.rwp core *~*
