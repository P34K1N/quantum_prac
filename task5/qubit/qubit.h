#ifndef QUBIT_H
#define QUBIT_H

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

static complexd Hm[2][2] = {{1 / std::sqrt(2), 1 / std::sqrt(2)}, {1 / std::sqrt(2), -1 / std::sqrt(2)}};
static complexd NOTm[2][2] = {{0, 1}, {1, 0}};
static complexd ROTm[2][2] = {{1, 0}, {0, -1}};
static complexd CNOTm[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 1}, {0, 0, 1, 0}};
static complexd CROTm[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, -1}};
static complexd Rmm[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

class Qubit {
    int *** transformTable;
    complexd * a;
    complexd * ac;
    int cur_distr[2];
    int act_distr[2];
    int n;
    int max;
    int myrank;
    int size;
    int block_size;
    int arr_size;
    int block_num;
    int sizelog;
    int first_qubits;
    MPI_Datatype BLOCK;

    // public:
    void init();
    void calculateTransform(int n1, int n2); 
    bool isCalculated(int n1, int n2) {return (transformTable[n1 - 1][n2 - 1] != NULL);}
    void resend(int n1, int n2);
    void debugPrint();
    void oneQubitTransform(int k, complexd U[2][2]);
    void twoQubitTransform(int k1, int k2, complexd U[4][4]);

    public:
    Qubit(int _n, int state = -1);
    Qubit(const char * filename);
    int nGet() {return n;};
    void write(const char * filename);
    void H(int k) {oneQubitTransform(k, Hm);}
    void Hn() {for (int i = 1; i <= n; i++) H(i);}
    void NOT(int k) {oneQubitTransform(k, NOTm);}
    void ROT(int k) {oneQubitTransform(k, ROTm);}
    void CNOT(int k1, int k2) {twoQubitTransform(k1, k2, CNOTm);}
    void CROT(int k1, int k2) {twoQubitTransform(k1, k2, CROTm);}
    void Rm(int k1, int k2, int m) {Rmm[3][3] = exp(2 * M_PI * complexd(0, 1) / double(1 << m)); twoQubitTransform(k1, k2, Rmm);}
    ~Qubit();
};

#endif
