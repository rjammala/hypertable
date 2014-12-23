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
#include "LoadRange.h"

#include <Hypertable/RangeServer/RangeServer.h>

#include <Hypertable/Lib/Types.h>

#include <AsyncComm/ResponseCallback.h>

#include <Common/Error.h>
#include <Common/Logger.h>
#include <Common/Serialization.h>

using namespace Hypertable;
using namespace Hypertable::RangeServer::Request::Handler;

void LoadRange::run() {
  ResponseCallback cb(m_comm, m_event);
  TableIdentifier table;
  RangeSpec range;
  RangeState range_state;
  bool needs_compaction;
  const uint8_t *decode_ptr = m_event->payload;
  size_t decode_remain = m_event->payload_len;

  try {
    table.decode(&decode_ptr, &decode_remain);
    range.decode(&decode_ptr, &decode_remain);
    range_state.decode(&decode_ptr, &decode_remain);
    needs_compaction = Serialization::decode_bool(&decode_ptr, &decode_remain);
    m_range_server->load_range(&cb, &table, &range,
                               &range_state, needs_compaction);
  }
  catch (Exception &e) {
    HT_ERROR_OUT << e << HT_END;
    cb.error(e.code(), "Error handling LoadRange message");
  }
}