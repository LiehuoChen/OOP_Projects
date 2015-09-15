/*

    cs540::assignment2::Performance_Speed_Test
    Siyuan Fu (富思源)
    sfu2@binghamton.edu

    The program generated lots of integers,
    then see the time cost of seq insert, destory, rand insert, rand lookup

*/
#include <map>
#include "map.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

class TimeCounter
{
public:
	TimeCounter() {reset();}
	~TimeCounter() {}
	void reset() { gettimeofday(&_start, NULL); }
	long GetTimeMilliS() {return get_interval()/1000;}
	long GetTimeMicroS() {return get_interval();}
	long GetTimeS() {return get_interval()/1000000;}
private:
	long get_interval() 
	{ 
		struct timeval now; 
		gettimeofday(&now, NULL); 
		return (long)(now.tv_sec - _start.tv_sec)*1000000 
			+ now.tv_usec - _start.tv_usec;
	}
	struct timeval _start;
};

int main(int argc, char *argv[])
{
    int test_case = 2000000;
    printf("Usage: %s keynum\n", argv[0]);
    printf("Defaut key num 2000000\n");
    if (argc > 1)
        test_case = atoi(argv[1]);

    int *keys = new int[test_case];

    std::map<int, int> stdmap;
    cs540::Map<int, int> cs540map;

    TimeCounter tc;
    tc.reset();
    for(int i = 0; i < test_case; i++) {
        stdmap.insert({i, i});
    }
    printf("std::map   sequentially insert %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());

    tc.reset();
    for(int i = 0; i < test_case; i++) {
        cs540map.insert({i, i});
    }
    printf("cs540::Map sequentially insert %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());

    tc.reset();
    stdmap.clear();
    printf("std::map   clear() %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());
        
    tc.reset();
    cs540map.clear();
    printf("cs540::Map clear() %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());

    printf("Making random numbers for insert....");
    srand(time(nullptr));
    for(int i = 0; i < test_case; i++)
        keys[i] = i;
    for(int i = 0; i < test_case; i++) {
        unsigned int p1 = ((rand() << 16) + (rand() & 0xffff)) % (unsigned)test_case;
        unsigned int p2 = ((rand() << 16) + (rand() & 0xffff)) % (unsigned)test_case;
        int tmp = keys[p1];
        keys[p1] = keys[p2];
        keys[p2] = tmp;
    }
    printf("Done\n");

    tc.reset();
    for(int i = 0; i < test_case; i++) {
        stdmap.insert({keys[i], i});
    }
    printf("std::map   randomly insert %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());

    tc.reset();
    for(int i = 0; i < test_case; i++) {
        cs540map.insert({keys[i], i});
    }
    printf("cs540::Map randomly insert %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());

    printf("Making random numbers for find....");
    for(int i = 0; i < test_case; i++) {
        unsigned int p1 = ((rand() << 16) + (rand() & 0xffff)) % (unsigned)test_case;
        unsigned int p2 = ((rand() << 16) + (rand() & 0xffff)) % (unsigned)test_case;
        int tmp = keys[p1];
        keys[p1] = keys[p2];
        keys[p2] = tmp;
    }
    printf("Done\n");

    int r = 0;
    tc.reset();
    for(int i = 0; i < test_case; i++) {
        r += stdmap.at(keys[i]);
    }
    printf("std::map   randomly find %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());

    tc.reset();
    for(int i = 0; i < test_case; i++) {
        r += cs540map.at(keys[i]);
    }
    printf("cs540::Map randomly find %d ints: %ld ms\n",
            test_case, tc.GetTimeMilliS());

    delete[] keys;
    (void)r;
    return 0;
}

