////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Dataset.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"

// stl
#include <algorithm>
#include <cassert>
#include <random>
#include <stdexcept>

namespace ell
{
namespace data
{
    template <typename IteratorExampleType>
    GetExampleIteratorFunctor<IteratorExampleType>::GetExampleIteratorFunctor(size_t fromIndex, size_t size)
        : _fromIndex(fromIndex), _size(size)
    {
    }

    template <typename IteratorExampleType>
    template <typename ExampleType>
    auto GetExampleIteratorFunctor<IteratorExampleType>::operator()(const Dataset<ExampleType>& dataset) const -> ReturnType
    {
        return dataset.template GetExampleIterator<IteratorExampleType>(_fromIndex, _size);
    }

    template <typename ExampleType>
    ExampleIterator<ExampleType> AnyDataset::GetExampleIterator() const
    {
        GetExampleIteratorFunctor<ExampleType> abstractor(_fromIndex, _size);

        // all Dataset types for which GetAnyDataset() is called must be listed below, in the variadic template argument.
        return utilities::AbstractInvoker<DatasetBase, Dataset<data::AutoSupervisedExample>, Dataset<data::DenseSupervisedExample>>::Invoke(abstractor, *_pDataset);
    }

    template <typename DatasetExampleType>
    template <typename IteratorExampleType>
    Dataset<DatasetExampleType>::DatasetExampleIterator<IteratorExampleType>::DatasetExampleIterator(InternalIteratorType begin, InternalIteratorType end)
        : _current(begin), _end(end)
    {
    }

    template <typename DatasetExampleType>
    Dataset<DatasetExampleType>::Dataset(ExampleIterator<DatasetExampleType> exampleIterator)
    {
        while (exampleIterator.IsValid())
        {
            AddExample(exampleIterator.Get());
            exampleIterator.Next();
        }
    }

    template <typename DatasetExampleType>
    Dataset<DatasetExampleType>::Dataset(const AnyDataset& anyDataset)
        : Dataset(anyDataset.GetExampleIterator<DatasetExampleType>())
    {
    }

    template <typename DatasetExampleType>
    DatasetExampleType& Dataset<DatasetExampleType>::GetExample(size_t index)
    {
        return _examples[index];
    }

    template <typename DatasetExampleType>
    const DatasetExampleType& Dataset<DatasetExampleType>::GetExample(size_t index) const
    {
        return _examples[index];
    }

    template <typename DatasetExampleType>
    DatasetExampleType& Dataset<DatasetExampleType>::operator[](size_t index)
    {
        return _examples[index];
    }

    template <typename DatasetExampleType>
    const DatasetExampleType& Dataset<DatasetExampleType>::operator[](size_t index) const
    {
        return _examples[index];
    }

    template <typename DatasetExampleType>
    template <typename IteratorExampleType>
    ExampleIterator<IteratorExampleType> Dataset<DatasetExampleType>::GetExampleIterator(size_t fromIndex, size_t size) const
    {
        size = CorrectRangeSize(fromIndex, size);
        return ExampleIterator<IteratorExampleType>(std::make_unique<DatasetExampleIterator<IteratorExampleType>>(_examples.cbegin() + fromIndex, _examples.cbegin() + fromIndex + size));
    }

    template <typename DatasetExampleType>
    auto Dataset<DatasetExampleType>::GetExampleReferenceIterator(size_t fromIndex, size_t size) const -> ExampleReferenceIterator
    {
        size = CorrectRangeSize(fromIndex, size);
        return ExampleReferenceIterator(_examples.cbegin() + fromIndex, _examples.cbegin() + fromIndex + size);
    }

    template <typename DatasetExampleType>
    void Dataset<DatasetExampleType>::AddExample(DatasetExampleType example)
    {
        size_t numFeatures = example.GetDataVector().PrefixLength();
        _examples.push_back(std::move(example));

        if (_numFeatures < numFeatures)
        {
            _numFeatures = numFeatures;
        }
    }

    template <typename DatasetExampleType>
    void Dataset<DatasetExampleType>::Reset()
    {
        _examples.clear();
        _numFeatures = 0;
    }

    template <typename DatasetExampleType>
    void Dataset<DatasetExampleType>::RandomPermute(std::default_random_engine& rng, size_t prefixSize)
    {
        prefixSize = CorrectRangeSize(0, prefixSize);
        for (size_t i = 0; i < prefixSize; ++i)
        {
            RandomSwap(rng, i, i, _examples.size() - i);
        }
    }

    template <typename DatasetExampleType>
    void Dataset<DatasetExampleType>::RandomPermute(std::default_random_engine& rng, size_t rangeFirstIndex, size_t rangeSize, size_t prefixSize)
    {
        rangeSize = CorrectRangeSize(rangeFirstIndex, rangeSize);

        if (prefixSize > rangeSize || prefixSize == 0)
        {
            prefixSize = rangeSize;
        }

        for (size_t s = 0; s < prefixSize; ++s)
        {
            size_t index = rangeFirstIndex + s;
            RandomSwap(rng, index, index, rangeSize - s);
        }
    }

    template <typename DatasetExampleType>
    void Dataset<DatasetExampleType>::RandomSwap(std::default_random_engine& rng, size_t targetExampleIndex, size_t rangeFirstIndex, size_t rangeSize)
    {
        using std::swap;
        rangeSize = CorrectRangeSize(rangeFirstIndex, rangeSize);
        if (targetExampleIndex > _examples.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange);
        }

        std::uniform_int_distribution<size_t> dist(rangeFirstIndex, rangeFirstIndex + rangeSize - 1);
        size_t j = dist(rng);
        swap(_examples[targetExampleIndex], _examples[j]);
    }

    template <typename DatasetExampleType>
    template <typename SortKeyType>
    void Dataset<DatasetExampleType>::Sort(SortKeyType sortKey, size_t fromIndex, size_t size)
    {
        size = CorrectRangeSize(fromIndex, size);

        std::sort(_examples.begin() + fromIndex,
                  _examples.begin() + fromIndex + size,
                  [&](const DatasetExampleType& a, const DatasetExampleType& b) -> bool {
                      return sortKey(a) < sortKey(b);
                  });
    }

    template <typename DatasetExampleType>
    template <typename PartitionKeyType>
    void Dataset<DatasetExampleType>::Partition(PartitionKeyType partitionKey, size_t fromIndex, size_t size)
    {
        size = CorrectRangeSize(fromIndex, size);
        std::partition(_examples.begin() + fromIndex, _examples.begin() + fromIndex + size, partitionKey);
    }

    template <typename DatasetExampleType>
    void Dataset<DatasetExampleType>::Print(std::ostream& os, size_t tabs, size_t fromIndex, size_t size) const
    {
        size = CorrectRangeSize(fromIndex, size);

        for (size_t index = fromIndex; index < fromIndex + size; ++index)
        {
            os << std::string(tabs * 4, ' ');
            _examples[index].Print(os);
            os << "\n";
        }
    }

    template <typename DatasetExampleType>
    std::ostream& operator<<(std::ostream& os, const Dataset<DatasetExampleType>& dataset)
    {
        dataset.Print(os);
        return os;
    }

    template <typename DatasetExampleType>
    size_t Dataset<DatasetExampleType>::CorrectRangeSize(size_t fromIndex, size_t size) const
    {
        if (size == 0 || fromIndex + size > _examples.size())
        {
            return _examples.size() - fromIndex;
        }
        return size;
    }
}
}
