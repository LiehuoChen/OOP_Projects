PROJECT = prog2
SRC_FILES = \
	map.hpp \
	test.cpp \
	Makefile \
	README

test.out:testF.cpp 
	g++ -std=c++11 -O -Wall -Wextra -g -pedantic $^ -o $@

clean:
	rm -rf *.out *.o core *.core *~

submit: $(SRC_FILES)
	tar cvfz $(PROJECT).tar.gz $(SRC_FILES)
