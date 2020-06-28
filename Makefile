#DEBUG	= -g -O0
DEBUG	= -O3
CC	= g++

LIBXML_ROOT ?= /usr/local/xml

INCLUDE	= -I/usr/local/include
CPPFLAGS	= $(DEBUG) -std=c++11 -std=c++1y -Wall -Winline -pipe $(INCLUDE) -I$(LIBXML_ROOT)/include/libxml2

LDFLAGS	= -L/usr/local/lib
LDLIBS  = -L$(LIBXML_ROOT)/lib -lxml2 -lstdc++fs -lpthread -lm

SRC_DIR   := src

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,%.o,$(SRC))

BINS	=	$(SRC:.cpp=)

vpath %.cpp $(SRC_DIR)

ifndef PREFIX
PREFIX = /usr/local
endif

all: create-index-html

create-index-html:
	@$(CC) -o $@ main.o $(LDFLAGS) $(LDLIBS)

.cpp.o:
	@$(CC) -c $(CPPFLAGS) $< -o $@

clean:
	@rm -f $(OBJ) create-index-html *~ core tags $(BINS)

.PHONY: all clean
