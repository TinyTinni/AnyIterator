#pragma once

#include <vector>
#include <iterator>

#include <cstdlib> //malloc, realloc
#include <cstring> //memcpy
#include <exception> //bad_alloc

namespace tyti {
template<typename T>
class any_iterator : std::iterator<std::bidirectional_iterator_tag, T>
{
    //functionpointer save structure
    struct FunctionPtrs
    {
        void(*inc_fn)(void*);
        void(*dec_fn)(void*);
        void(*del_fn)(void*);
        const bool(*equal_fn)(const void*,const void*);
        const T*(*deref_fn)(const void*);

        size_t size;
    };

    // function lookup table
    static std::vector<FunctionPtrs> functionPtrs_;

    // init lookuptable and generate number for the given type. used by the "getNumber" interface
    struct Incrementer
    {
        const size_t number;
        template< typename IterType>
        Incrementer(IterType /*dummy*/) :number(nextNumber())
        {
            //init function lookup table
            FunctionPtrs ptrs;
            ptrs.inc_fn = any_iterator::inc<IterType>;
            ptrs.dec_fn = any_iterator::dec<IterType>;
            ptrs.deref_fn = any_iterator::deref<IterType>;
            ptrs.equal_fn = any_iterator::equal<IterType>;
            ptrs.size = sizeof(IterType);

            functionPtrs_.push_back(ptrs);
        }
    private:
        static size_t nextNumber()
        {
            // maybe use std::type_index+ unordered_map. 
            // But i cannot see any advantage (req. more mem, more computation)
            static size_t next = 0; 
            return next++;
        }
    };

    //interface to get the number
    template <typename IterType>
    static size_t getNumber()
    {
        static Incrementer i = Incrementer(IterType());
        return i.number;
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
    static void assign(void* dst_, size_t dst_size_, const void* src_, size_t src_size_)
    {
        if (dst_size_ < src_size_)
        {
            dst_ = std::realloc(dst_, src_size_);
            if (!dst_)
                throw std::bad_alloc();
        }
        std::memcpy(dst_, src_, src_size_);
    }

    // member variables
    void* ptr_;
    size_t index_;

    /// Interface
public:
    template <typename IterType>
    explicit any_iterator(const IterType& _iter) 
        : ptr_(std::malloc(sizeof(IterType))), index_(getNumber<IterType>())
    {
        if (!ptr_)
            throw std::bad_alloc();
        std::memcpy(ptr_, &_iter, sizeof(IterType));
    }

    any_iterator(const any_iterator& _iter)
        : ptr_(std::malloc(functionPtrs_[_iter.index_].size)), index_(_iter.index_)
    {
        if (!ptr_)
            throw std::bad_alloc();
        std::memcpy(ptr_, _iter.ptr_, functionPtrs_[_iter.index_].size);
    }

    template <typename IterType>
    any_iterator operator=(const IterType& _iter)
    {
        const size_t old_size = functionPtrs_[index_].size;
        const size_t new_size = sizeof(IterType);
        assign(ptr_, old_size, &_iter, new_size);
        index_ = getNumber<IterType>();
        return *this;
    }

    any_iterator operator=(const any_iterator& _iter)
    {
        const size_t old_size = functionsPtrs_[index_].size;
        const size_t new_size = functionsPtrs_[_iter.index_].size;
        assign(ptr, oldsize, _iter.ptr, new_size);
        index_ = _iter.index_;
        return *this;
    }

    ~any_iterator() 
    {
        std::free(ptr_);
    }

    bool operator==(const any_iterator& _rhs) const
    {
        if (index_ != _rhs.index_)
            return false;
        return functionPtrs_[index_].equal_fn(ptr_, _rhs.ptr_);
    }

    template <typename IterType>
    bool operator==(const IterType& _rhs) const {
        return functionPtrs_[index_].equal_fn(ptr_, reinterpret_cast<const void*>(&_rhs));
    }

    template <typename IterType>
    bool operator!=(const IterType& _rhs) const{
        return !operator==(_rhs);
    }

    /// Standard pre-increment operator
    any_iterator& operator++() {
        functionPtrs_[index_].inc_fn(ptr_);
        return *this;
    }

    /// Standard post-increment operator
    any_iterator operator++(int) {
        any_iterator cpy(*this);
        functionPtrs_[index_].inc_fn(ptr_);
        return cpy;
    }

    /// Standard pre-decrement operator
    any_iterator& operator--() {
        functionPtrs_[index_].dec_fn(ptr_);
        return *this;
    }

    /// Standard post-decrement operator
    any_iterator operator--(int) {
        any_iterator cpy(*this);
        functionPtrs_[index_].dec_fn(ptr_);
        return cpy;
    }

    const T& operator*() const {
        return *(functionPtrs_[index_].deref_fn(ptr_));
    }

    /// Standard pointer operator.
    const T* operator->() const {
        return functionPtrs_[index_].deref_fn(ptr_);
    }
};

template<typename T>
std::vector<typename any_iterator<T>::FunctionPtrs> any_iterator<T>::functionPtrs_;

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