all: main
main: main.cpp
	mpicxx -o $@ $^ -std=c++98 -O3
test: main
	./test_script.sh
report: main
	./report_script.sh
collect:
	./collect_script.sh

