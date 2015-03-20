#Makefile for VK UI program

CROSS_COMPILE = ~/marsboard/fsl-linaro-toolchain/bin/arm-fsl-linux-gnueabi-
#CROSS_COMPILE = 
GCC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld

SRC_DIR = ./
OBJ_DIR = ./objects
INC_DIR = ./include
EXEC_DIR = ./build

INCLUDES = -I./
INCLUDES += -I./include
#INCLUDES += -I/usr/include
#INCLUDES += -I/usr/local/include
#INCLUDES +=

CFLAGS =
#CFLAGS +=

#LIBPATH = -L /lib
#LIBPATH += -L /usr/lib
#LIBPATH += -L /usr/local/lib

LIBS = -lpthread
#LIBS += 

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY : all clean

all : vk_app
vk_app:$(OBJS)
#	@if [ ! -d ./build ]; then\
		mkdir ./build;\
	fi
	$(GCC) -o $(EXEC_DIR)/$@ $(CFLAGS) $(INCLUDES) $(LIBPATH) $^ $(LIBS)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
#	@if [ ! -d ./objects ]; then\
		mkdir ./objects;\
	fi
	$(GCC) -o $@ -c $(CFLAGS) $(INCLUDES) $(LIBS) $<

clean :
	-rm $(EXEC_DIR)/vk_app $(OBJS)


