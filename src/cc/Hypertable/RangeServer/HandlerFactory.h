/* -*- c++ -*-
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

#ifndef HYPERTABLE_HANDLERFACTORY_H
#define HYPERTABLE_HANDLERFACTORY_H

#include "ConnectionHandler.h"
#include "RangeServer.h"

#include <AsyncComm/ApplicationQueue.h>
#include <AsyncComm/Comm.h>

namespace Hypertable {

  class Comm;

  class HandlerFactory : public ConnectionHandlerFactory {
  public:
    HandlerFactory(Comm *comm, ApplicationQueuePtr &app_queue,
                   Apps::RangeServerPtr range_server)
      : m_comm(comm), m_app_queue(app_queue),
        m_range_server(range_server) { }

    virtual void get_instance(DispatchHandlerPtr &dhp) {
      dhp = new ConnectionHandler(m_comm, m_app_queue, m_range_server);
    }
  private:
    Comm *m_comm;
    ApplicationQueuePtr m_app_queue;
    Apps::RangeServerPtr m_range_server;
  };

}

#endif // HYPERTABLE_HANDLERFACTORY_H
