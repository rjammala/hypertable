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
#include "OperationRelinquish.h"

#include <Hypertable/Master/Context.h>

#include <Hypertable/Lib/RangeServerClient.h>
#include <Hypertable/Lib/Types.h>

#include <AsyncComm/CommAddress.h>

#if 0
#include "Common/Error.h"
#include "Common/FailureInducer.h"
#include "Common/ScopeGuard.h"
#include "Common/Serialization.h"
#include "Common/Sweetener.h"
#include "Common/System.h"
#include "Common/md5.h"
#include "Common/StringExt.h"
#include "Common/ScopeGuard.h"

#include "AsyncComm/ResponseCallback.h"

#include "Hypertable/Lib/Key.h"
#include "Hypertable/Lib/TableScanner.h"

#include "LoadBalancer.h"
#include "OperationProcessor.h"
#include "Utility.h"
#include "BalancePlanAuthority.h"

#include <sstream>
#endif

using namespace Hypertable;
using namespace std;

OperationRelinquish::OperationRelinquish(ContextPtr &context,
                                         const std::string &location,
                                         TableIdentifier &table,
                                         RangeSpec &range) 
  : OperationEphemeral(context, MetaLog::EntityType::OPERATION_RELINQUISH),
    m_location(location), m_table(table), m_range(range),
    m_range_name(format("%s[%s..%s]", table.id,
                        range.start_row, range.end_row)) {

  m_dependencies.insert(Dependency::INIT);
  m_dependencies.insert(Dependency::SERVERS);
  m_dependencies.insert(Dependency::ROOT);
  m_dependencies.insert(Dependency::METADATA);
  m_dependencies.insert(Dependency::SYSTEM);
  m_dependencies.insert(Dependency::RECOVER_SERVER);

  // Serialize with MoveRange operation
  m_obstructions.insert(String("OperationMove ") + m_range_name.c_str());
}

void OperationRelinquish::execute() {
  int32_t state = get_state();

  HT_INFOF("Entering OperationRelinquish-%lld %s location=%s state=%s",
           (Lld)header.id, m_range_name.c_str(), m_location.c_str(),
           OperationState::get_text(state));

  try {
    RangeServerClient rsc(m_context->comm);
    CommAddress addr;

    addr.set_proxy(m_location);
    rsc.relinquish_range(addr, m_table, m_range);
    complete_ok();
  }
  catch (Exception &e) {
    BalancePlanAuthority *bpa = 0;

    complete_error(e);
  }


  HT_INFOF("Leaving OperationRelinquish-%lld %s location=%s state=%s",
           (Lld)header.id, m_range_name.c_str(), m_location.c_str(),
           OperationState::get_text(get_state()));
}

void OperationRelinquish::display_state(std::ostream &os) {
  os << " " << m_table << " " << m_range << " location=" << m_location;
}

const String OperationRelinquish::name() {
  return "OperationRelinquish";
}

const String OperationRelinquish::label() {
  return format("Relinquish %s from %s", m_range_name.c_str(),
                m_location.c_str());
}

