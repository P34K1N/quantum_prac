#include <iostream>
#include <fstream>
#include <complex>
#include <cstdlib>
#include <cstring>

typedef std::complex<double> complexd;

void generate (unsigned n, complexd * a) {
    double abs = 0.0;
    unsigned max = 1 << n;
    for (unsigned i = 0; i < max; i++) {
        a[i] = complexd((std::rand() / (double) RAND_MAX - 0.5), (std::rand() / (double) RAND_MAX - 0.5));
        //a[i] = i;
        abs += std::abs(a[i]) * std::abs(a[i]);
    }
    abs = std::sqrt(abs);
    
    for (unsigned i = 0; i < max; i++) {
        a[i] /= abs;
    }
}

int main (int argc, char * argv []) {
    srand(time(NULL));
    unsigned n = std::strtoul(argv[1], nullptr, 10);
    std::ofstream f;
    f.open(argv[2], std::ios::out | std::ios::binary);
    unsigned max = 1 << n;
    complexd * a = new complexd [max];
    generate(n, a);
    f.write((char *) &n, sizeof(n));
    f.write((char *) a, max * sizeof(*a));
    f.close();
    delete [] a;
}
