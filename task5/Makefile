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
	for i in 23 24 25 26 ; \
	do \
		for t in 1 2 4 8 16 ; \
		do \
			for u in 0 1 2 ; \
			do \
				# mpirun -n $$t ./qft g $$i /dev/null report/res_$${i}_$${t}_$${u}.dat ; \
				mpisubmit.pl -p $$t --stdout /dev/null --stderr /dev/null ./qft -- g $$i /dev/null report/res_$${i}_$${t}_$${u}.dat ; \
			done ; \
		done ; \
	done ;

collect: report/res_23_1_0.dat report/res_26_16_2.dat
	rm report/temp.dat ; \
	for i in 23 24 25 26 ; \
	do \
		for t in 1 2 4 8 16 ; \
		do \
			for u in 0 1 2 ; \
			do \
				cat report/res_$${i}_$${t}_$${u}.dat >> report/temp.dat ; \
			done ; \
			printf "$${i} $${t} " >> res.dat ; \
            awk '{ total += $$1; count++ } END  {print total / count}' report/temp.dat >> res.dat ; \
			rm report/temp.dat ; \
		done ; \
	done ;
