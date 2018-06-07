CXXFLAGS = -g -Ofast -std=c++14
LDLIBS = -lstdc++

othello: cli.o othello.o
api: api.o othello.o

othello.o: othello.h
cli.o: othello.h
api.o: othello.h

clean:
	rm *.o

.PHONY: clean
