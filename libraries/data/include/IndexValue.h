////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IndexValue.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <type_traits>

namespace ell
{
namespace data
{
    /// <summary> An entry in a vector </summary>
    struct IndexValue
    {
        size_t index;
        double value;
    };

    // parent classes for iterators
    struct IIndexValueIterator
    {
    };
#ifndef SWIG
    template <typename IteratorType>
    using IsIndexValueIterator = typename std::enable_if_t<std::is_base_of<IIndexValueIterator, IteratorType>::value, bool>;
#endif
}
}
