/*
 * Copyright (C) 2007-2012 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3 of the
 * License, or any later version.
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

/** @file
 * Abstract base class for a filesystem.
 * All commands have synchronous and asynchronous versions. Commands that
 * operate on the same file descriptor are serialized by the underlying
 * filesystem. In other words, if you issue three asynchronous commands,
 * they will get carried out and their responses will come back in the
 * same order in which they were issued. Unless otherwise mentioned, the
 * methods could throw Exception.
 */

#include <Common/Compat.h>

#include "Filesystem.h"

#include <Common/Error.h>
#include <Common/Serialization.h>

using namespace Hypertable;
using namespace Serialization;

namespace {
  uint8_t DIRENT_VERSION {1};
}

size_t Filesystem::Dirent::encoded_length() const {
  size_t length = encoded_length_internal();
  return 1 + Serialization::encoded_length_vi32(length) + length;
}
void Filesystem::Dirent::encode(uint8_t **bufp) const {
  encode_i8(bufp, DIRENT_VERSION);
  encode_vi32(bufp, encoded_length_internal());
  encode_vstr(bufp, name);
  encode_i64(bufp, length);
  encode_i32(bufp, last_modification_time);
  encode_bool(bufp, is_dir);
}

void Filesystem::Dirent::decode(const uint8_t **bufp, size_t *remainp) {
  uint8_t version = Serialization::decode_i8(bufp, remainp);
  if (version != DIRENT_VERSION)
    HT_THROWF(Error::PROTOCOL_ERROR,
	      "Dirent parameters version mismatch, expected %d, got %d",
	      (int)DIRENT_VERSION, (int)version);
  uint32_t encoding_length = Serialization::decode_vi32(bufp, remainp);
  const uint8_t *end = *bufp + encoding_length;
  name = decode_vstr(bufp, remainp);
  length = decode_i64(bufp, remainp);
  last_modification_time = decode_i32(bufp, remainp);
  is_dir = decode_bool(bufp, remainp);
  // If encoding is longer than we expect, that means we're decoding a newer
  // version, so skip the newer portion that we don't know about
  if (*bufp < end)
    *bufp = end;
}

size_t Filesystem::Dirent::encoded_length_internal() const {
  return 13 + encoded_length_vstr(name);
}


void
Filesystem::decode_response_readdir(EventPtr &event_ptr,
                                    std::vector<Dirent> &listing) {
  const uint8_t *decode_ptr = event_ptr->payload;
  size_t decode_remain = event_ptr->payload_len;

  int error = decode_i32(&decode_ptr, &decode_remain);

  if (error != Error::OK)
    HT_THROW(error, "");

  uint32_t len = decode_i32(&decode_ptr, &decode_remain);

  listing.clear();
  listing.reserve(len);
  Dirent entry;
  for (uint32_t i = 0; i < len; i++) {
    entry.decode(&decode_ptr, &decode_remain);
    listing.push_back(entry);
  }
}


bool
Filesystem::decode_response_exists(EventPtr &event_ptr) {
  const uint8_t *decode_ptr = event_ptr->payload;
  size_t decode_remain = event_ptr->payload_len;

  int error = decode_i32(&decode_ptr, &decode_remain);

  if (error != Error::OK)
    HT_THROW(error, "");

  return decode_bool(&decode_ptr, &decode_remain);
}


int
Filesystem::decode_response(EventPtr &event_ptr) {
  const uint8_t *decode_ptr = event_ptr->payload;
  size_t decode_remain = event_ptr->payload_len;

  return decode_i32(&decode_ptr, &decode_remain);
}

static String &
remove_trailing_duplicates(String &s, char separator) {
  while (s.size() > 1) {
    if (s[s.size()-1] == separator)
      s.resize(s.size() - 1);
    else
      break;
  }

  return s;
}

String Filesystem::dirname(String name, char separator) {
  if (name.size() == 1 && name[0] == separator)
    return name;

  // Remove trailing separators
  std::string::size_type cur = name.length();
  while (cur > 0 && name[cur - 1] == separator)
    --cur;
  name.resize(cur);

  // Find last separator
  std::string::size_type pos = name.rfind(separator);

  if (pos == std::string::npos)
    name = "."; // No separators
  else if (pos == 0 && name.length() == 1 && name[0] == separator)
    name = separator; // Only separators
  else if (pos == 0 && name.length() > 1 && name[0] == separator)
    name = "/";
  else
    name.resize(pos);

  // Remove any duplicate adjacent path separators
  return remove_trailing_duplicates(name, separator);
}


String Filesystem::basename(String name, char separator) {
  if (name.size() == 1 && name[0] == separator)
    return name;

  // Remove trailing separators
  std::string::size_type cur = name.length();
  while (cur > 0 && name[cur - 1] == separator)
    --cur;
  name.resize(cur);

  // Find last separator
  std::string::size_type pos = name.rfind(separator);

  std::string ret;
  if (pos == std::string::npos)
    ; // No separators
  else if (pos == 0 && name.length() == 1 && name[0] == separator)
    name = separator; // Only separators
  else
    name = name.substr(pos + 1); // Basename only

  // Remove any duplicate adjacent path separators
  return remove_trailing_duplicates(name, separator);
}
