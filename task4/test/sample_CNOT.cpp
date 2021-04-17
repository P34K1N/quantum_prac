#include <iostream>
#include <mpi.h>

#include "../qubit/qubit.cpp"


int main(int argc, char * argv[]) {
    MPI_Init(&argc, &argv);

    Qubit example(argv[1]);
    example.CNOT(std::strtol(argv[2], NULL, 10), std::strtol(argv[3], NULL, 10));
    example.write(argv[4]);

    MPI_Finalize();
}