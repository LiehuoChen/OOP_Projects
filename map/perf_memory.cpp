/*

    cs540::assignment2::Performance_OverheadMemory_Usage_Test
    Siyuan Fu (富思源)
    sfu2@binghamton.edu

    The program generated lots of integers in random order,
    insert them into map, then see the memory usage from OS

 */
#include <map>
#include "map.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int test_case = 2000000;
    int is_stdmap = 1;

    printf("Usage: %s [s|c] count\n", argv[0]);
    printf("s for stdmap, c for cs540map, default stdmap, with count = 2000000\n");
    if (argc > 2) {
        is_stdmap = !(argv[1][0] == 'c');
        test_case = atoi(argv[2]);
    }

    int *keys = new int[test_case];

    std::map<int, int> stdmap;
    cs540::Map<int, int> cs540map;

    printf("Making %d random numbers for insert....", test_case);
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
    printf("Inserting...");

    if (is_stdmap)
    for(int i = 0; i < test_case; i++) 
        stdmap.insert({keys[i], i});
    else    
    for(int i = 0; i < test_case; i++) 
        cs540map.insert({keys[i], i});

    printf("Done\n");

    char cmd[128];
    snprintf(cmd, 128, "pmap %d | grep anon | awk '{i+=$2};END{print i - %lu}'", (int)getpid(), sizeof(int)*3*test_case/1024);
    if (is_stdmap)
        printf("Memory overhead of std::map %d keys is (in KB): ", test_case);
    else
        printf("Memory overhead of cs540::Map %d keys is (in KB): ", test_case);
    
    fflush(stdout);
    (void)(system(cmd) + 1);
    delete[] keys;
    return 0;
}

