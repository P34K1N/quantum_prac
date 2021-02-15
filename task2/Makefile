all: main
main: main.cpp
	mpic++ -o $@ $^ -std=c++11
test: main
	./test_script.sh
report: main
	./report_script.sh
collect:
	./collect_script.sh

