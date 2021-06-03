# Makefile

# SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
# OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

# TARGET	:= main

# compile and lib parameter
# CC      := g++
CXXFLAGS:= -g -O2 -std=c++11 -pthread -march=native
LIBS    := -lntl -lgmp -lm
# LDFLAGS := 

# $(TARGET): $(OBJS)
# 	$(CC) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS) $(LIBS)

# .PHONY:
# clean:
# 	@rm -f $(TARGET)

# run:
# 	@./$(TARGET)

# output: main.o TDES.o PRNG.o PrimeGen.o RSA.o AES.o
# 	g++ $(CXXFLAGS) main.o TDES.o PRNG.o PrimeGen.o RSA.o AES.o -o output $(LIBS)
output: main.o AES.o
	g++ $(CXXFLAGS) main.o AES.o -o output
	
main.o: main.cpp
	g++ -c main.cpp

# TDES.o: TDES.cpp TDES.h
# 	g++ -c TDES.cpp

# PRNG.o: PRNG.cpp Random.h
# 	g++ -c -std=c++11 -pthread -march=native PRNG.cpp $(LIBS)

# PrimeGen.o: PrimeGen.cpp Random.h
# 	g++ -c -std=c++11 -pthread -march=native PrimeGen.cpp $(LIBS)

# RSA.o: Random.h RSA.h
# 	g++ -c -std=c++11 -pthread -march=native RSA.cpp $(LIBS)

AES.o: AES.cpp AES.h
	g++ -c AES.cpp
.PHONY:
clean:
	rm -f *.o output
run:
	./output