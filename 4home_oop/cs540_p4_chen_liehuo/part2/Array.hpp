/****
 * The idea of recursive the operator[] comes from the following code:
 *  http://www.drdobbs.com/a-class-template-for-n-dimensional-gener/184401319?pgno=7
 ***/
#ifndef LH_ARRAY
#define LH_ARRAY
#include<cstddef>
#include<iostream>
#include<vector>
#include<cassert>
#include<stdexcept>
#include<string>
namespace cs540 {

template <typename T, size_t... Dims> class Array;

template <size_t N1>
void check_dimension()
{
    static_assert(N1 > 0, "Array dimensions should greater than 0\n");
}

template <size_t N1, size_t N2, size_t... args>
void check_dimension()
{
    static_assert(N1 > 0, "Array dimensions should greater than 0\n");
    check_dimension<N2,args...>();
}

template <typename T, size_t... Dims> 
class LastIterator
{
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef LastIterator<T, Dims...> self;
    static constexpr size_t dimensions[] = {Dims...};
    static constexpr int N = sizeof...(Dims);
    size_t subArrayEle[N];
    size_t indexArray[N];
    int total_num; 
    int last_it;
     
    public:
    pointer node;
    pointer starts;
    
    LastIterator() :  total_num(1), last_it(0),
        node(nullptr), starts(nullptr)
    {
        for (int i = 0; i < N; i++)
        {
           total_num *= dimensions[i];
           subArrayEle[i] = 1;
           indexArray[i] = 0;
           for (int j = N-1; j > i; j--)
           {
               subArrayEle[i] *= dimensions[j];
           }
        }
    }

    LastIterator(const LastIterator& rhs)
        :  total_num(rhs.total_num), last_it(rhs.last_it),
        node(rhs.node), starts(rhs.starts) {
        for (int i = 0; i < N; i++)
        {
           subArrayEle[i] = rhs.subArrayEle[i];
           indexArray[i] = rhs.indexArray[i];
        }
   }
   LastIterator &
       operator=(const LastIterator &rhs) 
   {
        node = rhs.node;
        starts = rhs.starts;
        total_num = rhs.total_num;
        last_it = rhs.last_it;
        for (int i = 0; i < N; i++)
        {
           subArrayEle[i] = rhs.subArrayEle[i];
           indexArray[i] = rhs.indexArray[i];
        }
        return *this;
   }
   LastIterator &operator++()
   {
       if(last_it == (total_num - 1))
       {
           node++;
           return *this;
       }
       for (int i = 0; i < N; i++)
       {
           if (indexArray[i] == (dimensions[i] - 1))
           {
               indexArray[i] = 0;
           }
           else 
           {
               indexArray[i]++;
               break;
           }
       }
       if(last_it == total_num -1) node++;
       int index = 0;
       for (int j = 0; j < N; j++)
       {
           index += indexArray[j] * subArrayEle[j];
       }
       if(index < total_num)
       {
           node = starts + index;
           last_it = index;
       }
       return *this;
   }

   LastIterator operator++(int)
   {
      LastIterator orign = *this;
      ++(*this); 
      return orign;
   }
   reference operator*() const
   {
       return *node;
   }

};

template<typename T, size_t... Dims>
bool operator==(const LastIterator<T,Dims...> &lhs,
    const LastIterator<T,Dims...> &rhs)
{
   return (lhs.node == rhs.node);
}

template<typename T, size_t... Dims>
bool operator!=(const LastIterator<T,Dims...> &lhs,
    const LastIterator<T,Dims...> &rhs)
{
   return !(lhs == rhs);
}

template<typename T, size_t... Dims>
constexpr size_t LastIterator<T, Dims...>::dimensions[];


struct OutOfRange : public std::out_of_range
{
    OutOfRange(std::string const& msg) : std::out_of_range(msg) {}
};

template<typename T, size_t N>
class helpArray
{
    typedef T ValueType;
    typedef T* PointType;
    PointType elements;
    const size_t * dimensions;
    const size_t * subArrayEle;

    helpArray(T *pEle, const size_t *pDim, const size_t *pSub) : 
        elements(pEle), dimensions(pDim), subArrayEle(pSub)
    {}
    
    public:
    helpArray<T, N-1> operator[] (size_t i)
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return helpArray<T, N-1>(&elements[i*subArrayEle[0]],
                dimensions + 1, subArrayEle + 1);
    }
    const helpArray<T, N-1> operator[] (size_t i) const
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return helpArray<T, N-1>(&elements[i*subArrayEle[0]],
                dimensions + 1, subArrayEle + 1);
    }

    template <typename U, size_t... Dim> friend class Array; 
    friend class helpArray<T, N+1>;

};

template <typename T>
class helpArray<T, 1>
{
    typedef T ValueType;
    typedef T* PointType;
    PointType elements;
    const size_t * dimensions;
    const size_t * subArrayEle;

