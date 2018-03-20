#pragma once

#include <iterator>

#include <cstdlib> //malloc, realloc
#include <exception> //bad_alloc
#include <utility>

#include <cassert>

namespace tyti {
template<typename T>
class any_iterator_virtual : std::iterator<std::bidirectional_iterator_tag, T>
{
    struct AbstractIterHolder
    {
        virtual ~AbstractIterHolder() {};
        virtual void inc() = 0;
        virtual void dec() = 0;
        virtual T* deref() = 0;
        virtual bool equal_holder(const void* in) const = 0;
        virtual bool equal(const void* in) const = 0;

        virtual void copy_and_assign_to(void* dst) const = 0;
        virtual size_t holder_size() = 0;
    };

    template<typename IterT>
    struct IterHolder : public AbstractIterHolder
    {
    private:
        IterT iter_;
    public:
        IterHolder(IterT t):iter_{std::move(t)}{}
        ~IterHolder(){}
        void inc() { ++iter_; }
        void dec() { --iter_; }
        T* deref() { return &(*iter_); }
        bool equal_holder(const void* in) const { return iter_ == reinterpret_cast<const IterHolder<IterT>*>(in)->iter_; }
        bool equal(const void* in) const { return iter_ == *reinterpret_cast<const IterT*>(in); }
        void copy_and_assign_to(void* dst) const { new (dst) IterHolder(iter_); }
        size_t holder_size() { return sizeof(IterHolder<IterT>); }
    };

    // helper functions
    inline void destruct()
    {
        ptr_->~AbstractIterHolder();
    }

    void check_allocation(size_t oldsize, size_t newsize)
    {
        if (oldsize < newsize)
        {
            ptr_ = reinterpret_cast<AbstractIterHolder*>(std::realloc(ptr_, newsize));
            if (!ptr_)
                throw std::bad_alloc();
        }
    }

    // member variables
    AbstractIterHolder* ptr_;

    /// Interface
public:
    template <typename IterType, class = typename std::enable_if<!std::is_rvalue_reference<IterType>::value>::type>
    explicit any_iterator_virtual(IterType _iter) 
        : ptr_(reinterpret_cast<AbstractIterHolder*>(std::malloc(sizeof(IterHolder<IterType>))))
    {
        if (!ptr_)
            throw std::bad_alloc();
        new (ptr_) IterHolder<IterType>(std::move(_iter));
    }

    any_iterator_virtual(const any_iterator_virtual& _iter)
        : ptr_(reinterpret_cast<AbstractIterHolder*>(std::malloc(_iter.ptr_->holder_size())))
    {
        if (!ptr_)
            throw std::bad_alloc();
        _iter.ptr_->copy_and_assign_to(ptr_);
    }

    any_iterator_virtual(any_iterator_virtual&& _iter)
        :ptr_(std::move(_iter.ptr_))
    {
        _iter.ptr_ = nullptr;
    }

    template <typename IterType, class = typename std::enable_if<!std::is_rvalue_reference<IterType>::value>::type >
    const any_iterator_virtual& operator=(IterType _iter)
    {
        const size_t old_size = ptr_->holder_size();
        destruct();
        check_allocation(old_size, sizeof(IterHolder<IterType>));
        new (ptr_) IterHolder<IterType>(std::move(_iter));
        return *this;
    }

    const any_iterator_virtual& operator=(const any_iterator_virtual& _iter)
    {
        const size_t old_size = ptr_->holder_size();
        destruct();
        check_allocation(old_size, _iter.ptr_->holder_size());
        _iter.ptr_->copy_and_assign_to(ptr_);
        return *this;
    }

    const any_iterator_virtual& operator=(any_iterator_virtual&& _iter)
    {
        std::swap(ptr_, _iter.ptr_);
        //old ptr gets destructed via _iter
        return *this;
    }

    ~any_iterator_virtual() 
    {
        if (ptr_)
        {
            destruct();
            std::free(ptr_);
        }
    }

    bool operator==(const any_iterator_virtual& _rhs) const
    {
        if (typeid(*ptr_) != typeid(*_rhs.ptr_)) //different types
            return false;
        return ptr_->equal_holder(_rhs.ptr_);
    }

    template <typename IterType>
    bool operator==(const IterType& _rhs) const {
        return ptr_->equal(reinterpret_cast<const void*>(&_rhs));
    }

    template <typename IterType>
    bool operator!=(const IterType& _rhs) const{
        return !operator==(_rhs);
    }

    /// Standard pre-increment operator
    any_iterator_virtual& operator++() {
        ptr_->inc();
        return *this;
    }

    /// Standard post-increment operator
    any_iterator_virtual operator++(int) {
        any_iterator_virtual cpy(*this);
        ptr_->inc();
        return cpy;
    }

    /// Standard pre-decrement operator
    any_iterator_virtual& operator--() {
        ptr_->dec();
        return *this;
    }

    /// Standard post-decrement operator
    any_iterator_virtual operator--(int) {
        any_iterator_virtual cpy(*this);
        ptr_->dec();
        return cpy;
    }

    const T& operator*() const {
        return *ptr_->deref();
    }

    T& operator*() {
        return *ptr_->deref();
    }

    /// Standard pointer operator.
    const T* operator->() const {
        return ptr_->deref();
    }

    T* operator->() {
        return ptr_->deref();
    }
};

} // end namespace tyti

template<typename IterT, typename T>
bool operator==(const IterT&& _lhs, const tyti::any_iterator_virtual<T>&& _rhs)
{
    return _rhs.operator==(std::forward<IterT>(_lhs));
}

template<typename IterT, typename T>
bool operator!=(IterT&& _lhs, tyti::any_iterator_virtual<T>&& _rhs)
{
    return _rhs.operator!=(std::forward<IterT>(_lhs));
}