.SUFFIXES:	.o .c .obj
CC = gcc
CFLAGS = -Wall -Wno-format -Wno-parentheses -O -DDEBUG -g
CFLAGS2 = -Zomf -Zsys -Wall -Wno-format -Wno-parentheses -O3 -s
RM = rm
CP = cp

.c.o:
	$(CC) $*.c $(CFLAGS) -c -o $@
.c.obj:
	$(CC) $*.c $(CFLAGS2) -c -o $@

ALL_O = textutil.o debugutil.o
ALL_OBJ = $(ALL_O:.o=.obj)

#targets:
debug: $(ALL_O)
release: $(ALL_OBJ)
all:	$(ALL_O) $(ALL_OBJ)
debug_hanedit: textutil.o debugutil.o
release_hanedit: textutil.obj debugutil.obj

# O
textutil.o: textutil.h
textutil.obj: textutil.h

debugutil.o: debugutil.h
debugutil.obj: debugutil.h

new:
	touch *.c *.h
	$(MAKE) all

#clear

clean:
	-$(RM) *.o *.obj *.bak core *~
