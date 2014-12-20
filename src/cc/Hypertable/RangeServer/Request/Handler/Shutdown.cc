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

#include "Shutdown.h"

#include <Hypertable/RangeServer/RangeServer.h>

#include <AsyncComm/ResponseCallback.h>

extern "C" {
#include <poll.h>
}

using namespace Hypertable;
using namespace Hypertable::RangeServer::Request::Handler;

void Shutdown::run() {
  ResponseCallback cb(m_comm, m_event);
  m_range_server->shutdown();
  cb.response_ok();
  poll(0, 0, 2000);
  HT_INFO("Exiting RangeServer.");
  _exit(0);
}
