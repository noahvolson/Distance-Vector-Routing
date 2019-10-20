CC := gcc

all: project3.o nodeCommon.o node0.o node1.o node2.o node3.o
	$(CC) -o project3 project3.o nodeCommon.o node0.o node1.o node2.o node3.o

clean:
	rm -r *.o project3
