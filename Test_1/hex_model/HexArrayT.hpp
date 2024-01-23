#ifndef HEXARRAY_HPP
#define HEXARRAY_HPP

#include "HexIterator.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>

template<class valueType>
class HexArrayT
{
public:
    HexArrayT(const size_t& size);
    HexArrayT(const HexArrayT& other);
    HexArrayT(const std::vector<valueType>& other);
    HexArrayT(const size_t& size, const valueType& value);
    virtual ~HexArrayT();
    virtual HexArrayT& operator=(const HexArrayT& other);

    virtual valueType& operator[](const unsigned int& offset);
    virtual const valueType& operator[](const unsigned int& offset) const;

    virtual Hex_It<valueType> begin();
    virtual Hex_It<valueType> end();
    virtual Hex_It<const valueType> begin() const;
    virtual Hex_It<const valueType> end() const;

    virtual const size_t Size() const;
    virtual std::vector<valueType> ToVector() const;

protected:
    size_t size;
    valueType* array;
};

//------------------------------------------------------------------------------

template<class valueType>
inline HexArrayT<valueType>::HexArrayT(const size_t &size)
    : size{size}
    , array(new valueType[size])
{
    memset(array, 0, size * sizeof(valueType));
}

template<class valueType>
inline HexArrayT<valueType>::HexArrayT(const HexArrayT& other)
    : size(other.size)
    , array(new valueType[size])
{
    memcpy(this->array, other.array, sizeof(valueType) * size);
}

template<class valueType>
inline HexArrayT<valueType>::HexArrayT(const std::vector<valueType>& other)
    : size(other.size())
    , array(new valueType[size])
{
    valueType* p = array;
    for (const auto& cell : other)
    {
        *p = cell;
        ++p;
    }
}

template<class valueType>
inline HexArrayT<valueType>::HexArrayT(const size_t &size, const valueType &value)
    : size{size}
    , array(new valueType[size])
{
    memset(array, value, size * sizeof(valueType));
}

template<class valueType>
inline HexArrayT<valueType>::~HexArrayT()
{
    delete[] array;
}

template<class valueType>
inline HexArrayT<valueType>& HexArrayT<valueType>::operator=(const HexArrayT& other)
{
    this->size = other.size;
    if (array != nullptr)
    {
        delete[] array;
    }
    this->array = new valueType[size];
    memcpy(this->array, other.array, sizeof(valueType) * size);
    return *this;
}

template<class valueType>
inline valueType &HexArrayT<valueType>::operator[](const unsigned int &offset)
{
    assert(offset < size);
    return array[offset];
}

template<class valueType>
inline const valueType &HexArrayT<valueType>::operator[](const unsigned int &offset) const
{
    assert(offset < size);
    return array[offset];
}

template<class valueType>
inline Hex_It<valueType> HexArrayT<valueType>::begin()
{
    return Hex_It<valueType>(array);
}

template<class valueType>
inline Hex_It<valueType> HexArrayT<valueType>::end()
{
    return Hex_It<valueType>(array + size);
}

template<class valueType>
inline Hex_It<const valueType> HexArrayT<valueType>::begin() const
{
    return Hex_It<const valueType>(array);
}

template<class valueType>
inline Hex_It<const valueType> HexArrayT<valueType>::end() const
{
    return Hex_It<const valueType>(array + size);
}

template<class valueType>
inline const size_t HexArrayT<valueType>::Size() const
{
    return size;
}

template<class valueType>
inline std::vector<valueType> HexArrayT<valueType>::ToVector() const
{
    std::vector<valueType> vector(size);
    valueType* p = array;
    for (auto cell : vector)
    {
        cell = *p;
        ++p;
    }
    return vector;
}

#endif // HEXARRAY_HPP
