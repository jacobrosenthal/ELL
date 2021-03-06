////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

// math
#include "Vector.h"

// stl
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <vector>

namespace ell
{
namespace data
{
    /// <summary> Interface for infinite-dimensional vectors of double numbers. Each implementation of
    /// this interface has a mathematical dimension of infinity and is made up of an explicitly specified
    /// prefix followed by an implicit suffix of zeros. </summary>
    class IDataVector
    {
    public:
        /// <summary> Values that represent data vector types. </summary>
        enum class Type
        {
            DoubleDataVector,
            FloatDataVector,
            ShortDataVector,
            ByteDataVector,
            SparseDoubleDataVector,
            SparseFloatDataVector,
            SparseShortDataVector,
            SparseByteDataVector,
            SparseBinaryDataVector,
            AutoDataVector
        };

        virtual ~IDataVector() = default;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual Type GetType() const = 0;

        /// <summary> Adds a value at the end of the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendElement(size_t index, double value) = 0;

        /// <summary>
        /// A data vector has infinite dimension and ends with a suffix of zeros. This function returns
        /// the first index in this suffix. Equivalently, the returned value is one plus the index of the
        /// last non-zero element.
        /// </summary>
        ///
        /// <returns> The first index of the suffix of zeros at the end of this vector. </returns>
        virtual size_t PrefixLength() const = 0;

        /// <summary> Computes the 2-norm of the vector (not the squared 2-norm). </summary>
        ///
        /// <returns> The vector 2-norm. </returns>
        virtual double Norm2() const = 0;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(const math::UnorientedConstVectorReference<double> vector) const = 0;

        /// <summary>
        /// Performs the operation: vector += scalar * (*this), where other is an array of doubles.
        /// </summary>
        ///
        /// <param name="vector"> [in,out] The vector to which this data vector is added. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(math::RowVectorReference<double> vector, double scalar = 1.0) const = 0;

        /// <summary> Copies the contents of this DataVector into a double array of size PrefixLength(). </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const = 0;

        /// <summary> Copies the contents of this DataVector into a double array of a given size. </summary>
        ///
        /// <param name="size"> The desired array size. </param>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray(size_t size) const = 0;

        /// <summary> Copies this data vector into another type of data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> The return type. </typeparam>
        ///
        /// <returns> The new data vector. </returns>
        template <typename ReturnType>
        ReturnType DeepCopyAs() const;

        /// <summary> Copies this data vector into another type of data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> The return type. </typeparam>
        /// <param name="nonZeroTransform"> A functor that takes an IndexValue and returns a double, which is applied to each non-zero elements during the copy. </param>
        ///
        /// <returns> The new data vector. </returns>
        template <typename ReturnType, typename TransformType>
        ReturnType DeepCopyAs(TransformType nonZeroTransform) const;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        virtual void Print(std::ostream& os) const = 0;
    };

    /// <summary> A helper definition used to define the IsDataVector SFINAE concept. </summary>
    template <typename T>
    using IsDataVector = typename std::enable_if_t<std::is_base_of<IDataVector, T>::value, bool>;

    /// <summary>
    /// Base class for some of the data vector classes. This class uses a curiously recurring
    /// template pattern to significantly reduce code duplication in the derived classes.
    /// </summary>
    ///
    /// <typeparam name="DerivedType"> The derived type, in the curiously recurring template design pattern. </typeparam>
    template <class DerivedType>
    class DataVectorBase : public IDataVector
    {
    public:
        /// <summary> Takes an iterator and appends its entries to the data vector. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        void AppendElements(IndexValueIteratorType indexValueIterator);

        /// <summary> Takes an initializer list of index value pairs and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The initializer list of index value pairs. </param>
        void AppendElements(std::initializer_list<IndexValue> list);

        /// <summary> Takes an initializer list of values and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        void AppendElements(std::initializer_list<double> list);

        /// <summary> Takes a vector of index value pairs and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The vector of index value pairs. </param>
        void AppendElements(std::vector<IndexValue> vec);

        /// <summary> Takes a vector of values and appends them to the data vector. </summary>
        ///
        /// <param name="list"> The vector. </param>
        void AppendElements(std::vector<double> vec);

        /// <summary> Computes the 2-norm of the vector (not the squared 2-norm). </summary>
        ///
        /// <returns> The vector 2-norm. </returns>
        virtual double Norm2() const override;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(const math::UnorientedConstVectorReference<double> vector) const override;

        /// <summary>
        /// Performs the operation: vector += scalar * (*this), where other is an array of doubles.
        /// </summary>
        ///
        /// <param name="vector"> [in,out] The vector to which this data vector is added. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(math::RowVectorReference<double> vector, double scalar = 1.0) const override;

        /// <summary> Copies the contents of this DataVector into a double array of size PrefixLength(). </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const override;

        /// <summary> Copies the contents of this DataVector into a double array of a given size. </summary>
        ///
        /// <param name="size"> The desired array size. </param>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray(size_t size) const override;

        /// <summary> Copies the contents of a data vector to another data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> Type of the data vector to construct. </typeparam>
        ///
        /// <returns> A data vector of a specified type. </returns>
        template <typename ReturnType>
        ReturnType DeepCopyAs() const;

        /// <summary> Copies the contents of a data vector to another data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> Type of the data vector to construct. </typeparam>
        /// <param name="nonZeroTransform"> A functor that takes an IndexValue and returns a double, which is applied to each non-zero elements during the copy. </param>
        ///
        /// <returns> A data vector of a specified type. </returns>
        template <typename ReturnType, typename TransformType>
        ReturnType DeepCopyAs(TransformType nonZeroTransform) const;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        virtual void Print(std::ostream& os) const override;
    };
}
}

#include "../tcc/DataVector.tcc"