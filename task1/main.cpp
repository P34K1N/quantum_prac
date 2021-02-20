#include <iostream>
#include <fstream>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <omp.h>
#include <bitset>

typedef std::complex<double> complexd;
typedef unsigned long long ull;

void generate (unsigned n, complexd * a) {
    double abs = 0.0;
    ull max = 1ULL << n;
    for (ull i = 0; i < max; i++) {
        a[i] = complexd((std::rand() / (double) RAND_MAX - 0.5), (std::rand() / (double) RAND_MAX) - 0.5);
        abs += std::abs(a[i]) * std::abs(a[i]);
    }
    abs = std::sqrt(abs);
    for (ull i = 0; i < max; i++) {
        a[i] /= abs;
    }
}

void transform (unsigned n, unsigned ord, complexd U[2][2], complexd * a) {
    unsigned k = n - ord;
    ull diff = 1ULL << k;
    ull mask = diff - 1;
    ull max = 1ULL << (n - 1);
    for (ull i = 0; i < max; i++) {
        ull zero = ((i & ~mask) << 1) | (i & mask);
        ull one = zero | diff;
        complexd new0 = U[0][0] * a[zero] + U[0][1] * a[one];
        complexd new1 = U[1][0] * a[zero] + U[1][1] * a[one];
        a[zero] = new0;
        a[one] = new1;
    }
}

void transform_parallel (unsigned n, unsigned ord, complexd U[2][2], complexd * a) {
    unsigned k = n - ord;
    ull diff = 1ULL << k;
    ull mask = diff - 1;
    ull max = 1ULL << (n - 1);
    #pragma omp parallel for 
    for (ull i = 0; i < max; i++) {
        ull zero = ((i & ~mask) << 1) | (i & mask);
        ull one = zero | diff;
        complexd new0 = U[0][0] * a[zero] + U[0][1] * a[one];
        complexd new1 = U[1][0] * a[zero] + U[1][1] * a[one];
        a[zero] = new0;
        a[one] = new1;
    }
}

void print (unsigned n, complexd * a) {
    ull max = 1ULL << n;
    for (ull i = 0; i < max; i++) {
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
    std::srand(std::time(nullptr));
    unsigned n = std::strtoul(argv[1], NULL, 10);
    unsigned k = std::strtoul(argv[2], NULL, 10);
    omp_set_num_threads(std::strtoul(argv[3], NULL, 10));
    ull max = 1ULL << n;
    complexd U[2][2], * a = new complexd[max];
    //std::cin >> U[0][0] >> U[0][1] >> U[1][0] >> U[1][1];
    U[0][0] = 1 / std::sqrt(2);
    U[0][1] = U[0][0];
    U[1][0] = U[0][0];
    U[1][1] = - U[0][0];
    generate(n, a);
    //print(n, a);

    double start, end;
    /*
    start = omp_get_wtime();
    transform(n, k, U, a);
    end = omp_get_wtime();
    std::cout << end - start << std::endl;
    //print(n, a);
    */
    start = omp_get_wtime();
    transform_parallel(n, k, U, a);
    end = omp_get_wtime();

    std::ofstream f;
    f.open(argv[4], std::ios::out);
    f << end - start << std::endl;
    f.close();
    //print(n, a);

    delete [] a;
    return 0;
}
