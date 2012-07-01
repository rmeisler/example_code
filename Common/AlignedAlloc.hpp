#pragma once

#include <stdlib.h>
#include <new> // placement new

// Replacement for new/delete for aligned types

template <typename AlignedType>
inline void AlignedFree(AlignedType* value)
{
    value->~AlignedType();
    _aligned_free(value);
}

// Multiple versions of alloc for various ctor parameters
template <typename AlignedType>
inline AlignedType* AlignedAlloc()
{
    return new (_aligned_malloc(sizeof(AlignedType), 64)) AlignedType();
}

template <typename AlignedType, typename CtorParam1Type>
inline AlignedType* AlignedAlloc(const CtorParam1Type& param1)
{
    return new (_aligned_malloc(sizeof(AlignedType), 64)) AlignedType(param1);
}

template <typename AlignedType, typename CtorParam1Type, typename CtorParam2Type>
inline AlignedType* AlignedAlloc(const CtorParam1Type& param1, const CtorParam2Type& param2)
{
    return new (_aligned_malloc(sizeof(AlignedType), 64)) AlignedType(param1, param2);
}

template <typename AlignedType, typename CtorParam1Type, typename CtorParam2Type, typename CtorParam3Type>
inline AlignedType* AlignedAlloc(const CtorParam1Type& param1, const CtorParam2Type& param2, const CtorParam3Type& param3)
{
    return new (_aligned_malloc(sizeof(AlignedType), 64)) AlignedType(param1, param2, param3);
}
