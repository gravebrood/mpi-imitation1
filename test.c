#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv){

	int myrank, nprocs, resultlength;
	char name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init (&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Get_processor_name(name,&resultlength);
	printf("hello from processor %d of %d",myrank,nprocs);
	printf(" from %s\n",name);

	MPI_Finalize();

	return 0;
}
