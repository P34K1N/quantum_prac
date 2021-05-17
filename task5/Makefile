MPIC = mpicxx

all: qft
qft: qft.o qubit/qubit.o
	$(MPIC) -o $@ $^ -O2
qft.o: qft.cpp qubit/qubit.h
	$(MPIC) -c -o $@ $< -O2
qubit/qubit.o: qubit/qubit.cpp qubit/qubit.h
	$(MPIC) -c -o $@ $< -O2
clean: 
	rm -f *.o qubit/*.o qft

test/test_gen: test/test_gen.cpp
	g++ -o $@ $^ -O2
test/check_qft: test/check_qft.cpp
	g++ -o $@ $^ -O2
test/vec_comp: test/vec_comp.cpp
	g++ -o $@ $^ -O2
test_prep: test/test_gen test/check_qft test/vec_comp
	for i in 1 2 3 4 5; \
	do \
		./test/test_gen 16 test/test$${i} ; \
		./test/check_qft test/test$${i} test/res$${i} ; \
	done \

test: qft test/test1 test/res1
	for i in 1 2 3 4 5; \
	do \
		for t in 1 2 4 8 16; \
		do \
		mpirun -n $$t ./qft f test/test$${i} test/res /dev/null ; \
		./test/vec_comp test/res$${i} test/res ; \
		done ; \
	done ; \
	rm test/res ; \

report: qft
	for i in 25 ; \
	do \
		for t in 32 ; \
		do \
			for u in 0 ; \
			do \
				mpisubmit.bg -n $$t -w 00:05:00 ./qft -- g $$i /dev/null report/res_$${i}_$${t}_$${u}.dat ; \
			done ; \
		done ; \
	done ;

collect: report/res_25_32_0.dat report/res_28_512_2.dat
	for i in 25 26 27 28 ; \
	do \
		for t in 32 64 128 256 512 ; \
		do \
			for u in 0 1 2 ; \
			do \
				cat report/res_$${i}_$${t}_$${u}.dat >> report/temp.dat
			done ; \
			printf "${n[i]} ${t[s]} " >> res.dat ; \
            awk '{ total += $1; count++ } END  {print total / count}' temp.dat >> res.dat ; \
		done ; \
	done ;
