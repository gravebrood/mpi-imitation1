CC=g++
CFLAGS=-g

all: Dsc_mpi Dsc_mpi_server  


Dsc_mpi: Dsc_mpi.cpp
	$(CC) $(CFLAGS) Dsc_mpi.cpp -o Dsc_mpi -lpthread

Dsc_mpi_server: Dsc_mpi_server.cc
	$(CC) $(CFLAGS) Dsc_mpi_server.cc -o Dsc_mpi_server

clean: 
	rm *.o Dsc_mpi Dsc_mpi_server
