#pragma once

#include <iterator>

#include <cstdlib> //malloc, realloc
#include <cstring> //memcpy
#include <exception> //bad_alloc

namespace tyti {
template<typename T>
class any_iterator : std::iterator<std::bidirectional_iterator_tag, T>
{
    //functionpointer save structure
    struct TypeInfos
    {
        void(*inc_fn)(void*);
        void(*dec_fn)(void*);
        const bool(*equal_fn)(const void*,const void*);
        const T*(*deref_fn)(const void*);
        size_t size;
    };

    template<typename IterType>
    TypeInfos* getFunctionInfos()
    {
        static TypeInfos ti =
        {
            any_iterator::inc<IterType>,
            any_iterator::dec<IterType>,
            any_iterator::equal<IterType>,
            any_iterator::deref<IterType>,
            sizeof(IterType)
        };
        return &ti;
    }

    //access functions
    template<typename Iter>
    static void inc(void* _ptr)
    {
        ++(*reinterpret_cast<Iter*>(_ptr));
    }

    template<typename Iter>
    static void dec(void* _ptr)
    {
        --(*reinterpret_cast<Iter*>(_ptr));
    }

    template<typename Iter>
    static const T* deref(const void* _ptr)
    {
        return &(*(*reinterpret_cast<const Iter*>(_ptr)));
    }

    template<typename Iter>
    static const bool equal(const void* _lhs, const void* _rhs)
    {
        return *reinterpret_cast<const Iter*>(_lhs) == *reinterpret_cast<const Iter*>(_rhs);
    }

    // helper functions
    static void assign(void** dst_, size_t dst_size_, const void* src_, size_t src_size_)
    {
        if (dst_size_ < src_size_)
        {
            *dst_ = std::realloc(*dst_, src_size_);
            if (!*dst_)
                throw std::bad_alloc();
        }
        std::memcpy(*dst_, src_, src_size_);
    }

    // member variables
    void* ptr_;
    TypeInfos* ti_;

    /// Interface
public:
    template <typename IterType>
    explicit any_iterator(const IterType& _iter) 
        : ptr_(std::malloc(sizeof(IterType))), ti_(getFunctionInfos<IterType>())
    {
        if (!ptr_)
            throw std::bad_alloc();
        std::memcpy(ptr_, &_iter, sizeof(IterType));
    }

    any_iterator(const any_iterator& _iter)
        : ptr_(std::malloc(_iter.ti_->size)), ti_(_iter.ti_)
    {
        if (!ptr_)
            throw std::bad_alloc();
        std::memcpy(ptr_, _iter.ptr_, ti_->size);
    }

    template <typename IterType>
    any_iterator operator=(const IterType& _iter)
    {
        const size_t old_size = ti_->size;
        const size_t new_size = sizeof(IterType);
        assign(&ptr_, old_size, &_iter, new_size);
        ti_ = getFunctionInfos<IterType>();
        return *this;
    }

    any_iterator operator=(const any_iterator& _iter)
    {
        const size_t old_size = ti_->size;
        const size_t new_size = _iter.ti_->size;
        assign(&ptr_, old_size, _iter.ptr_, new_size);
        ti_ = _iter.ti_;
        return *this;
    }

    ~any_iterator() 
    {
        std::free(ptr_);
    }

    bool operator==(const any_iterator& _rhs) const
    {
        if (ti_ != _rhs.ti_) //different types
            return false;
        return ti_->equal_fn(ptr_, _rhs.ptr_);
    }

    template <typename IterType>
    bool operator==(const IterType& _rhs) const {
        return ti_->equal_fn(ptr_, reinterpret_cast<const void*>(&_rhs));
    }

    template <typename IterType>
    bool operator!=(const IterType& _rhs) const{
        return !operator==(_rhs);
    }

    /// Standard pre-increment operator
    any_iterator& operator++() {
        ti_->inc_fn(ptr_);
        return *this;
    }

    /// Standard post-increment operator
    any_iterator operator++(int) {
        any_iterator cpy(*this);
        ti_->inc_fn(ptr_);
        return cpy;
    }

    /// Standard pre-decrement operator
    any_iterator& operator--() {
        ti_->dec_fn(ptr_);
        return *this;
    }

    /// Standard post-decrement operator
    any_iterator operator--(int) {
        any_iterator cpy(*this);
        ti_->dec_fn(ptr_);
        return cpy;
    }

    const T& operator*() const {
        return *(ti_->deref_fn(ptr_));
    }

    /// Standard pointer operator.
    const T* operator->() const {
        return ti_->deref_fn(ptr_);
    }
};

// C++17 todo:
//if msvc >= 2017 or gcc with c++17 support
//#include <any>
//using AnyIterator = AnyIteratorT<std::any>;
//endif

} // end namespace tyti

template<typename IterT, typename T>
bool operator==(const IterT& _lhs, const tyti::any_iterator<T>& _rhs)
{
    return _rhs == _lhs;
}

template<typename IterT, typename T>
bool operator!=(const IterT& _lhs, const tyti::any_iterator<T>& _rhs)
{
    return _rhs != _lhs;
}
