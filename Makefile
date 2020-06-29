#DEBUG	= -g -O0
DEBUG	= -O3
CC	= g++

INCLUDE	= -I/usr/local/include
CPPFLAGS	= $(DEBUG) -std=c++11 -std=c++1y -Wall -Winline -pipe $(INCLUDE)

LDFLAGS	= -L/usr/local/lib
LDLIBS  = -lstdc++fs -lpthread -lm

SRC_DIR   := src

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,%.o,$(SRC))

BINS	=	$(SRC:.cpp=)

vpath %.cpp $(SRC_DIR)

ifndef PREFIX
PREFIX = /usr/local
endif

all: create-index-html

%.o: src/%.cpp
	@$(CC) -c $(CPPFLAGS) $< -o $@

create-index-html: $(OBJ)
	@$(CC) -o $@ main.o $(LDFLAGS) $(LDLIBS)


clean:
	@rm -f $(OBJ) create-index-html *~ core tags $(BINS)

.PHONY: all clean
