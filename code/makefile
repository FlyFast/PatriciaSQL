# Makefile for patriciasql (single-source C++ program)
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
LDFLAGS := -lncurses

# Get MySQL CFLAGS and LIBS from mysql_config
MYSQLCFLAGS = $(shell mysql_config --cflags)
MYSQLLIBS = $(shell mysql_config --libs)

SRC := main.cpp
OBJ := $(SRC:.cpp=.o)
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
