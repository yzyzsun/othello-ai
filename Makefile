CXXFLAGS = -g -Ofast -std=c++14
LDLIBS = -lstdc++

othello: cli.o othello.o

cli.o: othello.h
othello.o: othello.h

clean:
	rm *.o

.PHONY: clean
