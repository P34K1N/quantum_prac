#include <iostream>
#include <cstdio>
#include <complex>

typedef std::complex<double> complexd;

void transform1(int n, int ord, complexd U[2][2], complexd * a) {
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

void transform2(int n, int k1, int k2, complexd U[4][4], complexd * a) {
    int max = 1 << n;

    if (k1 == k2) {
        std::cerr << "Cannot apply a two-qubit transformation to same qubits" << std::endl;
        return;
    }
    bool is_swapped = false;

    if (k1 > k2) {
        int tmp = k1;
        k1 = k2;
        k2 = tmp;
        is_swapped = true;
    }

    int bit1 = n - k1;
    int bit2 = n - k2;
    if (bit1 == bit2) bit1--;

    int diff1 = 1 << bit1;
    int diff1_ = 1 << (bit1 - 1);
    int diff2 = 1 << bit2;
    int left = ~(diff1_ - 1);
    int mid = (diff1_ - 1) & ~(diff2 - 1);
    int right = diff2 - 1;
    if (is_swapped) {
        int tmp = diff1;
        diff1 = diff2;
        diff2 = tmp;
    }

    for (int i = 0; i < max / 4; i++) {
        int zero = ((i & left) << 2) | ((i & mid) << 1) | (i & right);
        int one = zero | diff2;
        int two = zero | diff1;
        int three = zero | diff1 | diff2;
        complexd neww[4];
        for (int i = 0; i < 4; i++) {
            neww[i] = U[i][0] * a[zero] + U[i][1] * a[one] + U[i][2] * a[two] + U[i][3] * a[three];
        }
        a[zero] = neww[0];
        a[one] = neww[1];
        a[two] = neww[2];
        a[three] = neww[3];
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

    complexd Rm[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, -1}};
    complexd H[2][2] = {{1 / std::sqrt(2), 1 / std::sqrt(2)}, {1 / std::sqrt(2), -1 / std::sqrt(2)}};

    for (int i = 1; i <= n; i++) {
        transform1(n, i, H, a);
        for (int j = i + 1; j <= n; j++) {
            Rm[3][3] = exp(2 * M_PI * complexd(0, 1) / double(1 << (j - i + 1)));
            transform2(n, i, j, Rm, a);
        }
    }

    f = fopen(argv[2], "wb");
    fwrite(&n, sizeof(n), 1, f);
    fwrite(a, sizeof(*a), max, f);
    fclose(f);
    delete [] a;
}