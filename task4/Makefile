test: 
	./test_script.sh
report_prep:
	cd report
	mpicxx -o report/sample_CNOT report/sample_CNOT.cpp -std=c++98 -O3
	mpicxx -o report/sample_Hn report/sample_Hn.cpp -std=c++98 -O3
report: report_prep
	./report_script.sh
collect:
	./collect_script.sh

