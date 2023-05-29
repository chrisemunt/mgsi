# UNIX makefile for MGateway Service Integration Gateway (mgsi)
#
# Build:
# make Makefile
#
# Cleanup:
# make clean
#
# Install:
# make install

MGSIINC=./

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
   MGSIOS = Linux
   CC = cc
   CCFLAGS += -DLINUX
   LINK += cc
   LINKFLAGS += -pthread
   LIBS += -lrt -ldl -lm
endif
ifeq ($(UNAME_S),Darwin)
   MGSIOS = OSX
   CC = cc
   CCFLAGS += -DMACOSX -D_NOTHREADS -DDARWIN
   LINK += cc
   LINKFLAGS +=
endif
ifeq ($(UNAME_S),FreeBSD)
   MGSIOS = FreeBSD
   CC = cc
   CCFLAGS += -DFREEBSD
   LINK = ld
   LINKFLAGS +=
endif
ifeq ($(UNAME_S),AIX)
   MGSIOS = AIX
   CC = xlc_r
   CCFLAGS += -DAIX
   LINK = xlc_r
   LINKFLAGS +=
endif

ifndef MGSIOS
   $(error Cannot determine the OS type)
endif

# Build mg_dba module
all : mgsi

# Link the object file and dependent libraries into a binary
mgsi : mgsiutl.o mgsicfg.o mgsidso.o mgsireq.o mgsidba.o mgsitls.o mgsiadm.o mgsi.o
	$(LINK) -o mgsi $(LINKFLAGS) mgsiutl.o mgsicfg.o mgsidso.o mgsireq.o mgsidba.o mgsitls.o mgsiadm.o mgsi.o $(LIBS)

# Compile the source file into object file
mgsi.o : mgsi.c
	$(CC) -o mgsi.o $(CCFLAGS) -I$(MGSIINC) -c mgsi.c

mgsiadm.o : mgsiadm.c
	$(CC) -o mgsiadm.o $(CCFLAGS) -I$(MGSIINC) -c mgsiadm.c

mgsiutl.o : mgsiutl.c
	$(CC) -o mgsiutl.o $(CCFLAGS) -I$(MGSIINC) -c mgsiutl.c

mgsidso.o : mgsidso.c
	$(CC) -o mgsidso.o $(CCFLAGS) -I$(MGSIINC) -c mgsidso.c

mgsireq.o : mgsireq.c
	$(CC) -o mgsireq.o $(CCFLAGS) -I$(MGSIINC) -c mgsireq.c

mgsidba.o : mgsidba.c
	$(CC) -o mgsidba.o $(CCFLAGS) -I$(MGSIINC) -c mgsidba.c

mgsitls.o : mgsitls.c
	$(CC) -o mgsitls.o $(CCFLAGS) -I$(MGSIINC) -c mgsitls.c

# Install mgsi module
install :
	$(error Copy mgsi to a location of your choice)

# Clean target
clean :
	rm -rf mgsi mgsi.o mgsiadm.o mgsiutl.o mgsidso.o mgsireq.o mgsidba.o mgsitls.o


