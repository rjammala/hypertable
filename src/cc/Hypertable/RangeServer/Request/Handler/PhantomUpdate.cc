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

#include "PhantomUpdate.h"

#include <Hypertable/RangeServer/RangeServer.h>
#include <Hypertable/RangeServer/Response/Callback/PhantomUpdate.h>

#include <Hypertable/Lib/Types.h>

#include <AsyncComm/ResponseCallback.h>

#include <Common/Error.h>
#include <Common/Logger.h>
#include <Common/Serialization.h>

using namespace Hypertable;
using namespace Hypertable::RangeServer;
using namespace Hypertable::RangeServer::Request::Handler;

void PhantomUpdate::run() {
  String location;
  vector<uint32_t> fragments;
  vector<QualifiedRangeSpecManaged> ranges;
  QualifiedRangeSpec range;
  int plan_generation;
  uint32_t fragment;

  Response::Callback::PhantomUpdate cb(m_comm, m_event);

  const uint8_t *decode_ptr = m_event->payload;
  size_t decode_remain = m_event->payload_len;
  try {
    location = Serialization::decode_vstr(&decode_ptr, &decode_remain);
    plan_generation = Serialization::decode_i32(&decode_ptr, &decode_remain);
    range.decode(&decode_ptr, &decode_remain);
    fragment = Serialization::decode_i32(&decode_ptr, &decode_remain);
    cb.initialize(range, fragment);
    m_range_server->phantom_update(&cb, location, plan_generation, 
         range, fragment, m_event);
  }
  catch (Exception &e) {
    HT_ERROR_OUT << e << HT_END;
    cb.error(e.code(), e.what());
    return;
  }
}
