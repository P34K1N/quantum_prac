#include "qubit.h"

void Qubit::init() {
    // initializing stored variables
    double abs = 0.0, abs_tot = 0.0;
    max = 1 << n;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    sizelog = (int) (std::log(size) / std::log(2));
    first_qubits = sizelog + 2;
    block_size = max / size / 4;
    arr_size = block_size * 4;
    block_num = size * 4;
    a = new complexd[arr_size];
    ac = new complexd[arr_size];

    MPI_Type_contiguous(block_size, MPI_DOUBLE_COMPLEX, &BLOCK);
    MPI_Type_commit(&BLOCK);
}
void Qubit::calculateTransform(int n1, int n2) {
    //Calculating the distribution of qubit state vector blocks for qubits n1 and n2
    if (n1 > n2) {
        int tmp = n2;
        n2 = n1;
        n1 = tmp;
    }
    int * transformArr = new int [block_num];
    int bit1 = first_qubits - n1;
    int bit2 = first_qubits - n2;
    if (bit1 == bit2) bit2 = 0;
    if (bit1 == bit2) bit2++;

    for (int i = 0; i < block_num; i++) {
        int ic = i; //copy of i
        int ia = 0; //bit-reversed rank of processor for this block aka i but bits bit1 and bit2 are removed
        for (int j = 0; j < first_qubits; j++) {
            if (j != bit1 && j != bit2) {
                ia <<= 1;
                ia |= ic & 1;
            }
            ic >>= 1;
        }
        int ib = 0; //rank of processor for this block aka i but bits bit1 and bit2 are removed
        for (int j = 0; j < first_qubits - 2; j++) {
            ib <<= 1;
            ib |= ia & 1;
            ia >>= 1;
        }
        transformArr[i] = ib;
    }
    transformTable[n1 - 1][n2 - 1] = transformArr;
};
void Qubit::resend(int n1, int n2) {
    if (n1 > n2) {
        int tmp = n2;
        n2 = n1;
        n1 = tmp;
    }
    if (n1 > first_qubits && n2 > first_qubits) {
        cur_distr[0] = n1;
        cur_distr[1] = n2;
        return;
    }
    int new_act_distr[2];
    new_act_distr[0] = n1;
    new_act_distr[1] = n2;
    if (new_act_distr[1] > first_qubits) new_act_distr[1] = first_qubits;

    if (new_act_distr[0] == act_distr[0] && new_act_distr[1] == act_distr[1]) {
        cur_distr[0] = n1;
        cur_distr[1] = n2;
        return;
    }

    if (!isCalculated(new_act_distr[0], new_act_distr[1])) {
        calculateTransform(new_act_distr[0], new_act_distr[1]);
    }

    int old_blocks[4], new_blocks[4], to[4], from[4];
    int oi = 0, ni = 0;

    for (int i = 0; i < block_num; i++) {
        if (transformTable[act_distr[0] - 1][act_distr[1] - 1][i] == myrank) {
            old_blocks[oi] = i;
            oi++;
        }
        if (transformTable[new_act_distr[0] - 1][new_act_distr[1] - 1][i] == myrank) {
            new_blocks[ni] = i;
            ni++;
        }
    }

    for (int i = 0; i < 4; i++) {
        from[i] = transformTable[act_distr[0] - 1][act_distr[1] - 1][new_blocks[i]];
        to[i] = transformTable[new_act_distr[0] - 1][new_act_distr[1] - 1][old_blocks[i]];
    }

    MPI_Status stat;
    MPI_Request r[4];

    for (int i = 0; i < 4; i++) {
        MPI_Isend(a + i * block_size, 1, BLOCK, to[i], 0, MPI_COMM_WORLD, &r[i]);
    }
    for (int i = 0; i < 4; i++) {
        MPI_Recv(ac + i * block_size, 1, BLOCK, from[i], 0, MPI_COMM_WORLD, &stat);
    }
    for (int i = 0; i < 4; i++) {
        MPI_Wait(&r[i], &stat);
    }
    memcpy(a, ac, arr_size * sizeof(*a));

    cur_distr[0] = n1;
    cur_distr[1] = n2;
    act_distr[0] = new_act_distr[0];
    act_distr[1] = new_act_distr[1];
}
void Qubit::debugPrint() {
    for (int i = 0; i < size; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (myrank == i) {
            std::cout << "myrank = " << i << std::endl;
            // for (int i = 0; i < arr_size; i++) {
            //     std::cout << a[i] << " ";
            // }
            std::cout << a[0] << " " << a[block_size] << " " << a[2 * block_size] << " " << a[3 * block_size] << std::endl;
            // std::cout << std::endl;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (myrank == 0) std::cout << std::endl;
}
void Qubit::oneQubitTransform(int k, complexd U[2][2]) {
    if (k > n) {
        throw "Requested number of qubit is bigger than the number of qubits in a vector";
    }
    resend(k, k);

    //debug_print();

    int bit = n - k;
    if (bit > n - first_qubits + 1) bit = n - first_qubits + 1;

    int diff = 1 << bit;
    int mask = diff - 1;
    for (int i = 0; i < arr_size / 2; i++) {
        int zero = ((i & ~mask) << 1) | (i & mask);
        int one = zero | diff;
        complexd new0 = U[0][0] * a[zero] + U[0][1] * a[one];
        complexd new1 = U[1][0] * a[zero] + U[1][1] * a[one];
        a[zero] = new0;
        a[one] = new1;
    }

    //debug_print();
}
void Qubit::twoQubitTransform(int k1, int k2, complexd U[4][4]) {
    if (k1 > n || k2 > n) {
        throw "Requested number of qubit is bigger than the number of qubits in a vector";
    }

    if (k1 == k2) {
        throw "Cannot apply a two-qubit transformation to same qubits";
    }
    bool is_swapped = false;

    if (k1 > k2) {
        int tmp = k1;
        k1 = k2;
        k2 = tmp;
        is_swapped = true;
    }
    resend(k1, k2);
    int bit1 = n - k1;
    if (bit1 > n - first_qubits + 1) bit1 = n - first_qubits + 1;
    int bit2 = n - k2;
    if (bit2 > n - first_qubits + 1) bit2 = n - first_qubits + 1;
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

    for (int i = 0; i < arr_size / 4; i++) {
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
Qubit::Qubit(int _n, int state): n(_n) {
    // Randomly generating a state vector of n qubits

    if (state < -1 || state >= max) {
        throw "State cannot be a negative number or bigger than 2^n - 1";
    }

    init();

    if (state == -1) {
        double abs, abs_tot;
        // generating normal vector
        for (int i = 0; i < arr_size; i++) {
            a[i] = complexd((std::rand() / (double) RAND_MAX - 0.5), (std::rand() / (double) RAND_MAX) - 0.5);
            abs += std::abs(a[i]) * std::abs(a[i]);
            // a[i] = i + myrank * arr_size;
        }
        // normalizing the vector

        MPI_Reduce(&abs, &abs_tot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Bcast(&abs_tot, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        abs_tot = std::sqrt(abs_tot);
        for (int i = 0; i < block_size; i++) {
            a[i] /= abs_tot;
        }
    } else {
        if (state / arr_size == myrank) {
            a[state % arr_size] = 1;
        }
    }

    //assuming blocks are distributed contiguously, so they're distributed by qubits (first_qubits - 1) and firts_qubits
    cur_distr[0] = first_qubits - 1;
    cur_distr[1] = first_qubits;
    act_distr[0] = first_qubits - 1;
    act_distr[1] = first_qubits;
    transformTable = (int ***)calloc(first_qubits, sizeof(*transformTable));
    for (int i = 0; i < first_qubits; i++) {
        transformTable[i] = (int **)calloc(first_qubits, sizeof(**transformTable));
    }

    calculateTransform(cur_distr[0], cur_distr[1]);
}
Qubit::Qubit(const char * filename) {
    MPI_File f;
    MPI_Status stat;
    char filenamec[256];
    std::strcpy(filenamec, filename);
    MPI_File_open(MPI_COMM_WORLD, filenamec, MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
    MPI_File_read_all(f, &n, 1, MPI_INT, &stat);

    init();

    MPI_File_set_view(f, sizeof(n) + arr_size * sizeof(complexd) * myrank, MPI_DOUBLE, BLOCK, "native", MPI_INFO_NULL);
    MPI_File_read_all(f, a, 4, BLOCK, &stat);
    MPI_File_close(&f);

    cur_distr[0] = first_qubits - 1;
    cur_distr[1] = first_qubits;
    act_distr[0] = first_qubits - 1;
    act_distr[1] = first_qubits;
    transformTable = (int ***)calloc(first_qubits, sizeof(*transformTable));
    for (int i = 0; i < first_qubits; i++) {
        transformTable[i] = (int **)calloc(first_qubits, sizeof(**transformTable));
    }

    calculateTransform(cur_distr[0], cur_distr[1]);
}
void Qubit::write(const char * filename) {
    resend(first_qubits - 1, first_qubits);

    MPI_File f;
    MPI_Status stat;
    char filenamec[256];
    std::strcpy(filenamec, filename);
    MPI_File_open(MPI_COMM_WORLD, filenamec, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &f);
    if (myrank == 0) MPI_File_write(f, &n, 1, MPI_INT, &stat);

    MPI_File_set_view(f, sizeof(n) + arr_size * sizeof(complexd) * myrank, MPI_DOUBLE, BLOCK, "native", MPI_INFO_NULL);
    MPI_File_write_all(f, a, 4, BLOCK, &stat);
    MPI_File_close(&f);
}
Qubit::~Qubit() {
        for (int i = 0; i < first_qubits; i++) {
            for (int j = 0; j < first_qubits; j++) {
                if (transformTable[i][j] != NULL) {
                    delete [] transformTable[i][j];
                }
            }
            delete [] transformTable[i];
        }
        delete [] transformTable;
        delete [] a;
        delete [] ac;
    }


