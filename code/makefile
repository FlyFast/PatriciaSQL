# Makefile for patriciasql (single-source C++ program)
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
LDFLAGS := -lncurses

SRC := main.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := patriciasql

.PHONY: all clean debug run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += -g -O0
debug: clean all

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

