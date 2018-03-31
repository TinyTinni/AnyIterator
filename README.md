# AnyIterator
[![Build Status](https://travis-ci.org/TinyTinni/AnyIterator.svg?branch=master)](https://travis-ci.org/TinyTinni/AnyIterator)
[![Build status](https://ci.appveyor.com/api/projects/status/8stwrgm6ud4ovjs3?svg=true)](https://ci.appveyor.com/project/TinyTinni/anyiterator)

Iterator with run-time polymorphism increment/decrement/deref operator.
Behaves like an Iterator from any container.
All Iterators must have the same dereference type, but can differ in container.

For the cases, where run-time polymorphism is needed and
[SCARY](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2913.pdf)
is not applicable e.g. not supported by container.

Example:
```
std::list<int> my_list = {1,2,3};
custom_list<int> my_custom_list = {1,2,3};

any_iterator<int> it;
if (condition)
    it = std::begin(my_list);
else
    it = std::begin(my_custom_list);

// do stuff with iterator like iterating

```

Requires C++11 (type_traits, static_assert, nullptr) See build status for more details.
Input Iterator must be destructable and at least copy_constructable and move_constructable.

Implementation nodes:
- only bidirectional iteratiors are supported (can easily be extended)
- uses heap for saving internal structure. If you know the maximum sies of your iterator, it is maybe a good idea to use [std::aligned_storage](http://en.cppreference.com/w/cpp/types/aligned_storage)
- for a quick performance overview, have a look at the [Performance Site](./tests/Performance.md)