    helpArray(T *pEle, const size_t *pDim,  const size_t *pSub) : 
        elements(pEle), dimensions(pDim), subArrayEle(pSub) {}
    public:
    T& operator[] (size_t i)
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return elements[i];
    }
    const T& operator[] (size_t i) const 
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return elements[i];
    }
    
    template <typename U, size_t... Dim> friend class Array; 
    friend class helpArray<T, 2>;
    
};


template <typename T, size_t... Dims> 
class Array
{
    public:
    typedef  T ValueType;
    typedef  LastIterator<T, Dims...>  LastDimensionMajorIterator;
    private:
    static constexpr size_t dimensions[] = {Dims...}; 
    static constexpr int N = sizeof...(Dims);
    int total_num;
    size_t *subArrayEle;

    ValueType *elements;
    template <typename U, size_t... Dim> friend class Array; 

    public:
    Array() : total_num(1)
    {
        static_assert(N > 0, "Array dimensions should greater than 0\n");
        check_dimension<Dims...>();
        subArrayEle = new size_t[N];
        assert(subArrayEle);
        for (int i = 0; i < N; i++)
        {
           total_num *= dimensions[i]; 
           subArrayEle[i] = 1;
           for (int j = N-1; j > i; j--)
           {
               subArrayEle[i] *= dimensions[j];
           }
        }
       elements = new T[total_num];
       assert(elements);
    }
    ~Array()
    {
        delete elements;
        delete [] subArrayEle;
    }

    Array(const Array &rhs)     
    {
        total_num = rhs.total_num; 
        elements = new T[total_num];
        subArrayEle = new size_t[N];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
    }

    Array & operator= (const Array &rhs)
    {
        if(this == &rhs)
            return *this;
        total_num = rhs.total_num;
        subArrayEle = new size_t[N];
        elements = new T[total_num];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
        return *this;
    }

    template <typename U>
    Array(const Array<U, Dims...> &rhs)
    {
        total_num = rhs.total_num;
        subArrayEle = new size_t[N];
        elements = new T[total_num];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
    }

    template <typename U>
    Array & operator= (const Array<U, Dims...> &rhs)
    {
        
        if((void *)this == (void *)&rhs)
            return *this;
        total_num = rhs.total_num;
        subArrayEle = new size_t[N];
        elements = new T[total_num];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
        return *this;
    }

    helpArray<T, N-1>
    operator[] (size_t i)
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return helpArray<T, N-1>(&elements[i*subArrayEle[0]],
                dimensions + 1, subArrayEle + 1);
    }
    

    const helpArray<T, N-1>
    operator[] (size_t i) const 
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return helpArray<T, N-1>(&elements[i*subArrayEle[0]],
                dimensions + 1, subArrayEle + 1);
    }


    class FirstDimensionMajorIterator
    {

        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef FirstDimensionMajorIterator self;
        
        public:
        pointer node;
       // size_t dimensions[] = {Dims...};
        FirstDimensionMajorIterator() : node(nullptr)  {}
        FirstDimensionMajorIterator(const FirstDimensionMajorIterator& rhs)
            : node(rhs.node) {}
       FirstDimensionMajorIterator &
           operator=(const FirstDimensionMajorIterator &rhs) 
       {
            node = rhs.node;
            return *this;
       }
       FirstDimensionMajorIterator &operator++()
       {
           node++;
           return *this;
       }
       FirstDimensionMajorIterator operator++(int)
       {
          FirstDimensionMajorIterator orign = *this;
          ++(*this); 
          return orign;
       }
       reference operator*() const
       {
           return *node;
       }

        friend bool operator==(const Array<T,Dims...>::FirstDimensionMajorIterator &lhs,
            const Array<T,Dims...>::FirstDimensionMajorIterator &rhs)
        {
          // std::cout << "lhs.node is " << lhs.node << std::endl;
          // std::cout << "rhs.node is " << rhs.node << std::endl;
           return (lhs.node == rhs.node);
        }

        friend bool operator!= (const Array<T,Dims...>::FirstDimensionMajorIterator &lhs,
            const Array<T,Dims...>::FirstDimensionMajorIterator &rhs)
        {
           return !(lhs == rhs);
        }

    };


    FirstDimensionMajorIterator fmbegin()
    {
        FirstDimensionMajorIterator begin;
        begin.node = elements;
        return begin;
    }
    FirstDimensionMajorIterator fmend()
    {
        FirstDimensionMajorIterator end;
        end.node = &elements[total_num];
        return end;
    }


    FirstDimensionMajorIterator fmbegin() const
    {
        FirstDimensionMajorIterator begin;
        begin.node = elements;
        return begin;
    }
    FirstDimensionMajorIterator fmend() const
    {
        FirstDimensionMajorIterator end;
        end.node = &elements[total_num];
        return end;
    }
    LastDimensionMajorIterator lmbegin()
    {
        LastDimensionMajorIterator begin;
        begin.node = elements;
        begin.starts = elements;
        return begin;
    }
    LastDimensionMajorIterator lmend()
    {
        LastDimensionMajorIterator end;
        end.node = &elements[total_num];
        end.starts = elements;
        return end;
    }

};


