/* -*- c++ -*-
 * Copyright (C) 2007-2014 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or any later version.
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
/// Declarations for Flush request parameters.
/// This file contains declarations for Flush, a class for encoding and
/// decoding paramters to the <i>flush</i> file system broker function.

#ifndef FsBroker_Lib_Request_Parameters_Flush_h
#define FsBroker_Lib_Request_Parameters_Flush_h

#include <Common/Serializable.h>

#include <string>

using namespace std;

namespace Hypertable {
namespace FsBroker {
namespace Lib {
namespace Request {
namespace Parameters {

  /// @addtogroup FsBrokerLibRequestParameters
  /// @{

  /// %Request parameters for <i>flush</i> requests.
  class Flush : public Serializable {
  public:

    /// Constructor.
    /// Empty initialization for decoding.
    Flush() {}

    /// Constructor.
    /// Initializes with parameters for encoding.  Sets #m_fd to
    /// <code>fd</code>.
    /// @param fd File descriptor
    Flush(int32_t fd) : m_fd(fd) {}

    /// Returns encoded length of parameters
    /// @return Encoded length of parameters
    size_t encoded_length() const override;

    /// Encodes parameters to buffer
    /// @param bufp Address of buffer to encode parameters to
    /// (advanced by call)
    void encode(uint8_t **bufp) const override;

    /// Decodes parameters from buffer
    /// @param bufp Address of buffer from which to decode parameters
    /// (advanced by call)
    /// @param remainp Address of remaining encoded data in buffer
    /// (advanced by call)
    void decode(const uint8_t **bufp, size_t *remainp) override;

    /// Gets file descriptor
    /// @return File descriptor
    int32_t get_fd() { return m_fd; }

  private:

    /// Returns internal encoded length
    size_t encoded_length_internal() const override;

    /// File descriptor to which flush applies
    int32_t m_fd {};

  };

  /// @}

}}}}}

#endif // FsBroker_Lib_Request_Parameters_Flush_h
