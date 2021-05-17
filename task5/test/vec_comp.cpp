#include <iostream>
#include <fstream>
#include <complex>
#include <cstdlib>
#include <cstring>

typedef std::complex<double> complexd;

const double EPS = 1e-6;

int main (int argc, char * argv []) {
    std::ifstream f1, f2;
    f1.open(argv[1], std::ios::out | std::ios::binary);
    f2.open(argv[2], std::ios::out | std::ios::binary);
    unsigned n, n_;
    f1.read((char *) &n, sizeof(n));
    f2.read((char *) &n_, sizeof(n_));
    if (n != n_) {
        std::cerr << "Sizes do not match" << std::endl;
        return 1;
    }
    unsigned max = 1 << n;
    for (unsigned i = 0; i < max; i++) {
        complexd a, b;
        f1.read((char *) &a, sizeof(a));
        f2.read((char *) &b, sizeof(b));
        if (std::abs(b - a) > EPS) {
            std::cerr << "Elements #" << i << " do not match: " << a << " in 1st file, " << b << " in 2nd." << std::endl;
            return 1;
        }
    }

    f1.close();
    f2.close();

    return 0;
}
