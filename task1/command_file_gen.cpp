#include <iostream>
#include <fstream>

int main(int argc, char * argv []) {
    std::ofstream f;
    f.open("bsub_comm.lsf", std::ios::out);
    f << "#BSUB -n 20" << std::endl << "#BSUB -W 00:15" << std::endl << "#BSUB -R " << '"' << "span[hosts=1]" << '"' << std::endl << "OMP_NUM_THREADS=" << argv[3] << " ../main " << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << std::endl;
    f.close();
}
