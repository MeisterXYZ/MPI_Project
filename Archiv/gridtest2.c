#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char** argv) {

	//Initialize MPI
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){		
		printf("%s\n",argv[1]);
		long grSize = (long)strtol(argv[1], NULL, 10);
		int *grid;
		grid = malloc (sizeof(int)*grSize*grSize);
		//printf("%ld OK\n",grSize*grSize);
		//printf("1\n");
		long pos = 1*grSize+1;
		grid[pos]=0;
		//printf("2\n");
		//printf("%d\n",grid[1*grSize+1]);
		//printf("3\n");
		printf("%ld OK\n",grSize*grSize);

	}
	MPI_Finalize();
}
