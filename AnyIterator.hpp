#pragma once

#include <vector>
#include <iterator>

namespace tyti {
template<typename HandleT>
class AnyIteratorT : std::iterator<std::bidirectional_iterator_tag, HandleT>
{
    //functionpointer save structure
    struct FunctionPtrs
    {
        void(*inc_fn)(void*);
        void(*dec_fn)(void*);
        void(*del_fn)(void*);
        const bool(*equal_fn)(const void*,const void*);
        const HandleT*(*deref_fn)(const void*);

        void*(*copy_fn)(const void*);
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
            ptrs.del_fn = AnyIteratorT::deletePtr<IterType>;
            ptrs.inc_fn = AnyIteratorT::inc<IterType>;
            ptrs.dec_fn = AnyIteratorT::dec<IterType>;
            ptrs.deref_fn = AnyIteratorT::deref<IterType>;
            ptrs.equal_fn = AnyIteratorT::equal<IterType>;
            ptrs.copy_fn = AnyIteratorT::createCopy < IterType > ;
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
    template <typename T>
    static size_t getNumber()
    {
        static Incrementer i = Incrementer(T());
        return i.number;
    }

    //access functions
    template<typename Iter>
    static void deletePtr(void* _ptr)
    {
        delete reinterpret_cast<Iter*>(_ptr);
    }

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
    static const HandleT* deref(const void* _ptr)
    {
        return &(*(*reinterpret_cast<const Iter*>(_ptr)));
    }

    template<typename Iter>
    static const bool equal(const void* _lhs, const void* _rhs)
    {
        return *reinterpret_cast<const Iter*>(_lhs) == *reinterpret_cast<const Iter*>(_rhs);
    }

    template<typename Iter>
    static void* createCopy(const void* _ptr)
    {
        return reinterpret_cast<void*>(new Iter(*reinterpret_cast<const Iter*>(_ptr)));
    }


    // member variables
    void* ptr_;
    size_t index_;

    /// Interface
public:
    template <typename IterType>
    explicit AnyIteratorT(const IterType& _iter) 
        : ptr_(reinterpret_cast<void*>(new IterType(_iter))), index_(getNumber<IterType>())
    {
    }

    AnyIteratorT(const AnyIteratorT& _iter)
        : ptr_(functionPtrs_[_iter.index_].copy_fn(_iter.ptr_)), index_(_iter.index_)
    {
    }

    template <typename IterType>
    AnyIteratorT operator=(const IterType& _iter)
    {
        functionPtrs_[index_].del_fn(ptr_);
        ptr_ = reinterpret_cast<void*>(new IterType(_iter));
        index_ = getNumber<IterType>();
        return *this;
    }

    AnyIteratorT operator=(const AnyIteratorT& _iter)
    {
        functionPtrs_[index_].del_fn(ptr_);
        ptr_ = functionPtrs_[_iter.index_].copy_fn(_iter.ptr_);
        index_ = _iter.index_;
        return *this;
    }

    ~AnyIteratorT() 
    {
        functionPtrs_[index_].del_fn(ptr_);
    }

    bool operator==(const AnyIteratorT& _rhs) const
    {
        if (index_ != _rhs.index_)
            return false;
        return functionPtrs_[index_].equal_fn(ptr_, _rhs.ptr_);
    }

    template <typename IterType>
    bool operator==(const IterType& _rhs) const {
        return functionPtrs_[index_].equal_fn(ptr_, reinterpret_cast<const void*>(&_rhs));
    }

    template <typename T>
    bool operator!=(const T& _rhs) const{
        return !operator==(_rhs);
    }

    /// Standard pre-increment operator
    AnyIteratorT& operator++() {
        functionPtrs_[index_].inc_fn(ptr_);
        return *this;
    }

    /// Standard post-increment operator
    AnyIteratorT operator++(int) {
        AnyIteratorT cpy(*this);
        functionPtrs_[index_].inc_fn(ptr_);
        return cpy;
    }

    /// Standard pre-decrement operator
    AnyIteratorT& operator--() {
        functionPtrs_[index_].dec_fn(ptr_);
        return *this;
    }

    /// Standard post-decrement operator
    AnyIteratorT operator--(int) {
        AnyIteratorT cpy(*this);
        functionPtrs_[index_].dec_fn(ptr_);
        return cpy;
    }

    const HandleT& operator*() const {
        return *(functionPtrs_[index_].deref_fn(ptr_));
    }

    /// Standard pointer operator.
    const HandleT* operator->() const {
        return functionPtrs_[index_].deref_fn(ptr_);
    }
};

template<typename HandleT>
std::vector<typename AnyIteratorT<HandleT>::FunctionPtrs> AnyIteratorT<HandleT>::functionPtrs_;

// C++17 todo:
//if msvc >= 2017 or gcc with c++17 support
//#include <any>
//using AnyIterator = AnyIteratorT<std::any>;
//endif

} // end namespace tyti

template<typename T, typename HandleT>
bool operator==(const T& _lhs, const tyti::AnyIteratorT<HandleT>& _rhs)
{
    return _rhs == _lhs;
}
