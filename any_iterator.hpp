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


    constexpr static bool is_small(size_t size)
    {
        return size <= sizeof(void*);
    }

    template<typename Iter>
    struct is_small_type
    {
        constexpr static bool value = is_small(sizeof(Iter));
    };
    template<typename Iter>
    static constexpr bool is_small_type_v = is_small_type<Iter>::value;


    template<typename K, typename U, bool = std::is_const<K>::value >
    struct if_const_add_const
    {
        using type = std::add_const_t<U>;
    };
    template<typename K, typename U>
    struct if_const_add_const<K,U,false>
    {
        using type = U;
    };
    template<typename K, typename U>
    using if_const_add_const_t = typename if_const_add_const<K,U>::type;


    // is small value -> address in void* is not a ptr
    template<typename Iter>
    inline static Iter ptr_cast(void* _ptr, std::true_type)
    {
        return reinterpret_cast<Iter>(_ptr);
    }
    // big type -> address in void* is a ptr
    template<typename Iter>
    inline static Iter ptr_cast(void* _ptr, std::false_type)
    {
        return reinterpret_cast<Iter>(_ptr);
    }

    //access functions
    template<typename Iter>
    inline static Iter ptr_cast(void* _ptr)
    {
        return ptr_cast<Iter>(_ptr, std::integral_constant<bool, is_small(sizeof(std::remove_pointer_t<Iter>))>());
    }

    template<typename Iter>
    inline static constexpr void* get_voidp(void** _ptr)
    {
        return is_small(sizeof(Iter)) ? static_cast<void*>(_ptr) : *_ptr;
    }
    template<typename Iter>
    inline static constexpr const void* get_voidp(const void** _ptr)
    {
        return is_small(sizeof(Iter)) ? static_cast<const void*>(_ptr): *_ptr;
    }

    template<typename Iter>
    static void inc(void* _ptr)
    {
        ++(*reinterpret_cast<Iter*>(get_voidp<Iter>(&_ptr)));
    }

    template<typename Iter>
    static void dec(void* _ptr)
    {
        --(*reinterpret_cast<Iter*>(get_voidp<Iter>(&_ptr)));
    }

    template<typename Iter>
    static const T* deref(const void* _ptr)
    {
        return &(*(*reinterpret_cast<const Iter*>(get_voidp<Iter>(&_ptr))));
    }

    template<typename Iter>
    static const bool equal(const void* _lhs, const void* _rhs)
    {
        return *reinterpret_cast<const Iter*>(get_voidp<Iter>(&_lhs)) == *reinterpret_cast<const Iter*>(get_voidp<Iter>(&_rhs));
    }
    template<typename Iter>
    static void dtor(void* _ptr)
    {
        reinterpret_cast<Iter*>(get_voidp<Iter>(&_ptr))->~Iter();
    }
    template<typename Iter>
    static void copyConstructor(void* _dst, const void* _src)
    {
        new (get_voidp<Iter>(&_dst)) Iter(*reinterpret_cast<const Iter*>(get_voidp<Iter>(&_src)));
    }
    template<typename Iter>
    static void moveConstructor(void* _dst, const void* _src)
    {
        new (get_voidp<Iter>(&_dst)) Iter(std::move(*reinterpret_cast<const Iter*>(get_voidp<Iter>(&_src))));
    }

    // helper functions
    inline void destruct()
    {
        ti_->dtor_fn(ptr_);
    }

    void switch_type(const TypeInfos* _newType)
    {
        destruct();

        //manage memory if type sizes are different
        if (ti_->size < _newType->size)
        {
            if ( is_small(ti_->size))
            {
                my_malloc(_newType->size);
            }
            else // old type is not small type
            {
                if (is_small(_newType->size))
                    std::free(ptr_);
                else 
                    ptr_ = std::realloc(ptr_, _newType->size);               
            }
            if ( !check_alloc(ptr_,_newType->size)) throw std::bad_alloc();
        }


        ti_ = _newType;
    }

    template<typename IterType>
    inline static constexpr const void* itertype_addr(const IterType& _iter)
    {
        const void* ptr = reinterpret_cast<const void*>(&_iter);
        return is_small(sizeof(IterType)) ? &ptr : ptr;
    }

    inline void copy_and_assign(const void* _src)
    {
        ti_->copy_ctor_fn(ptr_, _src);
    }

    inline void move_iter(const void* _src)
    {
        ti_->move_ctor_fn(ptr_, _src);
    }

    inline static void* my_malloc(size_t size)
    {
        return (is_small(size)) ? 0 : malloc(size);
    }

    inline static constexpr bool check_alloc(void* ptr, size_t size)
    {
        return (is_small(size)) ? true : ptr != nullptr;
    }

    // member variables
    void* ptr_;
    const TypeInfos* ti_;

    /// Interface
public:
    template <typename IterType, class = typename std::enable_if<!std::is_rvalue_reference<IterType>::value>::type>
    explicit any_iterator(const IterType& _iter) 
        : ptr_(my_malloc(sizeof(IterType))), ti_(getFunctionInfos<IterType>())
    {
        if (!check_alloc(ptr_, sizeof(IterType))) throw std::bad_alloc();
        copy_and_assign(itertype_addr(_iter));
    }

    template<typename IterType, class = typename std::enable_if<std::is_rvalue_reference<IterType>::value>::type>
        explicit any_iterator(IterType&& _iter)
        : ptr_(my_malloc(_iter.ti_->size)), ti_(std::move(_iter.ti_))
    {
        if ( !check_alloc(ptr_,_iter.ti_->size)) throw std::bad_alloc();
        move_iter(_iter);
    }

    any_iterator(const any_iterator& _iter)
        : ptr_(my_malloc(_iter.ti_->size)), ti_(_iter.ti_)
    {
        if ( !check_alloc(ptr_,_iter.ti_->size)) throw std::bad_alloc();
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
        copy_and_assign(itertype_addr(_iter));
        return *this;
    }

    template <typename IterType, class = typename std::enable_if<std::is_rvalue_reference<IterType>::value>::type>
        const any_iterator& operator=(IterType&& _iter)
    {
        switch_type(getFunctionInfos<IterType>());
        move_iter(itertype_addr(_iter));
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
        if (!is_small(ti_->size))
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
        return ti_->equal_fn(ptr_, itertype_addr(_rhs));
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