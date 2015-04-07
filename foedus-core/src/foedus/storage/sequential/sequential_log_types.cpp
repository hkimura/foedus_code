/*
 * Copyright (c) 2014-2015, Hewlett-Packard Development Company, LP.
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details. You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * HP designates this particular file as subject to the "Classpath" exception
 * as provided by HP in the LICENSE.txt file that accompanied this code.
 */
#include "foedus/storage/sequential/sequential_log_types.hpp"

#include <glog/logging.h>

#include <algorithm>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "foedus/assert_nd.hpp"
#include "foedus/storage/storage_log_types.hpp"
#include "foedus/storage/sequential/sequential_metadata.hpp"
#include "foedus/thread/thread.hpp"

namespace foedus {
namespace storage {
namespace sequential {

void SequentialCreateLogType::apply_storage(Engine* engine, StorageId storage_id) {
  reinterpret_cast<CreateLogType*>(this)->apply_storage(engine, storage_id);
}

void SequentialCreateLogType::assert_valid() {
  reinterpret_cast<CreateLogType*>(this)->assert_valid();
  ASSERT_ND(header_.log_length_ == sizeof(SequentialCreateLogType));
  ASSERT_ND(header_.get_type() == log::get_log_code<SequentialCreateLogType>());
}
std::ostream& operator<<(std::ostream& o, const SequentialCreateLogType& v) {
  o << "<SequentialCreateLog>" << v.metadata_ << "</SequentialCreateLog>";
  return o;
}

std::ostream& operator<<(std::ostream& o, const SequentialAppendLogType& v) {
  o << "<SequentialAppendLog>"
    << "<payload_count_>" << v.payload_count_ << "</payload_count_>";
  // show first few bytes
  o << "<data_>";
  for (uint16_t i = 0; i < std::min<uint16_t>(8, v.payload_count_); ++i) {
    o << i << ":" << static_cast<int>(v.payload_[i]) << " ";
  }
  o << "...</data_>";
  o << "</SequentialAppendLog>";
  return o;
}

}  // namespace sequential
}  // namespace storage
}  // namespace foedus
