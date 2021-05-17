#include <iostream>
#include <mpi.h>
#include "qubit/qubit.h"

int main(int argc, char * argv[]) {
    MPI_Init(&argc, &argv);

    Qubit * exampleref = argv[1][0] == 'g' ? new Qubit(std::strtol(argv[2], NULL, 10)) : new Qubit(argv[2]);
    Qubit & example = *exampleref;
    int n = example.nGet();
    double start = MPI_Wtime();
    for (int i = 1; i <= n; i++) {
        example.H(i);
        for (int j = i + 1; j <= n; j++) {
            example.Rm(i, j, j - i + 1);
        }
    }
    double end = MPI_Wtime();
    example.write(argv[3]);

    std::ofstream f(argv[4]);
    f << (end - start) << std::endl;

    delete exampleref;    

    MPI_Finalize();
}
