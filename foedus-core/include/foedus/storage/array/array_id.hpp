/*
 * Copyright (c) 2014, Hewlett-Packard Development Company, LP.
 * The license and distribution terms for this file are placed in LICENSE.txt.
 */
#ifndef FOEDUS_STORAGE_ARRAY_ARRAY_ID_HPP_
#define FOEDUS_STORAGE_ARRAY_ARRAY_ID_HPP_
#include <foedus/storage/storage_id.hpp>
#include <stdint.h>
#include <iosfwd>
/**
 * @file foedus/storage/array/array_id.hpp
 * @brief Definitions of IDs in this package and a few related constant values.
 * @ingroup ARRAY
 */
namespace foedus {
namespace storage {
namespace array {

/**
 * @brief The only key type in array storage.
 * @ingroup ARRAY
 * @details
 * The key in array storage is \e offset, or an integer starting from zero.
 * This means we don't support multi-dimensional, dynamic, sparse, nor any other fancy arrays.
 * However, those arrays can be provided by the relational layer based on this array storage.
 * The offset-conversion is fairly straightforward.
 */
typedef uint64_t ArrayOffset;

/**
 * @brief Represents an offset range in an array storage.
 * @ingroup ARRAY
 * @details
 * Begin is inclusive, end is exclusive.
 */
struct ArrayRange {
    ArrayRange() : begin_(0), end_(0) {}
    ArrayRange(ArrayOffset begin, ArrayOffset end) : begin_(begin), end_(end) {}

    bool    contains(ArrayOffset offset) const { return offset >= begin_ && offset < end_; }

    /** Inclusive beginning of the offset range. */
    ArrayOffset begin_;
    /** Exclusive end of the offset range. */
    ArrayOffset end_;
};

/**
 * @brief Byte size of header in each page of array storage.
 * @ingroup ARRAY
 */
const uint16_t HEADER_SIZE = 32;
/**
 * @brief Byte size of data region in each page of array storage.
 * @ingroup ARRAY
 */
const uint16_t DATA_SIZE = foedus::storage::PAGE_SIZE - HEADER_SIZE;
/**
 * @brief Byte size of an entry in interior page of array storage.
 * @ingroup ARRAY
 */
const uint16_t INTERIOR_SIZE = 16;
/**
 * @brief Max number of entries in an interior page of array storage.
 * @ingroup ARRAY
 */
const uint16_t INTERIOR_FANOUT = (foedus::storage::PAGE_SIZE - HEADER_SIZE) / INTERIOR_SIZE;

}  // namespace array
}  // namespace storage
}  // namespace foedus
#endif  // FOEDUS_STORAGE_ARRAY_ARRAY_ID_HPP_