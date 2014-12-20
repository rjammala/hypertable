/*
 * Copyright (C) 2007-2014 Hypertable, Inc.
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

#include <Common/Compat.h>

#include "AcknowledgeLoad.h"

#include <Hypertable/RangeServer/RangeServer.h>
#include <Hypertable/RangeServer/Response/Callback/AcknowledgeLoad.h>

#include <Hypertable/Lib/Types.h>

#include <Common/Error.h>
#include <Common/Logger.h>
#include <Common/Serialization.h>

using namespace Hypertable;
using namespace Hypertable::RangeServer;
using namespace Hypertable::RangeServer::Request::Handler;

void AcknowledgeLoad::run() {
  Response::Callback::AcknowledgeLoad cb(m_comm, m_event);
  vector<QualifiedRangeSpec> ranges;
  QualifiedRangeSpec range;
  const uint8_t *decode_ptr = m_event->payload;
  size_t decode_remain = m_event->payload_len;

  try {
    int nn = Serialization::decode_i32(&decode_ptr, &decode_remain);
    for (int ii = 0; ii < nn; ++ii) {
      range.decode(&decode_ptr, &decode_remain);
      ranges.push_back(range);
    }
    m_range_server->acknowledge_load(&cb, ranges);
  }
  catch (Exception &e) {
    HT_ERROR_OUT << "AcknowledgeLoad " << e << HT_END;
    cb.error(e.code(), "Error handling AcknowledgeLoad message");
  }
}
