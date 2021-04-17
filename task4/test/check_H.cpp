#include <iostream>
#include <cstdio>
#include <complex>

typedef std::complex<double> complexd;

void transform (int n, int ord, complexd U[2][2], complexd * a) {
    int k = n - ord;
    int diff = 1 << k;
    int mask = diff - 1;
    int max = 1 << (n - 1);
    for (int i = 0; i < max; i++) {
        int zero = ((i & ~mask) << 1) | (i & mask);
        int one = zero | diff;
        complexd new0 = U[0][0] * a[zero] + U[0][1] * a[one];
        complexd new1 = U[1][0] * a[zero] + U[1][1] * a[one];
        a[zero] = new0;
        a[one] = new1;
    }
}

int main(int argc, char * argv[]) {
    FILE * f = fopen(argv[1], "rb");
    int n;
    fread(&n, sizeof(n), 1, f);
    int max = 1 << n;
    complexd * a = new complexd [max];
    fread(a, sizeof(*a), max, f);
    fclose(f);

    complexd U[2][2] = {{1 / std::sqrt(2), 1 / std::sqrt(2)}, {1 / std::sqrt(2), -1 / std::sqrt(2)}};
    transform(n, std::strtol(argv[2], NULL, 10), U, a);

    f = fopen(argv[3], "wb");
    fwrite(&n, sizeof(n), 1, f);
    fwrite(a, sizeof(*a), max, f);
    fclose(f);
    delete [] a;
}