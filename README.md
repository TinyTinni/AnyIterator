# AnyIterator
Iterator with run-time polymorphism increment/decrement/deref operator.
Behaves like an Iterator from any container.
All Iterators must have the same dereference type, but can differ in container.

For the cases, where run-time polymorphism is needed and
[SCARY](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2913.pdf)
is not applicable e.g. not supported by container.

Just an older project, currently only basic operations and bidirectional iterators are supported.
Tests needs to be rewritten, since the original one depends on another project. 