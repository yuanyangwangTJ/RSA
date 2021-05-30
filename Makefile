# Makefile

# SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
# OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

# TARGET	:= main

# compile and lib parameter
# CC      := g++
# CXXFLAGS:= -g -O2 -std=c++11 -pthread -march=native
# LIBS    := -lntl -lgmp -lm
# LDFLAGS := 

# $(TARGET): $(OBJS)
# 	$(CC) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS) $(LIBS)

# .PHONY:
# clean:
# 	@rm -f $(TARGET)

# run:
# 	@./$(TARGET)

output: main.o TDES.o
	g++ main.o TDES.o -o output

main.o: main.cpp
	g++ -c main.cpp

TDES.o: TDES.cpp TDES.h
	g++ -c TDES.cpp
clean:
	rm -f *.o output