
#include "map.hpp"

#include <cassert>

// basically an int wrapper
class MyKeyType {
    private:
        int val;

    public:
        //not default constructable, not copy assignable, not move assignable
        MyKeyType() = delete;
        MyKeyType& operator=(const MyKeyType&) = delete;
        MyKeyType& operator=(MyKeyType&&) = delete;

        // copy constructable and move assignable
        MyKeyType(MyKeyType&&) = default;
        MyKeyType(const MyKeyType&) = default;
        ~MyKeyType() = default;

        MyKeyType(int i) : val(i) { }

        bool operator<(const MyKeyType& other) const { 
            return this->val < other.val;
        }

        bool operator==(const MyKeyType &other) const {
            return this->val == other.val;
        }
};

// same as keytype except no operator<
class MyValueType {
    private:
        int val;

    public:
        //not default constructable, not copy assignable, not move assignable
        MyValueType() = delete;
        MyValueType& operator=(const MyValueType&) = delete;
        MyValueType& operator=(MyValueType&&) = delete;

        // copy constructable and move assignable
        MyValueType(MyValueType&&) = default;
        MyValueType(const MyValueType&) = default;
        ~MyValueType() = default;

        MyValueType(int i) : val(i) { }

        bool operator==(const MyValueType &other) const {
            return this->val == other.val;
        }
};

class MyDefaultConstructable {
    private:
        int val = 0;

    public:
        // not copy assignable, not move assignable
        MyDefaultConstructable& operator=(const MyDefaultConstructable&) = delete;
        MyDefaultConstructable& operator=(MyDefaultConstructable&&) = delete;

        // default constructable, copy constructable and move assignable
        MyDefaultConstructable() = default;
        MyDefaultConstructable(MyDefaultConstructable&&) = default;
        MyDefaultConstructable(const MyDefaultConstructable&) = default;
        ~MyDefaultConstructable() = default;

        MyDefaultConstructable(int i) : val(i) { }

        bool operator==(const MyDefaultConstructable &other) const {
            return this->val == other.val;
        }
};

class MyAssignable {
    private:
        int val = 0;

    public:
        MyAssignable() = default;
        MyAssignable(int i) : val(i) { }
        bool operator==(const MyAssignable &other) const {
            return this->val == other.val;
        }
};

int main() {
    cs540::Map<MyKeyType, MyValueType> m{{3, 5}};
    m.insert({{2}, {3}});
    m.insert({{1}, {3}});
    m.insert({{5}, {3}});
    m.insert({{7}, {3}});
    m.at(2);
    m.find(2);
    auto m_copy = m;
    assert(m_copy == m);

    cs540::Map<MyKeyType, MyDefaultConstructable> m2{{8, 9}};
    m2[10]; // should default construct these values
    m2[15];

    cs540::Map<MyKeyType, MyAssignable> m3{{6, 7}};
    m3[20] = {5}; // move assign
    MyAssignable ma{1};
    m3[10] = ma; //copy assign

    return 0;
}
