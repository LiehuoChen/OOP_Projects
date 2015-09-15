#ifndef LH_SHARED_PTR
#define LH_SHARED_PTR

#include <cassert>
#include <iostream>
#include <cstdio>
#include <functional>
#include <cstddef>

//using namespace std;
namespace cs540 {

struct Helper {
	Helper() {}; 
	virtual ~Helper() {};
};

class atomic_count
{
private:

    class scoped_lock
    {
    public:

        scoped_lock(pthread_mutex_t & m): m_(m)
        {
            pthread_mutex_lock(&m_);
        }

        ~scoped_lock()
        {
            pthread_mutex_unlock(&m_);
        }

    private:

        pthread_mutex_t & m_;
    };

public:

    explicit atomic_count(long v): value_(v)
    {
        pthread_mutex_init(&mutex_, 0);
    }

    ~atomic_count()
    {
        pthread_mutex_destroy(&mutex_);
    }

    long operator++()
    {
        scoped_lock lock(mutex_);
        return ++value_;
    }

    long operator--()
    {
        scoped_lock lock(mutex_);
        return --value_;
    }

    operator long() const
    {
        scoped_lock lock(mutex_);
        return value_;
    }

private:

    atomic_count(atomic_count const &);
    atomic_count & operator=(atomic_count const &);

    mutable pthread_mutex_t mutex_;
    long value_;
};


template <typename U>
struct HelperDerived : public Helper {
		HelperDerived(U *ptr) { pointee_ = ptr; } 
		virtual ~HelperDerived() {
			delete pointee_;
			pointee_ = 0;
		}
	protected:
		U *pointee_;
};

template <typename T>
class SharedPtr
{
	public:
		typedef	T*	PointerType;
		typedef	T&	ReferenceType;

	public:
		explicit 
		SharedPtr() { 
			pointee_ = 0;
			helpConversion_ = 0;
			pCount_ = 0;
		}
	
		template <typename U>
		SharedPtr(U *other) : pointee_(other) {
			helpConversion_ = new HelperDerived<U>(other);
			pCount_ = new atomic_count(1);
		}

		SharedPtr(const SharedPtr &sp) : pointee_(sp.pointee_), 
				       pCount_(sp.pCount_), 
				       helpConversion_(sp.helpConversion_) {
			if(pCount_)
				++*pCount_;
		}

		template <typename U>
		SharedPtr(const SharedPtr<U> &rhs) : pointee_(rhs.pointee_), 
					   pCount_(rhs.pCount_),
					   helpConversion_(rhs.helpConversion_) 
		{

			if (pCount_)
				++*pCount_; 
		}

        SharedPtr(SharedPtr&& sp)
        {
            //std::cout<< "move construct here" << std::endl;
            pointee_ = sp.pointee_;
            pCount_ = sp.pCount_;
            helpConversion_ = sp.helpConversion_;
            sp.pointee_ = nullptr;
            sp.pCount_ = nullptr;
            sp.helpConversion_ = nullptr;
        }

        template <typename U> SharedPtr(SharedPtr<U>&& sp)
        {

            //std::cout<< "move construct with U here" << std::endl;
            pointee_ = sp.pointee_;
            pCount_ = sp.pCount_;
            helpConversion_ = sp.helpConversion_;
            sp.pointee_ = nullptr;
            sp.pCount_ = nullptr;
            sp.helpConversion_ = nullptr;
        }

		~SharedPtr() {
			if (pCount_ && *pCount_ != 0 && !--*pCount_) { 
				delete pCount_;
				pCount_ = 0;
				delete helpConversion_;
				helpConversion_ = 0;
				pointee_ = 0;
			}
		}

		PointerType get() const{
			return pointee_;
		}
        
        template <typename U> 
        void reset(U *up)
        {
            if(up != get())
            this->~SharedPtr();
            pointee_ = up;
            helpConversion_ = new HelperDerived<U>(up);
            pCount_ = new atomic_count(1);
        }

		void reset(std::nullptr_t) {
            if(get() != nullptr)
            {
                this->~SharedPtr();
                pointee_ = nullptr;
                helpConversion_ = nullptr;
                pCount_ = nullptr;
            }
		}

        //Move assignment.
        SharedPtr& operator=(SharedPtr&& sp)
        {
            if(*this == sp) return *this;
           // std::cout<< "move assignment here" << std::endl;
            this->~SharedPtr();
            pointee_ = sp.pointee_;
            pCount_ = sp.pCount_;
            helpConversion_ = sp.helpConversion_;
            sp.pointee_ = nullptr;
            sp.pCount_ = nullptr;
            sp.helpConversion_ = nullptr;
            return *this; 
        }

        template <typename U> SharedPtr& operator=(SharedPtr<U>&& sp)
        {
            if(*this == sp) return *this;
            //std::cout<< "move assignment with U here" << std::endl;
            this->~SharedPtr();
            pointee_ = sp.pointee_;
            pCount_ = sp.pCount_;
            helpConversion_ = sp.helpConversion_;
            sp.pointee_ = nullptr;
            sp.pCount_ = nullptr;
            sp.helpConversion_ = nullptr;
            return *this; 
        }

		SharedPtr& operator= (const SharedPtr &rhs) {
			if (this != &rhs && pointee_ != rhs.pointee_) {
				this->~SharedPtr();
				pointee_ = rhs.pointee_;
                pCount_ = rhs.pCount_;
				helpConversion_ = rhs.helpConversion_;
				if(pCount_)
					++*pCount_;
			} 
			return *this;
		}

		template <typename U>
		SharedPtr& operator= (const SharedPtr<U> &rhs) {

			if (pointee_ != rhs.pointee_) {
				if (pointee_)
					this->~SharedPtr();
				pCount_ = rhs.pCount_;
				pointee_ = rhs.pointee_;
				helpConversion_ = rhs.helpConversion_;
				if(pCount_)
					++*pCount_;
			}
			return *this;
		}

		PointerType operator-> () const {
			return pointee_;
		}

		ReferenceType operator* () const {
			return *pointee_;
		}

        explicit
		operator bool() const {
			return get() == nullptr ? false : true;
		}

	private:
		T *pointee_;
		atomic_count *pCount_;
		Helper *helpConversion_;
		template <typename U> friend class SharedPtr;
};


// operator template
template <typename T1, typename T2>
bool operator== (const SharedPtr<T1> &lhs, const SharedPtr<T2> &rhs) {
	return lhs.get() == rhs.get();
}

template <typename T>
bool operator== (std::nullptr_t, const SharedPtr<T> &rhs) {
	return nullptr == rhs.get();
}

template <typename T>
bool operator== (const SharedPtr<T> &lhs, std::nullptr_t) {
	return nullptr == lhs.get();
}

template <typename T1, typename T2>
bool operator!= (const SharedPtr<T1> &lhs, const SharedPtr<T2> &rhs) {
	return !(lhs == rhs);
}

template <typename T>
bool operator!= (std::nullptr_t, const SharedPtr<T> &rhs) {
	return !(nullptr == rhs);
}

template <typename T>
bool operator!= (const SharedPtr<T> &lhs, std::nullptr_t) {
	return !(nullptr == lhs);
}
}
#endif
