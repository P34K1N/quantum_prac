#include <iostream>
#include <mpi.h>

#include "../qubit/qubit.cpp"


int main(int argc, char * argv[]) {
    MPI_Init(&argc, &argv);

    Qubit example(std::strtol(argv[1], NULL, 10));
    double start, end;
    start = MPI_Wtime();
    example.Hn();
    end = MPI_Wtime();
    std::ofstream f(argv[2]);
    f << (end - start) << std::endl;
    f.close();

    MPI_Finalize();
}
