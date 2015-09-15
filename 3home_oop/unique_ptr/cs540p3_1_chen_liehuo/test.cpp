#include "uniqueptr.hpp"

#include <utility>
#include <iostream>
#include <string>
#include <cassert>
#define DEB 0

class Base {
    private:
        int *array;
    public:
        Base() : array{new int[10]} {
            if(DEB) std::cout<<"Base C"<<std::endl; }
        ~Base() {
            delete [] this->array;
            if(DEB) std::cout<<"Base D"<<std::endl; }
};

class Derived : public Base {
    private:
        int *another_array;
    public:
        Derived() : another_array{new int[20]} {
            if(DEB) std::cout<<"Derived C"<<std::endl; }
        
        ~Derived() {
            delete [] this->another_array;
            if(DEB) std::cout<<"Derived D"<<std::endl; }
};

class Derived2 : public Derived {
    private:
        int *another_array2;
    public:
        Derived2() : another_array2{new int[20]} {
            if(DEB) std::cout<<"Derived2 C"<<std::endl; }
        
        ~Derived2() {
            delete [] this->another_array2;
            if(DEB) std::cout<<"Derived2 D"<<std::endl; }
};

class Person {
    private:
        std::string name;
        int age;
    public:
        Person(std::string n, int a)
            : name{n},
            age{a}
        { }

        void say_hi() const {
            std::cout << "Hello my name is " << this->name
                      << " and I am " << this->age << " years old\n";
        }
};


// instantiate all member functions
template class cs540::UniquePtr<Base>;

int main() {
    int *i = nullptr;
    delete i;
    cs540::UniquePtr<Base> uptr{new Derived};
    cs540::UniquePtr<Base> uptr2{std::move(uptr)};
    assert(uptr2 != nullptr);
    uptr = std::move(uptr2);


    // shouldn't compile:
    // cs540::UniquePtr<Base> badptr1{uptr}; // copy construct
    //
    // shouldn't compile:
    // cs540::UniquePtr<Base> badptr2;
    // badptr2 = uptr; // copy assign

		cs540::UniquePtr<Base> bp{new Base}, dp{new Derived}, dp22{new Derived2};
        if(DEB) std::cout<< "Derived to BP here" << std::endl;
		{
			cs540::UniquePtr<Base> mp{std::move(bp)};
            if(DEB) std::cout<< "move bp to bp here" << std::endl;
			mp = std::move(dp);
            if(DEB) std::cout<< "move dp to bp here" << std::endl;
            mp = std::move(dp22);
            if(DEB) std::cout<< "move dp22 to bp here" << std::endl;
		}
		bp.reset(new Base);
        if(DEB) std::cout<< " bp.reset Based  here" << std::endl;
		dp.reset(new Derived);
        if(DEB) std::cout<< " dp.reset Based  here" << std::endl;
		cs540::UniquePtr<Base> dp2{new Derived};
        if(DEB) std::cout<< " new Derived to DP here" << std::endl;
    cs540::UniquePtr<Base> uptr3{new Derived};
    uptr3.reset(new Base);
    uptr3.reset(new Derived);
    uptr3.reset(nullptr);
    assert(uptr3 == nullptr);


    // * and ->
    cs540::UniquePtr<Person> personp{new Person{"John", 30}};
    (*personp).say_hi();
    personp->say_hi();

    cs540::UniquePtr<int> ip{new int{}};
    *ip = 99;
    std::cout << *ip << '\n'; // should print 99

    static_assert(!std::is_convertible<cs540::UniquePtr<Person>, bool>{}, "");
    return 0;
}
