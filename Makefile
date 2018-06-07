CXXFLAGS = -Ofast -march=native -flto -std=c++14
LDFLAGS = -Ofast -march=native -flto
LDLIBS = -lstdc++

othello: cli.o othello.o
api: api.o othello.o

othello.o: othello.h
cli.o: othello.h
api.o: othello.h

clean:
	rm *.o

.PHONY: clean
