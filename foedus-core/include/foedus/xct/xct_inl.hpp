/*
 * Copyright (c) 2014, Hewlett-Packard Development Company, LP.
 * The license and distribution terms for this file are placed in LICENSE.txt.
 */
#ifndef FOEDUS_XCT_XCT_INL_HPP_
#define FOEDUS_XCT_XCT_INL_HPP_
#include <foedus/assert_nd.hpp>
#include <foedus/compiler.hpp>
#include <foedus/error_stack.hpp>
#include <foedus/storage/record.hpp>
#include <foedus/assorted/atomic_fences.hpp>
#include <foedus/xct/xct.hpp>
#include <foedus/xct/xct_access.hpp>
#include <iosfwd>
/**
 * @file foedus/xct/xct_inl.hpp
 * @brief Inline functions of Xct.
 * @ingroup XCT
 */
namespace foedus {
namespace xct {

inline ErrorCode Xct::add_to_read_set(storage::Record* record) {
    if (isolation_level_ == DIRTY_READ_PREFER_SNAPSHOT
        || isolation_level_ == DIRTY_READ_PREFER_VOLATILE) {
        return ERROR_CODE_OK;
    } else if (UNLIKELY(read_set_size_ >= max_read_set_size_)) {
        return ERROR_CODE_XCT_READ_SET_OVERFLOW;
    }

    ASSERT_ND(record->owner_id_.epoch_.is_valid());
    read_set_[read_set_size_].observed_owner_id_ = record->owner_id_;

    // for RCU protocol, make sure compiler/CPU don't reorder the data access before tag copy.
    // This is _consume rather than _acquire because it's fine to see stale information as far as
    // we don't access before the tag copy.
    assorted::memory_fence_consume();

    // If the record is locked, we will most likely abort at commit time.
    // So, do it immediately to avoid wasting CPU resource.
    if (UNLIKELY(read_set_[read_set_size_].observed_owner_id_.is_locked<15>())) {
        return ERROR_CODE_XCT_RACE_ABORT;
    }
    read_set_[read_set_size_].record_ = record;
    ++read_set_size_;
    return ERROR_CODE_OK;
}
inline ErrorCode Xct::add_to_write_set(storage::Record* record, void* log_entry) {
    if (UNLIKELY(write_set_size_ >= max_write_set_size_)) {
        return ERROR_CODE_XCT_WRITE_SET_OVERFLOW;
    }

    write_set_[write_set_size_].observed_owner_id_ = record->owner_id_;
    write_set_[write_set_size_].record_ = record;
    write_set_[write_set_size_].log_entry_ = log_entry;
    ++write_set_size_;
    return ERROR_CODE_OK;
}

}  // namespace xct
}  // namespace foedus
#endif  // FOEDUS_XCT_XCT_INL_HPP_