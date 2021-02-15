#include <iostream>
#include <fstream>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <bitset>
#include <mpi.h>
#include <cmath>

typedef std::complex<double> complexd;

void generate (unsigned n, complexd * a) {
    double abs = 0.0;
    unsigned max = 1 << n;
    for (unsigned i = 0; i < max; i++) {
        a[i] = complexd((std::rand() / (double) RAND_MAX - 0.5), (std::rand() / (double) RAND_MAX) - 0.5);
        abs += std::abs(a[i]) * std::abs(a[i]);
    }
    abs = std::sqrt(abs);
    for (unsigned i = 0; i < max; i++) {
        a[i] /= abs;
    }
}

void vread (unsigned & n, complexd *& a, char * filename) {
    std::ifstream f;
    f.open(filename, std::ios::in | std::ios::binary);
    f.read((char *) &n, sizeof(n));
    unsigned max = 1 << n;
    a = new complexd[max];
    f.read((char *) a, max * sizeof(*a));
    f.close();
}

void vwrite (unsigned n, complexd * a, char * filename) {
    std::ofstream f;
    f.open(filename, std::ios::out | std::ios::binary);
    f.write((char *) &n, sizeof(n));
    unsigned max = 1 << n;
    f.write((char *) a, max * sizeof(*a));
    f.close();
}

void transform (unsigned n, unsigned ord, complexd U[2][2], complexd * a) {
    unsigned k = n - ord;
    unsigned diff = 1 << k;
    unsigned mask = diff - 1;
    unsigned max = 1 << (n - 1);
    for (unsigned i = 0; i < max; i++) {
        unsigned zero = ((i & ~mask) << 1) | (i & mask);
        unsigned one = zero | diff;
        complexd new0 = U[0][0] * a[zero] + U[0][1] * a[one];
        complexd new1 = U[1][0] * a[zero] + U[1][1] * a[one];
        a[zero] = new0;
        a[one] = new1;
    }
}

void print (unsigned n, complexd * a) {
    unsigned max = 1 << n;
    for (unsigned i = 0; i < max; i++) {
        if (a[i].imag() == 0) {
            std::cout << a[i].real();
        } else if (a[i].real() == 0) {
            std::cout << a[i].imag() << "i";
        } else {
            std::cout << "(" << a[i].real() << " + " << a[i].imag() << "i)";
        }
        std::cout << "|" << std::bitset<32>(i).to_string().erase(0, 32 - n) << ">";
        if (i != max - 1) std::cout << " + ";
    }
    std::cout << std::endl;
}

int main (int argc, char * argv []) {
    MPI_Init(&argc, &argv);
    int myrank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    std::srand(std::time(nullptr));

    unsigned n, k, max;
    complexd U[2][2], * a;

    switch (argv[1][0]) {
        case 'g':
            n = std::strtoul(argv[2], nullptr, 10);
            k = std::strtoul(argv[3], nullptr, 10);
            max = 1 << n;
            if (myrank == 0) {
                a = new complexd[max];
                generate(n, a);
                //print(n, a);
            }
            break;
        case 'f':
            k = std::strtoul(argv[2], nullptr, 10);
            if (myrank == 0) {
                vread(n, a, argv[3]);
            }
            MPI_Bcast(&n, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
            max = 1 << n;
            break;
        default: break;
    }
    
    //std::cin >> U[0][0] >> U[0][1] >> U[1][0] >> U[1][1];
    
    U[0][0] = 1 / std::sqrt(2);
    U[0][1] = U[0][0];
    U[1][0] = U[0][0];
    U[1][1] = - U[0][0];
    /*
    U[0][0] = 0;
    U[0][1] = 1;
    U[1][0] = 1;
    U[1][1] = 0;
    */
    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    MPI_Datatype COMPLEXD, BLOCK, BLOCKx2_pre, BLOCKx2;
    MPI_Type_contiguous(2, MPI_DOUBLE, &COMPLEXD);
    MPI_Type_commit(&COMPLEXD);
    MPI_Type_contiguous(max / size / 2, COMPLEXD, &BLOCK);
    MPI_Type_commit(&BLOCK); 
    int stride = size * 2 / (1 << k);
    if (stride < 1) stride = 1;
    MPI_Type_vector(2, 1, stride, BLOCK, &BLOCKx2_pre);
    MPI_Type_commit(&BLOCKx2_pre);
    MPI_Type_create_resized(BLOCKx2_pre, 0, sizeof(*a) * max / size / 2, &BLOCKx2);
    MPI_Type_commit(&BLOCKx2);

    int * displc = new int [size];
    int * counts = new int [size];
    int add = 0;
    for (int i = 0; i < size; i++) {
        counts[i] = 1;
        displc[i] = i + add;
        if (((i + 1) & stride) != (i & stride)) add += stride;
    }

    complexd * a_loc = new complexd [max / size];

    MPI_Scatterv(a, counts, displc, BLOCKx2, a_loc, max / size, COMPLEXD, 0, MPI_COMM_WORLD);\

    unsigned ord;
    unsigned sizelog = (unsigned) std::log2(size);
    if (k - 1 < sizelog) ord = 1; else ord = k - sizelog;

    transform(n - sizelog, ord, U, a_loc);

    MPI_Gatherv(a_loc, max / size, COMPLEXD, a, counts, displc, BLOCKx2, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    switch (argv[1][0]) {
        case 'g':
            if (myrank == 0) {
                std::ofstream ftime;
                ftime.open(argv[4], std::ios::out);
                ftime << end - start << std::endl;
                ftime.close(); 
            }
            break;
        case 'f':
            if (myrank == 0) {
                vwrite(n, a, argv[4]);
            }
            break;
        default: break;
    } 

    if (myrank == 0) {
        delete [] a;
    }
    delete [] a_loc;
    delete [] counts;
    delete [] displc;
    MPI_Finalize();
    return 0;
}
