#ifndef LH_UPTR_
#define LH_UPTR_
#include <tuple>
#include <iostream>
#include <utility>
#include <memory>

namespace cs540 {

struct Helper {
    Helper() {};
    virtual ~Helper() {};
    virtual Helper *clone() const = 0;
};

template <typename U>
struct HelperDerived : public Helper {
    HelperDerived(U *ptr) { pointee_ = ptr; }
    virtual ~HelperDerived() {
        delete pointee_;
        pointee_ = 0;
    }
    virtual Helper *clone() const {
        return new HelperDerived<U>(pointee_);
    }
protected:
    U *pointee_;
};

template <typename T>
class UniquePtr
{
public:
    typedef T*  pointer;
    typedef T   element_type;

    UniquePtr() : pointee_(nullptr), helpConversion_(nullptr) {
    }

    template <typename U> 
    explicit UniquePtr(U *p) : pointee_(p) {
       helpConversion_ = new HelperDerived<U>(p);
    }

    // Move constructors
    UniquePtr(UniquePtr &&u)
    {
        pointee_ = u.pointee_;
        helpConversion_ = u.helpConversion_;
        u.helpConversion_ = nullptr;
        u.pointee_ = nullptr;
    }

  template <typename U> 
  UniquePtr(UniquePtr<U> &&u) {
      pointee_ = u.pointee_;
      helpConversion_ = u.helpConversion_;
      u.helpConversion_ = nullptr;
      u.pointee_ = nullptr;
  }

    // Destructor
    ~UniquePtr() { 
        delete helpConversion_; 
        pointee_ = nullptr;
        helpConversion_ = nullptr;
    }

    pointer get() const {
        return pointee_;
    }

    template <typename U> 
    void reset(U *up)
    {
        if (up != get()) {
            this->~UniquePtr();
            pointee_ = up;
            helpConversion_ = new HelperDerived<U>(up);
        }
    }

    void reset(std::nullptr_t)
    {
        if (get() != nullptr) {
            this->~UniquePtr();
            pointee_ = nullptr;
            helpConversion_ = nullptr;
        }
    }

    // Move assignment
    UniquePtr& operator= (UniquePtr &&rhs) {
        this->~UniquePtr();
        pointee_ = rhs.pointee_;
        helpConversion_ = rhs.helpConversion_;
        rhs.pointee_ = nullptr;
        rhs.helpConversion_ = nullptr;
        return *this;
    }

    template <typename U>
    UniquePtr& operator= (UniquePtr<U> &&rhs) {
        this->~UniquePtr();
        pointee_ = rhs.pointee_;
        helpConversion_ = rhs.helpConversion_;
        rhs.pointee_ = nullptr;
        rhs.helpConversion_ = nullptr;
        return *this;
    }

    typename std::add_lvalue_reference<element_type>::type operator*() const {
        return *get();
    }

    pointer operator->() const {
        return get();
    }

    explicit operator bool() const {
        return get() == nullptr ? false : true;
    }

    // Disable copy from lvalue
    UniquePtr(const UniquePtr&) = delete;

    template <typename U>
    UniquePtr(const UniquePtr<U>&) = delete;

    UniquePtr& operator=(const UniquePtr&) = delete;

    template <typename U>
    UniquePtr& operator= (const UniquePtr<U>&) = delete;

private:
        T   *pointee_;
        Helper *helpConversion_;
        template <typename U> friend class UniquePtr;
};

template <typename T1, typename T2>
inline bool operator==(const UniquePtr<T1> &x, const UniquePtr<T2> &y) {
    return x.get() == y.get();
}

template <typename T1>
inline bool operator==(const UniquePtr<T1> &x, std::nullptr_t) {
    return x.get() == nullptr;

}

template <typename T1>
inline bool operator==(std::nullptr_t, const UniquePtr<T1> &x) {
    return x.get() == nullptr;

}

template <typename T1, typename T2>
inline bool operator!=(const UniquePtr<T1> &x, const UniquePtr<T2> &y) {
    return !(x == y);
}

template <typename T1>
inline bool operator!=(const UniquePtr<T1> &x, std::nullptr_t) {
    return !(x == nullptr);

}

template <typename T1>
inline bool operator!=(std::nullptr_t, const UniquePtr<T1> &x) {
    return !(x == nullptr);
}


}
#endif
