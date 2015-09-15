#ifndef LH_ARRAY
#define LH_ARRAY

#include <iterator>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
using std::size_t;

namespace cs540 {
    struct OutOfRange : public std::out_of_range {
        OutOfRange(std::string const &msg) : std::out_of_range(msg) {}
    };

    size_t Dhelper(size_t, size_t){}

    template <typename... Dims>
    size_t Dhelper(size_t dimension, size_t count, size_t currD, Dims... leftD)
    {
        return (dimension == count) \
            ? currD : Dhelper(dimension, ++count, leftD...);
    }

    template <typename T, std::size_t... leftD>
    class Array {
        Array &operator= (Array &rhs) {
        }
    };

    template <typename T, std::size_t CD>
    class Array<T, CD> {
    public:
        typedef T type[CD];
        type data;
        T& operator[] (size_t i) { return data[i]; }
        const T& operator[] (size_t i) const { return data[i]; }
        static const size_t length = CD;
        constexpr size_t size() { return length; }

        //T *data() { return data; }
        //const T *data() const { return data; }
    };

    template <typename T, std::size_t CD, std::size_t... leftD>
    class Array<T, CD, leftD...> {
    public:
        typedef T ValueType;
        typedef T* Iterator;
        typedef const T* ConstIterator;
        typedef T& Reference;
        typedef const T& ConstReference;
        typedef std::reverse_iterator<Iterator> ReverseIterator;
        typedef std::reverse_iterator<ConstReference> ConstReverseIterator;
        typedef size_t size_type;

        //using SubArray = typename Array<T, LeftD...>::type;
        typedef typename Array<T, leftD...>::type SubArray;
        typedef SubArray type[CD];
        type    data;

        /*
        Array<T, CD, leftD...> &operator=(Array<T, CD, leftD...> &rhs)
        {
            if (this == &rhs)
                return *this;
            for (int i = 0; i < CD; i++)
            {
                data[i] = rhs.data[i];
            }
            return *this;
        }
        */

        SubArray &operator[] (size_type i) { return data[i]; }
        const SubArray &operator[] (size_type i) const { return data[i]; }
        
        static_assert(CD != 0, "The dimensions of Array <= 0\n");
        constexpr size_type numOfDims() { return sizeof...(leftD) + 1; }

        static const size_type length = CD * Array<T, leftD...>::length;
        constexpr size_type size() { return length; }
        //constexpr size_type max_size() { return length; }

        size_type dSize (size_type dim) {
            return Dhelper(dim, 0, CD, leftD...);
        }

        T *aData() { return reinterpret_cast<T*>(data); }
        const T *aData() const { return reinterpret_cast<const T*>(data); }

        Iterator begin() { return reinterpret_cast<T*>(data); }
        Iterator end() { return begin() + size(); }
        /*
        ConstIterator begin() const { return reinterpret_cast<const T*>(data); }
        ConstIterator cbegin() const { return reinterpret_cast<const T*>(data); }
        */
    };
}

#endif
