/*

This is a rough and simple test. It creates N mutexes and M threads. The threads iterate,
randomly picking a mutex to either lock, try_lock, or unlock.

To run, do:

    ./a.out n_mutexes n_threads
*/

#include "ErrorCheckingMutex.hpp"
#include <thread>
#include <iostream>
#include <vector>
#include <vector>
#include <stdio.h>
#include <random>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <set>
#include <atomic>
#include <sstream>

using namespace std;
using namespace cs540;

bool keep_going = true;

// Can't use member = 0 syntax with atomics.
template <typename T>
struct Stats {
    Stats() :
     successful_locks(0),
     lock_recursive_locks(0),
     successful_trylocks(0),
     failed_trylocks(0),
     trylock_recursive_locks(0),
     successful_unlocks(0),
     notlocked_unlocks(0),
     locked_by_other_thread_unlocks(0) { }
    T
     successful_locks,
     lock_recursive_locks,
     successful_trylocks,
     failed_trylocks,
     trylock_recursive_locks,
     successful_unlocks,
     notlocked_unlocks,
     locked_by_other_thread_unlocks;
};

struct ErrorCheckingMutexInfo {
    ErrorCheckingMutexInfo() : locked_by(-1) {}
    ErrorCheckingMutex mutex;
    Stats<atomic_int_fast64_t> stats;
    atomic_int locked_by;
};

void
run(int id, vector<ErrorCheckingMutexInfo> &mutexes, Stats<int_fast64_t> *s) {

    default_random_engine eng(id);
    uniform_int_distribution<int> pick_mutex(0, mutexes.size() - 1);
    uniform_int_distribution<int> pick_action(0, 2);

    // Used to keep track of the max index of locks currently held, to make
    // sure that we never lock a lower-indexed lock, so as to avoid deadlock.
    class : public set<int> {
        public:
            int max() const {
                if (size() == 0) {
                    return -1;
                } else {
                    return *--end();
                }
            }
    } held;

    #define CS540_INC_STAT(v) do {                                             \
        s->v++;                                                                \
        mi.stats.v++;                                                          \
    } while (0)

    while (keep_going) {

        const int i = pick_mutex(eng);
        ErrorCheckingMutexInfo &mi(mutexes.at(i));

        switch (pick_action(eng)) {
            case 0:
                {
                    // We can only lock in ascending order, to avoid deadlock.
                    // Also can attempt if this thread already holds the lock.
                    if (i >= held.max() || held.find(i) != held.end()) {
                        try {
                            //fprintf(stderr, "%d: Locking %d\n", id, i);
                            mi.mutex.lock();
                            assert(mi.locked_by == -1);
                            mi.locked_by = id;
                            //fprintf(stderr, "%d: Locked %d\n", id, i);
                            CS540_INC_STAT(successful_locks);
                            held.insert(i);
                        } catch (RecursiveLock) {
                            CS540_INC_STAT(lock_recursive_locks);
                        } catch (LockedByOtherThread) {
                            //printf("%d error locked by other: %d\n", id, i);
                        }
                    }
                }
                break;
            case 1:
                {
                    try {
                        if (mi.mutex.try_lock()) {
                            assert(mi.locked_by == -1);
                            mi.locked_by = id;
                            //fprintf(stderr, "%d: Locked %d\n", id, i);
                            CS540_INC_STAT(successful_trylocks);
                            held.insert(i);
                        } else {
                            CS540_INC_STAT(failed_trylocks);
                        }
                    } catch (RecursiveLock) {
                        CS540_INC_STAT(trylock_recursive_locks);
                    } catch (LockedByOtherThread) {
                        //printf("%d error locked by other: %d\n", id, i);
                    }
                }
                break;
            case 2:
                {
                    try {
                        // To avoid race conditions, must clear the locked_by
                        // before the unlock.

                        if (mi.locked_by == id) {
                            mi.locked_by = -1;
                            mi.mutex.unlock();
                        } else {
                            mi.mutex.unlock();
                            assert(false);
                        }
                        //fprintf(stderr, "%d: Unlocked %d\n", id, i);
                        CS540_INC_STAT(successful_unlocks);
                        held.erase(i);
                    } catch (NotLocked) {
                        CS540_INC_STAT(notlocked_unlocks);
                    } catch (LockedByOtherThread) {
                        CS540_INC_STAT(locked_by_other_thread_unlocks);
                    }
                }
                break;
            default:
                assert(false); abort();
                break;
        }
    }

    // Unlock all held, so that any thread blocked will be unblocked.
    for (int i : held) {
        try {
            mutexes.at(i).locked_by = -1;
            mutexes.at(i).mutex.unlock();
        } catch (NotLocked) {
            fprintf(stderr, "Spurious NotLocked thrown in thread %d when unlocking mutex %d during cleanup.\n",
             id, i);
        } catch (LockedByOtherThread) {
            fprintf(stderr, "Spurious LockedByOtherThread in thread %d when unlocking mutex %d during cleanup.\n",
             id, i);
        } catch (...) {
            assert(false);
        }
    }
}

template <typename T>
ostream &
operator<<(ostream &os, const Stats<T> &s) {
    os << "    Successful locks: " <<  s.successful_locks << "\n";
    os << "    Recursive locks on lock(): " <<  s.lock_recursive_locks << "\n";
    os << "    Successful try locks: " <<  s.successful_trylocks << "\n";
    os << "    Failed try locks: " <<  s.failed_trylocks << "\n";
    os << "    Recursive locks on trylock(): " <<  s.trylock_recursive_locks << "\n";
    os << "    Sucessful unlocks: " <<  s.successful_unlocks << "\n";
    os << "    Not locked unlocks: " <<  s.notlocked_unlocks << "\n";
    os << "    Locked by other thread unlocks: " <<  s.locked_by_other_thread_unlocks << "\n";
    return os.flush();
}

void
bad_args() {
    fprintf(stderr, "Bad args, usage is: ./a.out n_mutexes n_threads.\n");
    exit(1);
}

int
main(int argc, char *argv[]) {

    int n_mutexes = -1, n_threads = -1;

    if (argc != 3) {
        bad_args();
    }
    istringstream (argv[1]) >> n_mutexes;
    istringstream (argv[2]) >> n_threads;
    if (n_mutexes <= 0 || n_threads <= 0) {
        bad_args();
    }
    printf("Running with %d mutexes and %d threads for 5 seconds.\n", n_mutexes, n_threads);

    vector<ErrorCheckingMutexInfo> mutexes(n_mutexes);
    vector<thread> threads(n_threads);
    vector<Stats<int_fast64_t>> stats(n_threads);

    for (size_t i = 0; i < threads.size(); i++) {
        threads.at(i) = thread(run, i, ref(mutexes), &stats.at(i));
    }

    sleep(5);
    keep_going = false;

    for (thread &t : threads) {
        t.join();
    }

    for (size_t i = 0; i < stats.size(); i++) {
        fprintf(stderr, "Thread %zu:\n", i);
        cerr << stats.at(i);
    }

    // Lock counts should increase as the index of mutexes increases.
    for (size_t i = 0; i < mutexes.size(); i++) {
        fprintf(stderr, "Mutex %zu:\n", i);
        cerr << mutexes.at(i).stats;
    }
}
