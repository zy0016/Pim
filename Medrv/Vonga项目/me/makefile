include ../mkinc/config.mk

# macro definitions for tools & dir

#
# Makefile for the 166/rt166t library.
#
SRC		= ../../

# Tools
AS      = $(TASKING_PATH)/bin/a166
ASM     = $(TASKING_PATH)/bin/a166
CC      = $(TASKING_PATH)/bin/c166
AR      = $(TASKING_PATH)/bin/ar166
MM      = $(TASKING_PATH)/bin/m166
GSO166  = $(TASKING_PATH)/bin/gso166
MK      = $(TASKING_PATH)/bin/mk166
C166INC = $(TASKING_PATH)/include

MACRO := _C166_ARCH=1

TARGET 		:= me.lib

ifeq ($(CFG),Release)
CFLAGS = -AF -Be -xmfp -e -s -Ml -znocustack -OaBcdEFghjkLnopQrStUwX -T0 -w68 -w130 -w150 -w200 -w196  -BZc166sv1sp -BZc166sv1cp -BZc166sv1sp2 -BZcpu_reti_int -BZc166sv1ext -BK -BZno_cpu_jmpra_cache -BM -BZc166sv1div -DPRJ_UW_LOG -mPR=0,4000
SRCASMFLAGS = NOPAGING NOMOD166 CASE EXTINSTR EXTMEM SEGMENTED MODEL(LARGE)
LOCFLAGS =	CA NOVI NOVT 'NOWA(152)' 'NOWA(176)' 'NOWA(177)' PUBTOGLB NOPA NOSORTALIGN CHECKCLASSES
else
CFLAGS = -g -AF -Be -xmfp -e -s -Ml -znocustack -OaBcdEFghjkLnopQrStUwX -T0 -w68 -w130 -w150 -w200 -w196 -D_DEBUG -BZc166sv1sp -BZc166sv1cp -BZc166sv1sp2 -BZcpu_reti_int -BZc166sv1ext -BK -BZno_cpu_jmpra_cache -BM -BZc166sv1div -DPRJ_UW_LOG -mPR=0,4000
SRCASMFLAGS =	DEBUG NOPAGING NOMOD166 CASE EXTINSTR EXTMEM SEGMENTED MODEL(LARGE)
LOCFLAGS = CA DEBUG NOVI NOVT 'NOWA(152)' 'NOWA(176)' 'NOWA(177)' PUBTOGLB NOPA NOSORTALIGN CHECKCLASSES LISTSYMBOLS MAP
endif

CLINTFLAGS =	-d__STDC__=1 -e537 +fdi ./std_m.lnt 
SRECFLAGS =		-r2

RM-F := rm -f

CFLAGS_DEBUG = $(CFLAGS) $(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO)) -D_DEBUG
CFLAGS_RELEASE = $(CFLAGS) $(addprefix -I, $(INCPATH)) $(addprefix -D, $(MACRO))

#Hopen SDK dirs
SDK_INCLUDE = $(SDKPATH)/include

SRC_PATH = .
INCLUDE_PATH = $(SRC_PATH)/include 
INCPATH = $(INCLUDE_PATH) $(SRC_PATH) $(SDK_INCLUDE) 

OBJ_ME		= me_wnd.o me_func.o me_comn.o me_init.o me_at.o smspdu.o

OBJS        = $(OBJ_ME)
DIR_ME  = .



all : $(TARGET)

$(OBJ_ME) : %.o : $(DIR_ME)/%.c
	$(CC) -c $(CFLAGS_DEBUG) -I$(APPINC) -I$(DIR_ME) -o $(patsubst %.o, %.src, $@) $< 
	$(ASM) $(patsubst %.o, %.src, $@) '$(SRCASMFLAGS)' TO $@

.PHONY : force clean

clean :
	$(RM-F) *.o
	$(RM-F) *.d
	$(RM-F) *.lst
	$(RM-F) *.src
	$(RM-F) $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) crv $@ $(OBJS)
	cp $(TARGET)  $(TARGETLIBDIR)
