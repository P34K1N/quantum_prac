#include <iostream>
#include <mpi.h>

#include "../qubit/qubit.cpp"


int main(int argc, char * argv[]) {
    MPI_Init(&argc, &argv);

    Qubit example(argv[1]);
    example.Hn();
    example.write(argv[2]);

    MPI_Finalize();
}