all: main
main: main.cpp
	g++ -o $@ $^ -std=c++11 -fopenmp
report: main
	./report_script.sh
collect: 
	./collect_script.sh

