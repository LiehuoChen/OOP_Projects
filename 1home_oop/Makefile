PROJECT = prog1
SRC_FILES = \
	Deque.hpp \
	test.cpp \
	Makefile \
	README

test.out:test.cpp Deque.hpp
	g++ -Wall -Wextra -std=c++11 -g -std=c++11 $^ -o $@

clean:
	rm -rf *.out *.o core *.core *~

submit: $(SRC_FILES)
	tar cvfz $(PROJECT).tar.gz $(SRC_FILES)
