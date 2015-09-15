#ifndef LH_UPTR_
#define LH_UPTR_
#include <tuple>
#include <iostream>
#include <utility>
#include <memory>

namespace cs540 {

template <typename T>
struct default_delete {
    default_delete() {};

    template <typename U>
        default_delete(const default_delete<U>&) {}

    virtual void operator()(T *ptr) const {
        static_assert(sizeof(T) > 0,
                "delete error: can't delete pointer");
       // printf("the ptr in default delete is %d\n", ptr);
       // printf("the typeid name is %s\n", typeid(T).name());
        /*
        if(ptr)
        {
            delete ptr;
        }
        */
        delete ptr;
    }
};

template <typename T, typename U>
struct derived_delete : public default_delete<T>
{
    derived_delete() {}
    virtual void operator()(U *ptr) const {
        static_assert(sizeof(U) > 0,
                "delete error: can't delete pointer");
       // printf("the ptr in default delete is %d\n", ptr);
       // printf("the typeid name is %s\n", typeid(T).name());
        /*
        if(ptr)
        {
            delete ptr;
        }
        */
        delete ptr;
    }
};

template <typename T>
struct default_delete<T[]> {
    void operator() (T* ptr) const {
        static_assert(sizeof(T) > 0, 
                "delete error: can't delete pointer");
        delete [] ptr;
    }
};


template <typename T, typename TD = default_delete<T> >
class UniquePtr
{
    typedef std::tuple<T*, TD*>          tuple_type;
    typedef tuple_type UniquePtr::*     unspecified_bool_type;
    typedef T* UniquePtr::*             unspecified_pointer_type;

public:
    typedef T*  pointer;
    typedef T   element_type;
    typedef TD*  deleter_type;

    UniquePtr() : M_t(pointer(), deleter_type()) {
        //static_assert(!std::is_pointer<deleter_type>::value,
         //       "constructed with null function pointer deleter");
    }

    template <typename U> 
    explicit UniquePtr(U *p) : M_t(p,
            /*deleter_type()*/&derived_delete<T, U>()) {
       // static_assert(!std::is_pointer<deleter_type>::value,
        //        "constructed with null function pointer deleter");
    }

    // for this 
    /*
    explicit
    UniquePtr(pointer p) : M_t(p, deleter_type()) {
        static_assert(!std::is_pointer<deleter_type>::value,
                "constructed with null function pointer deleter");
    }
    */

    /*
    explicit
    UniquePtr(pointer p,
            typename std::remove_reference<deleter_type>::type && d)
        : M_t(std::move(p), std::move(d)) {
        static_assert(!std::is_reference<deleter_type>::value,
                "rvalue deleter bound to reference"); 
    }
    */

    // Move constructors
    UniquePtr(UniquePtr &&u) 
        : M_t(u.release(), std::forward<deleter_type>(u.get_deleter())) {}

  template <typename U> 
  UniquePtr(UniquePtr<U> &&u)
        : M_t(u.release(), std::forward<deleter_type>(u.get_deleter())) {}

    // for this UD
  /*
    template<typename U, typename UD>
    UniquePtr(UniquePtr<U, UD>&& u)
    : M_t(u.release(), std::forward<deleter_type>(u.get_deleter())) {}
    */

    // Destructor
    ~UniquePtr() { reset(); }

    pointer get() const {
        return std::get<0>(M_t);
    }

    template <typename U> 
    void reset(U *up)
    {
        if (up != get()) {
            (*get_deleter())(get());
            std::get<0>(M_t) = up;
        }
        //to do for homework Replace owned resource with another pointer.
    }

    void reset(std::nullptr_t)
    {
        // to do for homework Delete the previously managed object
        if (nullptr != get()) {
            (*get_deleter())(get());
            std::get<0>(M_t) = nullptr;
        }
    }

    //for this
    void reset(pointer p = pointer()) {
        if (p != get()) {
            (*get_deleter())(get());
            std::get<0>(M_t) = p;
        }
    }

    // Move assignment
    UniquePtr& operator= (UniquePtr &&rhs) {
        reset(rhs.release());
        get_deleter() = std::move(rhs.get_deleter());
        return *this;
    }

    //for this UD
    template <typename U, typename UD>
    UniquePtr& operator= (UniquePtr<U, UD>&& rhs) {
        reset(rhs.release());
        get_deleter() = std::move(rhs.get_deleter());
        return *this;
    }

    //for this
    UniquePtr& operator= (unspecified_pointer_type) {
        reset();
        return *this;
    }

    typename std::add_lvalue_reference<element_type>::type operator*() const {
        return *get();
    }

    pointer operator->() const {
        return get();
    }

    explicit operator bool() const {
        //to do for homework Returns true if the UniquePtr owns a non-nullptr address
        return get() == nullptr ? false : true;
        //return false;
    }


    typename std::add_lvalue_reference<deleter_type>::type
    get_deleter() {
        return std::get<1>(M_t);
    }

    typename std::add_lvalue_reference<
        typename std::add_const<deleter_type>::type>::type
    get_deleter() const {
        return std::get<1>(M_t);
    }

    /*
    operator unspecified_bool_type() const {
        return get() == 0 ? 0 : &UniquePtr::M_t;
    }
    */

    pointer release() {
        pointer p = get();
        std::get<0>(M_t) = 0;
        return p;
    }


    void swap(UniquePtr&& u)
    {
        using std::swap;
        swap(M_t, u.M_t);
    }

    // Disable copy from lvalue
    UniquePtr(const UniquePtr&) = delete;

    template <typename U, typename UD>
    UniquePtr(const UniquePtr<U, UD>&) = delete;

    UniquePtr& operator=(const UniquePtr&) = delete;

    template <typename U, typename UD>
    UniquePtr& operator= (const UniquePtr<U, UD>&) = delete;

private:
        tuple_type  M_t;

};

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator==(const UniquePtr<T1, D1> &x, const UniquePtr<T2, D2> &y) {
    return x.get() == y.get();
}

template <typename T1, typename D1>
inline bool operator==(const UniquePtr<T1, D1> &x, std::nullptr_t) {
    return x.get() == nullptr;

}

template <typename T1, typename D1>
inline bool operator==(std::nullptr_t, const UniquePtr<T1, D1> &x) {
    return x.get() == nullptr;

}

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator!=(const UniquePtr<T1, D1> &x, const UniquePtr<T2, D2> &y) {
    return !(x == y);
}

template <typename T1, typename D1>
inline bool operator!=(const UniquePtr<T1, D1> &x, std::nullptr_t) {
    return !(x == nullptr);

}

template <typename T1, typename D1>
inline bool operator!=(std::nullptr_t, const UniquePtr<T1, D1> &x) {
    return !(x == nullptr);
}


}
#endif
