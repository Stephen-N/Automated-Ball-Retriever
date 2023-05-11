CXX=g++
CXXFLAGS=-w -g -pedantic -Wall -fno-omit-frame-pointer -lSDL2 -lSDL2main -lSDL2_image -lpqxx -lpq -lwiringPi #-fsanitize=address,undefined
LDLIBS=-lpthread -lrt #-Wextra


SRCS=brain.cpp sensor.cpp website.cpp #gostop.cpp
DEPS=common.cpp SHMRequestChannel.cpp
BINS=$(SRCS:%.cpp=%.exe)
OBJS=$(DEPS:%.cpp=%.o)


all: clean $(BINS)

%.o: %.cpp %.h
	$(CXX) -std=c++2a $(CXXFLAGS) -c -o $@ $<

%.exe: %.cpp $(OBJS)
	$(CXX) -std=c++2a $^ $(CXXFLAGS) -o $(patsubst %.exe,%,$@) $(LDLIBS)


.PHONY: clean print-var run

clean:
	rm -f brain sensor website data*_* *.tst *.o *.csv data/*

print-var:
	echo $(OUT)

website:
	g++ website.cpp common.cpp SHMRequestChannel.cpp -w -g -pedantic -Wall -fno-omit-frame-pointer -lSDL2 -lSDL2main -lSDL2_image -lpqxx -lpq -lwiringPi -lpthread -lrt -o website

run: all
	./brain