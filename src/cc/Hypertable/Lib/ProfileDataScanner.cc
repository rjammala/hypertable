/*
 * Copyright (C) 2007-2014 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3
 * of the License.
 *
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/// @file
/// Ddefinitions for ProfileDataScanner.
/// This file contains type ddefinitions for ProfileDataScanner, a calls that
/// holds profile information for a scanner request.

#include <Common/Compat.h>

#include "ProfileDataScanner.h"

#include <Common/Serialization.h>
#include <Common/StringExt.h>

using namespace Hypertable;
using namespace Hypertable::Serialization;
using namespace std;

namespace {
  uint8_t VERSION {2};
}

size_t ProfileDataScanner::encoded_length() const {
  size_t length = encoded_length_internal();
  return 1 + Serialization::encoded_length_vi32(length) + length;
}

void ProfileDataScanner::encode(uint8_t **bufp) const {
  encode_i8(bufp, (uint8_t)VERSION);
  Serialization::encode_vi32(bufp, encoded_length_internal());
  encode_i32(bufp, (uint32_t)subscanners);
  encode_i32(bufp, (uint32_t)scanblocks);
  encode_i64(bufp, (uint64_t)cells_scanned);
  encode_i64(bufp, (uint64_t)cells_returned);
  encode_i64(bufp, (uint64_t)bytes_scanned);
  encode_i64(bufp, (uint64_t)bytes_returned);
  encode_i64(bufp, (uint64_t)disk_read);
  encode_i32(bufp, (uint32_t)servers.size());
  if (!servers.empty()) {
    for (auto & str : servers)
      encode_vstr(bufp, str);
  }
}

void ProfileDataScanner::decode(const uint8_t **bufp, size_t *remainp) {
  uint8_t version = decode_i8(bufp, remainp);
  uint32_t encoding_length {};
  const uint8_t *end {};
  if (version >= 2) {
    encoding_length = Serialization::decode_vi32(bufp, remainp);
    end = *bufp + encoding_length;
  }
  subscanners = (int32_t)decode_i32(bufp, remainp);
  scanblocks = (int32_t)decode_i32(bufp, remainp);
  cells_scanned = (int64_t)decode_i64(bufp, remainp);
  cells_returned = (int64_t)decode_i64(bufp, remainp);
  bytes_scanned = (int64_t)decode_i64(bufp, remainp);
  bytes_returned = (int64_t)decode_i64(bufp, remainp);
  disk_read = (int64_t)decode_i64(bufp, remainp);
  size_t count = (size_t)decode_i32(bufp, remainp);
  for (size_t i=0; i<count; i++)
    servers.insert( decode_vstr(bufp, remainp) );
  if (version >= 2) {
    // If encoding is longer than we expect, that means we're decoding a newer
    // version, so skip the newer portion that we don't know about
    if (*bufp < end)
      *bufp = end;
  }
}

size_t ProfileDataScanner::encoded_length_internal() const {
  size_t length = 52;
  if (!servers.empty()) {
    for (auto & str : servers)
      length += encoded_length_vstr(str);
  }
  return length;
}


ProfileDataScanner &ProfileDataScanner::operator+=(const ProfileDataScanner &other) {
  subscanners += other.subscanners;
  scanblocks += other.scanblocks;
  cells_scanned += other.cells_scanned;
  cells_returned += other.cells_returned;
  bytes_scanned += other.bytes_scanned;
  bytes_returned += other.bytes_returned;
  disk_read += other.disk_read;
  servers.insert(other.servers.begin(), other.servers.end());
  return *this;
}

ProfileDataScanner &ProfileDataScanner::operator-=(const ProfileDataScanner &other) {
  subscanners -= other.subscanners;
  scanblocks -= other.scanblocks;
  cells_scanned -= other.cells_scanned;
  cells_returned -= other.cells_returned;
  bytes_scanned -= other.bytes_scanned;
  bytes_returned -= other.bytes_returned;
  disk_read -= other.disk_read;
  for (auto &server : other.servers)
    servers.erase(server);
  return *this;
}

string ProfileDataScanner::to_string() {
  string str = "{ProfileDataScanner: ";
  str += string("cells_scanned=") + cells_scanned + " ";
  str += string("cells_returned=") + cells_returned + " ";
  str += string("bytes_scanned=") + bytes_scanned + " ";
  str += string("bytes_returned=") + bytes_returned + " ";
  str += string("disk_read=") + disk_read + " ";
  str += string("subscanners=") + subscanners + " ";
  str += string("scanblocks=") + scanblocks + " ";
  str += string("servers=");
  bool first = true;
  for (auto & server : servers) {
    if (first)
      first = false;
    else
      str += ",";
    str += server;
  }
  str += "}";
  return str;
}
