CC=gcc
#CFLAGS=-ggdb3 -Wall 
CFLAGS=-Wall -O3

#fsanitize=-fsanitize=undefined
#CC=clang-3.5 $(fsanitize) `llvm-config --ldflags`
#CFLAGS=-fno-omit-frame-pointer -g -Wall -pedantic -mtune=native

CXX=g++
CXXFLAGS= -std=c++11 -ggdb3 -Wall -I/usr/local/include

PROGRAMS=only_ascii mean_strlen orderfreq construct_search timeread test_ffr multiple_8 construct_search_cpp
OBJS=only_ascii.o mean_strlen.o orderfreq.o fast_file_reader.o multiple_8.o construct_search_cpp.o

all: $(PROGRAMS)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

only_ascii: only_ascii.o
	$(CC) -o $@ $^ $(CFLAGS)

orderfreq: orderfreq.o fast_file_reader.o
	$(CC) -o $@ $^ $(CFLAGS)

test_ffr: test_ffr.o fast_file_reader.o
	$(CC) -o $@ $^ $(CFLAGS)

timeread: timeread.o
	$(CC) -o $@ $^ $(CFLAGS)

construct_search: construct_search.o 
	$(CC) -o $@ $^ $(CFLAGS)

construct_search_cpp: construct_search_cpp.o 
	$(CXX) -o $@ $^ $(CXXFLAGS)

multiple_8: multiple_8.o 
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core $(PROGRAMS) $(OBJS)

