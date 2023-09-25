#ifndef HEXITERATOR_HPP
#define HEXITERATOR_HPP

template<class T>
class Hex_It
{
public:
    Hex_It(T* p) : ptr(p) { }
    T& operator*() { return *ptr; }
    Hex_It& operator++() { return ++ptr, *this; }
    bool operator!=(const Hex_It& other) { return this->ptr != other.ptr; }
    bool operator==(const Hex_It& other) { return this->ptr == other.ptr; }
private:
    T* ptr;
};


#endif // HEXITERATOR_HPP
