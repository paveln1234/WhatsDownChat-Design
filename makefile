#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I ./inc -I ./config -I ../../advc/inc
CFLAGS = -Wall
RESINC = 
LIBDIR = 
LIB = ../../advc/libds.a
LDFLAGS = 


INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/TcpProject

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/TcpProject

OBJ_DEBUG = $(OBJDIR_DEBUG)/src/Tlv.o $(OBJDIR_DEBUG)/src/ServerLogic.o $(OBJDIR_DEBUG)/src/Server.o $(OBJDIR_DEBUG)/src/Protocol.o $(OBJDIR_DEBUG)/src/Net.o $(OBJDIR_DEBUG)/src/IpDb.o $(OBJDIR_DEBUG)/src/Db.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/config/Config.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/src/Tlv.o $(OBJDIR_RELEASE)/src/ServerLogic.o $(OBJDIR_RELEASE)/src/Server.o $(OBJDIR_RELEASE)/src/Protocol.o $(OBJDIR_RELEASE)/src/Net.o $(OBJDIR_RELEASE)/src/IpDb.o $(OBJDIR_RELEASE)/src/Db.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/config/Config.o

all: frontEnd debug release

frontEnd: 
	$(MAKE) all -C client/client

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	test -d $(OBJDIR_DEBUG)/config || mkdir -p $(OBJDIR_DEBUG)/config

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/src/Tlv.o: src/Tlv.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Tlv.c -o $(OBJDIR_DEBUG)/src/Tlv.o

$(OBJDIR_DEBUG)/src/ServerLogic.o: src/ServerLogic.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/ServerLogic.c -o $(OBJDIR_DEBUG)/src/ServerLogic.o

$(OBJDIR_DEBUG)/src/Server.o: src/Server.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Server.c -o $(OBJDIR_DEBUG)/src/Server.o

$(OBJDIR_DEBUG)/src/Protocol.o: src/Protocol.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Protocol.c -o $(OBJDIR_DEBUG)/src/Protocol.o

$(OBJDIR_DEBUG)/src/Net.o: src/Net.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Net.c -o $(OBJDIR_DEBUG)/src/Net.o

$(OBJDIR_DEBUG)/src/IpDb.o: src/IpDb.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/IpDb.c -o $(OBJDIR_DEBUG)/src/IpDb.o

$(OBJDIR_DEBUG)/src/Db.o: src/Db.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Db.c -o $(OBJDIR_DEBUG)/src/Db.o

$(OBJDIR_DEBUG)/main.o: main.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.c -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/config/Config.o: config/Config.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c config/Config.c -o $(OBJDIR_DEBUG)/config/Config.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)/src
	rm -rf $(OBJDIR_DEBUG)
	rm -rf $(OBJDIR_DEBUG)/config

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	test -d $(OBJDIR_RELEASE)/config || mkdir -p $(OBJDIR_RELEASE)/config

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/src/Tlv.o: src/Tlv.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Tlv.c -o $(OBJDIR_RELEASE)/src/Tlv.o

$(OBJDIR_RELEASE)/src/ServerLogic.o: src/ServerLogic.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/ServerLogic.c -o $(OBJDIR_RELEASE)/src/ServerLogic.o

$(OBJDIR_RELEASE)/src/Server.o: src/Server.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Server.c -o $(OBJDIR_RELEASE)/src/Server.o

$(OBJDIR_RELEASE)/src/Protocol.o: src/Protocol.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Protocol.c -o $(OBJDIR_RELEASE)/src/Protocol.o

$(OBJDIR_RELEASE)/src/Net.o: src/Net.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Net.c -o $(OBJDIR_RELEASE)/src/Net.o

$(OBJDIR_RELEASE)/src/IpDb.o: src/IpDb.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/IpDb.c -o $(OBJDIR_RELEASE)/src/IpDb.o

$(OBJDIR_RELEASE)/src/Db.o: src/Db.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Db.c -o $(OBJDIR_RELEASE)/src/Db.o

$(OBJDIR_RELEASE)/main.o: main.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.c -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/config/Config.o: config/Config.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c config/Config.c -o $(OBJDIR_RELEASE)/config/Config.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)/src
	rm -rf $(OBJDIR_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/config
	$(MAKE) clean -C client/client

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

