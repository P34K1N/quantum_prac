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

const unsigned print_rank = 3;

void generate (unsigned n, complexd * a, unsigned myrank) {
    double abs = 0.0, abs_tot = 0.0;
    unsigned max = 1 << n;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (unsigned i = 0; i < max; i++) {
        a[i] = complexd((std::rand() / (double) RAND_MAX - 0.5), (std::rand() / (double) RAND_MAX) - 0.5);
        abs += std::abs(a[i]) * std::abs(a[i]);
        //if (i >= max / 2) a[i] = i - max / 2 + max * size / 2 + myrank * max / 2; else a[i] = i + myrank * max / 2;
        
    }
    MPI_Reduce(&abs, &abs_tot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Bcast(&abs_tot, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    abs_tot = std::sqrt(abs_tot);
    for (unsigned i = 0; i < max; i++) {
        a[i] /= abs_tot;
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
        fflush(stdout);
    }
    std::cout << std::endl;
    fflush(stdout);
}

void rearrange (unsigned size, unsigned myrank, unsigned n, complexd * a, unsigned k_old, unsigned k_new) {
    unsigned max = 1 << n;
    MPI_Datatype COMPLEXD, BLOCK;

    MPI_Type_contiguous(2, MPI_DOUBLE, &COMPLEXD);
    MPI_Type_commit(&COMPLEXD);
    MPI_Type_contiguous(max / size / 2, COMPLEXD, &BLOCK);
    MPI_Type_commit(&BLOCK);
    
    unsigned * orders_old = new unsigned [size * 2];
    unsigned * orders_new = new unsigned [size * 2]; 

    int old_blocks[2], new_blocks[2], to[2], from[2];

    int stride = size * 2 / (1 << k_old);
    if (stride < 1) stride = 1;
    int displc, add = 0;
    for (int i = 0; i < size; i++) {
        displc = i + add;
        if (((i + 1) & stride) != (i & stride)) add += stride;
        orders_old[displc] = i;
        orders_old[displc + stride] = i;
        if (i == myrank) {
            old_blocks[0] = displc;
            old_blocks[1] = displc + stride;
        }
    }

    stride = size * 2 / (1 << k_new);
    if (stride < 1) stride = 1;
    add = 0;
    for (int i = 0; i < size; i++) {
        displc = i + add;
        if (((i + 1) & stride) != (i & stride)) add += stride;
        orders_new[displc] = i;
        orders_new[displc + stride] = i;
        if (i == myrank) {
            new_blocks[0] = displc;
            new_blocks[1] = displc + stride;
        }
    }

    from[0] = orders_old[new_blocks[0]];
    from[1] = orders_old[new_blocks[1]];

    to[0] = orders_new[old_blocks[0]];
    to[1] = orders_new[old_blocks[1]];

    /*
    if (myrank == print_rank) {
        for (unsigned i = 0; i < (size * 2); i++) std::cout << orders_old[i] << " ";
        std::cout << std::endl;
        for (unsigned i = 0; i < (size * 2); i++) std::cout << orders_new[i] << " ";
        std::cout << std::endl;
        std::cout << old_blocks[0] << " " << old_blocks[1] << std::endl;
        std::cout << new_blocks[0] << " " << new_blocks[1] << std::endl;
        std::cout << from[0] << " " << from[1] << std::endl;
        std::cout << to[0] << " " << to[1] << std::endl;
    }
    */

    MPI_Status stat;

    MPI_Request r1, r2;

    MPI_Isend(a, 1, BLOCK, to[0], 0, MPI_COMM_WORLD, &r1);
    MPI_Isend(a + max / size / 2, 1, BLOCK, to[1], 0, MPI_COMM_WORLD, &r2);
    MPI_Recv(a, 1, BLOCK, from[0], 0, MPI_COMM_WORLD, &stat);
    MPI_Recv(a + max / size / 2, 1, BLOCK, from[1], 0, MPI_COMM_WORLD, &stat);

    delete [] orders_old;
    delete [] orders_new;

    MPI_Type_free(&COMPLEXD);
    MPI_Type_free(&BLOCK);
}

void noisify(complexd U[2][2], double noise_level) {
    double s = 0.;
    for (int i = 0; i < 12; ++i) { 
        s += (double) rand() / RAND_MAX; 
    }
    s -= 6.;
    double theta = noise_level * s;

    complexd E[2][2];
    E[0][0] = cos(theta);
    E[0][1] = sin(theta);
    E[1][0] = -E[0][1];
    E[1][1] = E[0][0];

    complexd Un[2][2];
    Un[0][0] = U[0][0] * E[0][0] + U[0][1] * E[1][0];
    Un[0][1] = U[0][0] * E[0][1] + U[0][1] * E[1][1];
    Un[1][0] = U[1][0] * E[0][0] + U[1][1] * E[1][0];
    Un[1][1] = U[1][0] * E[0][1] + U[1][1] * E[1][1];

    U[0][0] = Un[0][0];
    U[0][1] = Un[0][1];
    U[1][0] = Un[1][0];
    U[1][1] = Un[1][1];
}

complexd * nAdamar(unsigned size, unsigned myrank, unsigned n, complexd * a, double noise_level) {
    unsigned max = 1 << n;
    unsigned ord = 1;
    unsigned sizelog = (unsigned) (std::log(size) / std::log(2));
    complexd U[2][2];
    //if (myrank == print_rank) print(n - sizelog, a);
    for (unsigned k = 1; k <= n; k++) {
        U[0][0] = 1 / std::sqrt(2);
        U[0][1] = U[0][0];
        U[1][0] = U[0][0];
        U[1][1] = - U[0][0];
        
        // U[0][0] = 1;
        // U[0][1] = 0;
        // U[1][0] = 0;
        // U[1][1] = 1;
        
        noisify(U, noise_level);
        
        transform(n - sizelog, ord, U, a);
        //if (myrank == print_rank) print(n - sizelog, a);

        if (k <= sizelog) 
            rearrange(size, myrank, n, a, k, k + 1);
        else ord++;
        //if (myrank == print_rank) print(n - sizelog, a);
    }
    return a;
}

double dot(unsigned n, complexd * a, complexd * b) {
    unsigned max = 1 << n;
    double res = 0.0;
    for (int i = 0; i < max; i++) {
        res += std::abs(a[i] * b[i]);
    }

    int size, myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    /*
    for (int i = 0; i < size; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (i == myrank) {
            std::cout << res << std::endl;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (myrank == 0) std::cout << std::endl;
    */
    double fin;
    MPI_Reduce(&res, &fin, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    return fin;
}

int main (int argc, char * argv []) {
    MPI_Init(&argc, &argv);
    int myrank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    std::srand(std::time(NULL) + myrank);

    unsigned n, max;
    unsigned sizelog = (unsigned) (std::log(size) / std::log(2));
    complexd U[2][2];

    MPI_File f;
    MPI_Status stat;

    switch (argv[1][0]) {
        case 'f':
            MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
            MPI_File_read_all(f, &n, 1, MPI_UNSIGNED, &stat);
            break;
        case 'g':
            n = std::strtoul(argv[2], NULL, 10);
    }

    max = 1 << n;

    double noise_level = std::strtod(argv[3], NULL);

    complexd * a_loc = new complexd [max / size];

    if (argv[1][0] == 'g') {
        generate(n - sizelog, a_loc, myrank);
    }

    MPI_Datatype COMPLEXD, BLOCK, BLOCKx2_pre, BLOCKx2;
    MPI_Type_contiguous(2, MPI_DOUBLE, &COMPLEXD);
    MPI_Type_commit(&COMPLEXD);
    MPI_Type_contiguous(max / size / 2, COMPLEXD, &BLOCK);
    MPI_Type_commit(&BLOCK); 
    int stride = size;
    MPI_Type_vector(2, 1, stride, BLOCK, &BLOCKx2_pre);
    MPI_Type_commit(&BLOCKx2_pre);
    MPI_Type_create_resized(BLOCKx2_pre, 0, sizeof(*a_loc) * max / size / 2, &BLOCKx2);
    MPI_Type_commit(&BLOCKx2);

    int * displc = new int [size];
    int * counts = new int [size];
    int add = 0;
    for (int i = 0; i < size; i++) {
        counts[i] = 1;
        displc[i] = i + add;
        if (((i + 1) & stride) != (i & stride)) add += stride;
    }

    if (argv[1][0] == 'f') {
        MPI_File_set_view(f, sizeof(n) + (displc[myrank] * max / size / 2 * sizeof(complexd)), MPI_DOUBLE, BLOCKx2, "native", MPI_INFO_NULL);
        MPI_File_read_all(f, a_loc, max / size * 2, MPI_DOUBLE, &stat);
        MPI_File_close(&f);
    }

    MPI_Type_free(&BLOCKx2_pre);
    MPI_Type_free(&BLOCKx2);

    // for (int i = 0; i < size; i++) {
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (i == myrank) {
    //         std::cout << myrank << " " << a_loc[0] << std::endl;
    //     }
    // }

    // MPI_Barrier(MPI_COMM_WORLD);
    // if (0 == myrank) std::cout << std::endl;

    complexd * a_check = new complexd [max / size];

    memcpy(a_check, a_loc, max / size * sizeof(*a_loc));

    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    
    nAdamar(size, myrank, n, a_loc, noise_level);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    nAdamar(size, myrank, n, a_check, 0);

    MPI_Barrier(MPI_COMM_WORLD); 

    /*
    for (int i = 0; i < size; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (i == myrank) {
            print(n - sizelog, a_loc);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (myrank == 0) std::cout << std::endl;
    
    for (int i = 0; i < size; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (i == myrank) {
            print(n - sizelog, a_check);
        }
    }
    */
    stride = 1;
    MPI_Type_vector(2, 1, stride, BLOCK, &BLOCKx2_pre);
    MPI_Type_commit(&BLOCKx2_pre);
    MPI_Type_create_resized(BLOCKx2_pre, 0, sizeof(*a_loc) * max / size / 2, &BLOCKx2);
    MPI_Type_commit(&BLOCKx2);

    add = 0;
    for (int i = 0; i < size; i++) {
        counts[i] = 1;
        displc[i] = i + add;
        if (((i + 1) & stride) != (i & stride)) add += stride;
    }

    // for (int i = 0; i < size; i++) {
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (i == myrank) {
    //         std::cout << myrank << " " << a_loc[0] << std::endl;
    //     }
    // }

    // MPI_Barrier(MPI_COMM_WORLD);
    // if (0 == myrank) std::cout << std::endl;

    double d;

    switch (argv[1][0]) {
        case 'g':
            d = dot(n - sizelog, a_loc, a_check);
            if (myrank == 0) {
                std::ofstream ftime, facc;
                ftime.open(argv[4], std::ios::out);
                ftime << end - start << std::endl;
                ftime.close(); 
                
                facc.open(argv[5], std::ios::out);
                facc << 1 - d << std::endl;
                facc.close();
            }
            break;
        case 'f':
            MPI_File_open(MPI_COMM_WORLD, argv[4], MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &f);
            if (myrank == 0) {
                MPI_File_write(f, &n, 1, MPI_UNSIGNED, &stat);
            }
            MPI_File_set_view(f, sizeof(n) + (displc[myrank] * max / size / 2 * sizeof(complexd)), MPI_DOUBLE, BLOCKx2, "native", MPI_INFO_NULL);
            MPI_File_write_all(f, a_loc, max / size * 2, MPI_DOUBLE, &stat);
            MPI_File_close(&f);
            break;
        default: break;
    } 

    MPI_Type_free(&COMPLEXD);
    MPI_Type_free(&BLOCK);
    MPI_Type_free(&BLOCKx2_pre);
    MPI_Type_free(&BLOCKx2);
    
    delete [] a_loc;
    delete [] a_check;
    delete [] counts;
    delete [] displc;
    MPI_Finalize();
    return 0;
}
