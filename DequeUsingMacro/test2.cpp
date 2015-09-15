#include "Deque.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <deque>

Deque_DEFINE(int)
Deque_DEFINE(short)
Deque_DEFINE(char)

bool int_less(const int &a, const int &b) {
	return a < b;
}

int int_ctor(void) {
	return rand();
}

#define NCYCLES 10          //Number of ctor dtor cycles
#define NITERATIONS 1000    //Number of operation/assert cycles
#define NOPERATIONS 1000    //Number of push/pop/clear operations
#define NITERATOR 100       //Number of iterator operations testing operations

int
main(void) {
	srand(time(NULL));
	Deque_int d;
	for (size_t i = 0; i < NCYCLES; ++i) {
		std::deque<int> s;
		Deque_int_ctor(&d, int_less);
		Deque_int d2;
		for (size_t i = 0; i < NITERATIONS; ++i) {
			for (size_t i = 0; i < NOPERATIONS; ++i) {
				int val;
				switch(rand() % 100) {
					case 0:
						d.clear(&d);
						s.clear();
						break;
					default:
						switch(rand() % 3) {
							case 0:
								switch(rand() % 2) {
									case 0:
										if (s.empty()) continue;
										d.pop_front(&d);
										s.pop_front();
										break;
									case 1:
										if (s.empty()) continue;
										d.pop_back(&d);
										s.pop_back();
								}
							default:
								switch(rand() % 2) {
									case 0:
										val = int_ctor();
										d.push_front(&d, val);
										s.push_front(val);
										break;
									case 1:
										val = int_ctor();
										d.push_back(&d, val);
										s.push_back(val);
								}
						}
				}

			}
            //printf("the size for my deque is %lu \n",d.size(&d));
            //printf("the size for the lib de queue is %lu \n",s.size());
			assert(d.size(&d) == s.size());
			assert(d.empty(&d) == s.empty());

			for (size_t i = 0; i < s.size(); ++i) {
				assert(d.at(&d, i) == s.at(i));
			}

			Deque_int_ctor(&d2, int_less);
			for (size_t i = 0; i < s.size(); ++i)
				d2.push_back(&d2, s.at(i));
			assert(Deque_int_equal(d, d2));
			assert(Deque_int_equal(d2, d));
			d2.dtor(&d2);
			//Deque_int_dtor(&d2);

			Deque_int_Iterator dit = d.begin(&d);
			std::deque<int>::iterator sit = s.begin();
			for (size_t i = 0; i < NITERATOR; ++i) {
				switch(rand() % 2) {
					case 0:
						if (sit == s.end()) continue;
						++sit;
						dit.inc(&dit);
						break;
					case 1:
						if (sit == s.begin()) continue;
						--sit;
						dit.dec(&dit);
						break;
				}
				assert((sit == s.begin()) == Deque_int_Iterator_equal(dit, d.begin(&d)));
				assert((sit == s.end()) == Deque_int_Iterator_equal(dit, d.end(&d)));
				if (sit == s.end()) continue;
				assert(*sit == dit.deref(&dit));
			}

			Deque_int_Iterator dit2 = dit = d.begin(&d);
			int itval = 0, itval2 = 0;
			for (size_t i = 0; i < NITERATOR; ++i) {
				switch(rand() % 4) {
					case 0:
						if (Deque_int_Iterator_equal(dit, d.end(&d))) continue;
						dit.inc(&dit);
						++itval;
						break;
					case 1:
						if (Deque_int_Iterator_equal(dit, d.begin(&d))) continue;
						dit.dec(&dit);
						--itval;
						break;
					case 2:
						if (Deque_int_Iterator_equal(dit2, d.end(&d))) continue;
						dit2.inc(&dit2);
						++itval2;
						break;
					case 3:
						if (Deque_int_Iterator_equal(dit2, d.begin(&d))) continue;
						dit2.dec(&dit2);
						--itval2;
						break;
				}
				assert((itval == itval2) == Deque_int_Iterator_equal(dit, dit2));
				assert((itval == itval2) == Deque_int_Iterator_equal(dit2, dit));
			}

			if (s.empty()) continue;
			assert(d.front(&d) == s.front());
			assert(d.back(&d) == s.back());
		}
		d.dtor(&d);
		//Deque_int_dtor(&d);
	}
	return 0;
}
