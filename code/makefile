# Makefile for patriciasql (single-source C++ program)
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
LDFLAGS := -lncurses -lmenu

# Get MySQL CFLAGS and LIBS from mysql_config
MYSQLCFLAGS = $(shell mariadb_config --cflags)
MYSQLLIBS = $(shell mariadb_config --libs)

SRC := main.c
OBJ := $(SRC:.c=.o)
TARGET := patriciasql

.PHONY: all clean debug run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(MYSQLLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLCFLAGS) -c $< -o $@

debug: CXXFLAGS += -g -O0
debug: clean all

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
