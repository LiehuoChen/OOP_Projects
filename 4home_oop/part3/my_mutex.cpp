#include <iostream>
#include <thread>
#include "errorchecking_mutex.hpp"

cs540::errorchecking_mutex mtx;
cs540::errorchecking_mutex mtx2;
cs540::errorchecking_mutex mtx3;
void print_thread_id (int id) {
    // critical section (exclusive access to std::cout signaled by locking mtx):
    try {
        mtx.lock();
        std::cout << "thread #" << id << '\n';
        mtx.unlock();
    } catch(cs540::LockedByOtherThread & e) {
        std::cout << e.message << std::endl;
    } catch(cs540::ResursiveLock &e) {
        std::cout << e.message << std::endl;
    } catch(cs540::NotLocked & e) {
        std::cout << e.message << std::endl;
    }
}

void recursive_func() {
    mtx2.lock();
    recursive_func();
    mtx2.unlock();
}

void recursive_thread (int arg) {
    try{
        /*
        mtx.lock();
        std::cout << "thread #" << id << '\n';
        mtx.unlock();
        */
        recursive_func();
    } catch(cs540::LockedByOtherThread & e) {
        std::cout << e.message << std::endl;
    } catch(cs540::ResursiveLock &e) {
        std::cout << e.message << std::endl;
    } catch(cs540::NotLocked & e) {
        std::cout << e.message << std::endl;
    }
}

void nonlocked_thread(int arg) {
    try{
        mtx3.unlock();
    } catch(cs540::LockedByOtherThread & e) {
        std::cout << e.message << std::endl;
    } catch(cs540::ResursiveLock &e) {
        std::cout << e.message << std::endl;
    } catch(cs540::NotLocked & e) {
        std::cout << e.message << std::endl;
    }
}

int main ()
{
    std::thread threads[10];
    std::thread threads2[1];
    std::cout << "===== locked by other thread test =====" << std::endl;
    // spawn 10 threads:
    for (int i=0; i<10; ++i)
        threads[i] = std::thread(print_thread_id,i+1);

    for (auto& th : threads) th.join();

    std::cout << "===== recursive lock test =====" << std::endl;
    for (int i=0; i<1; ++i)
        threads2[i] = std::thread(recursive_thread,i+1);

    for (auto& th : threads2) th.join();

    std::cout << "===== not locked test =====" << std::endl;
    std::thread thread3 = std::thread(nonlocked_thread, 1);
    thread3.join();


    return 0;
}

