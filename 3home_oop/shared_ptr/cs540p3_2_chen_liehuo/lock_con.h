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