template <typename T, size_t... Dims>
constexpr size_t Array<T,Dims...>::dimensions[];


template <typename T, size_t D1> 
class Array<T, D1>
{
    public:
    typedef  T ValueType;
    static constexpr size_t dimensions[] = {D1}; 
    static constexpr int N = 1;
    int total_num;
    size_t *subArrayEle;

    ValueType *elements;
    typedef  LastIterator<T, D1>  LastDimensionMajorIterator;
    Array() : total_num(1)
    {
        static_assert(N > 0, "Array dimensions should greater than 0\n");
        check_dimension<D1>(); 
        subArrayEle = new size_t[N];
        assert(subArrayEle);
        for (int i = 0; i < N; i++)
        {
           total_num *= dimensions[i]; 
           subArrayEle[i] = 1;
           for (int j = N-1; j > i; j--)
           {
               subArrayEle[i] *= dimensions[j];
           }
        }
       elements = new T[total_num];
       assert(elements);
    }
    ~Array()
    {
        delete elements;
        delete [] subArrayEle;
    }

    Array(const Array &rhs)     
    {
        total_num = rhs.total_num; 
        elements = new T[total_num];
        subArrayEle = new size_t[N];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
    }

    Array & operator= (const Array &rhs)
    {
        if(this == &rhs)
            return *this;
        total_num = rhs.total_num;
        subArrayEle = new size_t[N];
        elements = new T[total_num];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
        return *this;
    }

    template <typename U>
    Array(const Array<U, D1> &rhs)
    {
        total_num = rhs.total_num;
        subArrayEle = new size_t[N];
        elements = new T[total_num];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
    }

    template <typename U>
    Array & operator= (const Array<U, D1> &rhs)
    {
        
        if((void *)this == (void *)&rhs)
            return *this;
        total_num = rhs.total_num;
        subArrayEle = new size_t[N];
        elements = new T[total_num];
        for (int i = 0; i < N; i++)
        {
            subArrayEle[i] = rhs.subArrayEle[i];
        }
        for (int i = 0; i < total_num; i++)
        {
            elements[i] = rhs.elements[i];
        }
        return *this;
    }

    T& 
    operator[] (size_t i)
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return elements[i];
    }
    

    const T& 
    operator[] (size_t i) const 
    {
        if(i >= dimensions[0])
            throw OutOfRange("array access out of range \n");
        return elements[i];
    }


    class FirstDimensionMajorIterator
    {

        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef FirstDimensionMajorIterator self;
        
        public:
        pointer node;
       // size_t dimensions[] = {Dims...};
        FirstDimensionMajorIterator() : node(nullptr)  {}
        FirstDimensionMajorIterator(const FirstDimensionMajorIterator& rhs)
            : node(rhs.node) {}
       FirstDimensionMajorIterator &
           operator=(const FirstDimensionMajorIterator &rhs) 
       {
            node = rhs.node;
            return *this;
       }
       FirstDimensionMajorIterator &operator++()
       {
           node++;
           return *this;
       }
       FirstDimensionMajorIterator operator++(int)
       {
          FirstDimensionMajorIterator orign = *this;
          ++(*this); 
          return orign;
       }
       reference operator*() const
       {
           return *node;
       }

        friend bool operator==(const Array<T,D1>::FirstDimensionMajorIterator &lhs,
            const Array<T,D1>::FirstDimensionMajorIterator &rhs)
        {
          // std::cout << "lhs.node is " << lhs.node << std::endl;
          // std::cout << "rhs.node is " << rhs.node << std::endl;
           return (lhs.node == rhs.node);
        }

        friend bool operator!= (const Array<T,D1>::FirstDimensionMajorIterator &lhs,
            const Array<T,D1>::FirstDimensionMajorIterator &rhs)
        {
           return !(lhs == rhs);
        }

    };

    FirstDimensionMajorIterator fmbegin()
    {
        FirstDimensionMajorIterator begin;
        begin.node = elements;
        return begin;
    }
    FirstDimensionMajorIterator fmend()
    {
        FirstDimensionMajorIterator end;
        end.node = &elements[total_num];
        return end;
    }
    LastDimensionMajorIterator lmbegin()
    {
        LastDimensionMajorIterator begin;
        begin.node = elements;
        begin.starts = elements;
        return begin;
    }
    LastDimensionMajorIterator lmend()
    {
        LastDimensionMajorIterator end;
        end.node = &elements[total_num];
        end.starts = elements;
        return end;
    }
};

}
#endif
