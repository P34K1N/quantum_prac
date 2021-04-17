#include <iostream>
#include <fstream>
#include <mpi.h>

#include "../qubit/qubit.cpp"


int main(int argc, char * argv[]) {
    MPI_Init(&argc, &argv);

    Qubit example(std::strtol(argv[1], NULL, 10));
    double start, end;
    start = MPI_Wtime();
    example.CNOT(std::strtol(argv[2], NULL, 10), std::strtol(argv[3], NULL, 10));
    end = MPI_Wtime();
    std::ofstream f(argv[4]);
    f << (end - start) << std::endl;
    f.close();

    MPI_Finalize();
}
