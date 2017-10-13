#pragma once

#include <iterator>

#include <cstdlib> //malloc, realloc
#include <cstring> //memcpy, memmove
#include <exception> //bad_alloc
#include <type_traits>
#include <utility>

#include <cassert>

namespace tyti {
template<typename T>
class any_iterator : std::iterator<std::bidirectional_iterator_tag, T>
{
    //functionpointer save structurez
    struct TypeInfos
    {
        void(*const inc_fn)(void*);
        void(*const dec_fn)(void*);
        const bool(*const equal_fn)(const void*,const void*);
        const T*(*const deref_fn)(const void*);
        void(*const dtor_fn)(void*);
        void(*const copy_ctor_fn)(void*,const void*);
        void(*const move_ctor_fn)(void*, const void*);
        const size_t size;
    };

    template<typename IterType>
    TypeInfos* getFunctionInfos()
    {
        static TypeInfos ti =
        {
            &any_iterator::inc<IterType>,
            &any_iterator::dec<IterType>,
            &any_iterator::equal<IterType>,
            &any_iterator::deref<IterType>,
            //(std::is_trivially_destructible<IterType>::value) ? 
            //static_cast<void(*)(void*)>(nullptr) : 
            &any_iterator::dtor<IterType>,
            //(std::is_trivially_copy_constructible<IterType>::value) ?
            //static_cast<void(*)(void*,const void*)>(nullptr) :
            &any_iterator::copyConstructor<IterType>,
            &any_iterator::moveConstructor<IterType>,
            sizeof(IterType)
        };
        return &ti;
    }

    // used to destruct nothing e.g. used when the l-value should not destruct anything
    // Do not provide it to the user.
    struct NoDestruct
    {
        ~NoDestruct() {} //only destructor is used

        NoDestruct(const NoDestruct&) { assert(false); }
        NoDestruct(NoDestruct&&) { assert(false); }
        NoDestruct operator++() { assert(false); return *this; }
        NoDestruct operator--() { assert(false); return *this; }
        bool operator==(const NoDestruct&) const { assert(false); return false; }
        bool operator!=(const NoDestruct&) const { assert(false); return false; }
        //this function will never be called. just for compile correctness
        const T& operator*() const { assert(false); return *(reinterpret_cast<const T*>(this)); }
    };

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
    template<typename Iter>
    static void dtor(void* _ptr)
    {
        reinterpret_cast<Iter*>(_ptr)->~Iter();
    }
    template<typename Iter>
    static void copyConstructor(void* _dst, const void* _src)
    {
        new (_dst) Iter(*reinterpret_cast<const Iter*>(_src));
    }
    template<typename Iter>
    static void moveConstructor(void* _dst, const void* _src)
    {
        new (_dst) Iter(std::move(*reinterpret_cast<const Iter*>(_src)));
    }

    // helper functions
    inline void destruct()
    {
        ti_->dtor_fn(ptr_);
    }

    void switch_type(const TypeInfos* _newType)
    {
        destruct();
        if (ti_->size < _newType->size)
        {
            ptr_ = std::realloc(ptr_, _newType->size);
            if (!ptr_)
                throw std::bad_alloc();
        }
        ti_ = _newType;
    }

    inline void copy_and_assign(const void* _src)
    {
        ti_->copy_ctor_fn(ptr_, _src);
    }

    inline void move_iter(const void* _src)
    {
        ti_->move_ctor_fn(ptr_, _src);
    }

    // member variables
    void* ptr_;
    const TypeInfos* ti_;

    /// Interface
public:
    template <typename IterType, class = typename std::enable_if<!std::is_rvalue_reference<IterType>::value>::type>
    explicit any_iterator(const IterType& _iter) 
        : ptr_(std::malloc(sizeof(IterType))), ti_(getFunctionInfos<IterType>())
    {
        if (!ptr_)
            throw std::bad_alloc();
        copy_and_assign(&_iter);
    }

    template<typename IterType, class = typename std::enable_if<std::is_rvalue_reference<IterType>::value>::type>
        explicit any_iterator(IterType&& _iter)
        : ptr_(std::malloc(_iter.ti_->size)), ti_(std::move(_iter.ti_))
    {
        if (!ptr_)
            throw std::bad_alloc();
        move_iter(_iter.ptr_);
    }

    any_iterator(const any_iterator& _iter)
        : ptr_(std::malloc(_iter.ti_->size)), ti_(_iter.ti_)
    {
        if (!ptr_)
            throw std::bad_alloc();
        copy_and_assign(_iter.ptr_);
    }

    any_iterator(any_iterator&& _iter)
        :ptr_(_iter.ptr_), ti_(_iter.ti_)
    {
        _iter.ti_ = getFunctionInfos<NoDestruct>();
    }

    template <typename IterType, class = typename std::enable_if<!std::is_rvalue_reference<IterType>::value>::type >
    const any_iterator& operator=(const IterType& _iter)
    {
        switch_type(getFunctionInfos<IterType>());
        copy_and_assign(&_iter);
        return *this;
    }

    template <typename IterType, class = typename std::enable_if<std::is_rvalue_reference<IterType>::value>::type>
        const any_iterator& operator=(IterType&& _iter)
    {
        switch_type(getFunctionInfos<IterType>());
        move_iter(&_iter);
        return *this;
    }

    const any_iterator& operator=(const any_iterator& _iter)
    {
        switch_type(_iter.ti_);
        copy_and_assign(_iter.ptr_);
        return *this;
    }

    const any_iterator& operator=(any_iterator&& _iter)
    {
        std::swap(ti_, _iter.ti_);
        std::swap(ptr_, _iter.ptr_);
        //old ptr gets destructed via _iter
        return *this;
    }

    ~any_iterator() 
    {
        destruct();
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
bool operator==(const IterT&& _lhs, const tyti::any_iterator<T>&& _rhs)
{
    return _rhs.operator==(std::forward<IterT>(_lhs));
}

template<typename IterT, typename T>
bool operator!=(IterT&& _lhs, tyti::any_iterator<T>&& _rhs)
{
    return _rhs.operator!=(std::forward<IterT>(_lhs));
}