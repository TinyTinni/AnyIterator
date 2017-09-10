# AnyIterator
[![Build Status](https://travis-ci.org/TinyTinni/AnyIterator.svg?branch=master)](https://travis-ci.org/TinyTinni/AnyIterator)
[![Build status](https://ci.appveyor.com/api/projects/status/8stwrgm6ud4ovjs3?svg=true)](https://ci.appveyor.com/project/TinyTinni/anyiterator)

Iterator with run-time polymorphism increment/decrement/deref operator.
Behaves like an Iterator from any container.
All Iterators must have the same dereference type, but can differ in container.

For the cases, where run-time polymorphism is needed and
[SCARY](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2913.pdf)
is not applicable e.g. not supported by container.

Just an older project, currently only basic operations and bidirectional iterators are supported.
Tests needs to be rewritten, since the original one depends on another project. 

Requires C++11
Input Iterator must be destructable and at least copy_constructable