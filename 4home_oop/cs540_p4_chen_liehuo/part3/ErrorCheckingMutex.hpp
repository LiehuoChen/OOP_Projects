#ifndef _LH_ERROR_CHECKING_MUTEX
#define _LH_ERROR_CHECKING_MUTEX
#include <iostream>
#include <mutex>
#include <thread>
#include <string>


namespace cs540 {
struct RecursiveLock {
    std::string message;
    RecursiveLock(std::string name) : message(name) {}
};
struct LockedByOtherThread {
    std::string message;
    LockedByOtherThread(std::string name) : message(name) {}
};
struct NotLocked {
    std::string message;
    NotLocked(std::string name) : message(name) {}
};

class ErrorCheckingMutex 
{
public:
    ErrorCheckingMutex(){ value_ = 1; 
        //std::cout << "constructor called" <<std::endl; 
    }

    void lock() {
        std::lock_guard<std::mutex> guard(mutex_);
        std::thread::id current_id = std::this_thread::get_id();
        if (value_ == 0) {
            if(this_id == current_id) {
                throw RecursiveLock("throw resursive lock");
            } else {
                throw LockedByOtherThread("throw locked by other thread");
            }
        } else {
            this_id = current_id;
            //std::cout << "value_ before minus " << value_ << std::endl;
            value_--;
        }
    }

    int try_lock() {
        std::lock_guard<std::mutex> guard(mutex_);
        std::thread::id current_id = std::this_thread::get_id();
        if (value_ == 0) {
            if(this_id == current_id) {
                throw RecursiveLock("throw resursive lock");
            } else {
                return 0;
                //throw LockedByOtherThread("throw locked by other thread");
            }
        } else {
            this_id = current_id;
            //std::cout << "value_ before minus " << value_ << std::endl;
            value_--;
        }
       return 1;
    }

    void unlock() {
        std::lock_guard<std::mutex> guard(mutex_);
        std::thread::id current_id = std::this_thread::get_id();
        //printf(stderr "value_ is %d, this_id: %ld:%ld\n", value_, this_id,
                //current_id);
        //std::cout << "value_ is " << value_ << " " << this_id << " " << current_id << std::endl;
        if ((value_ == 1))
            throw NotLocked("throw not locked yet");
        if (this_id != current_id)
            throw LockedByOtherThread("locked by other thread");
        //std::cout << "value_ before plus" << value_ << std::endl;
        value_++;
    }

private:
    std::thread::id this_id;
    std::mutex mutex_;
    int value_;
};
}
#endif
