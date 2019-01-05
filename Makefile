CC = g++
CFLAGS = -std=c++17 -pedantic 

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $<

pathfinder: pathfindermain.o actorgraph.o
	$(CC) $(CFLAGS) -o pathfinder pathfindermain.o actorgraph.o

predictorandrecommender: predictormain.o
	$(CC) $(CFLAGS) -o predictorandrecommender predictormain.o

popularityfinder: popularityfindermain.o
	$(CC) $(CFLAGS) -o popularityfinder popularityfindermain.o 

clean: 
	rm -f *.o pathfinder predictorandrecommender popularityfinder

