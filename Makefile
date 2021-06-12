# Makefile

# SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
# OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

# TARGET	:= main

# compile and lib parameter
# CC      := g++
CXXFLAGS:= -O3 -std=c++11 -pthread -march=native
LIBS    := -lntl -lgmp -lm
# LDFLAGS := 

# $(TARGET): $(OBJS)
# 	$(CC) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS) $(LIBS)

# .PHONY:
# clean:
# 	@rm -f $(TARGET)

# run:
# 	@./$(TARGET)

output: main.o TDES.o Random.o RSA.o AES.o
	g++ $(CXXFLAGS) main.o TDES.o Random.o RSA.o AES.o -o output $(LIBS)
	
main.o: main.cpp Engine.hpp
	g++ -c main.cpp

TDES.o: TDES.cpp TDES.h
	g++ -c TDES.cpp

Random.o: Random.cpp Random.h
	g++ -c -std=c++11 -pthread -march=native Random.cpp $(LIBS)

RSA.o: RSA.cpp Random.h RSA.h AES.h conio.hpp
	g++ -c -std=c++11 -pthread -march=native RSA.cpp $(LIBS)

AES.o: AES.cpp AES.h
	g++ -c AES.cpp

.PHONY:
clean:
	rm -f *.o output
run:
	./output