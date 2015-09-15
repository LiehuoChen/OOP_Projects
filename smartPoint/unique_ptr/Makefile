PROJECT = prog3
testc.out:testc.cpp 
	g++ -Wall -Wextra -std=c++11 -pedantic -g $^ -o $@
test.out:test.cpp 
	g++ -Wall -Wextra -std=c++11 -pedantic -g $^ -o $@
clean:
	rm -rf *.out *.o core *.core *~

submit: $(SRC_FILES)
	tar cvfz $(PROJECT).tar.gz $(SRC_FILES)
