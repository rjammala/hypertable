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

#ifndef Hypertable_Master_OperationRelinquish_h
#define Hypertable_Master_OperationRelinquish_h

#include <Hypertable/Master/OperationEphemeral.h>

namespace Hypertable {

  class OperationRelinquish : public OperationEphemeral {
  public:
    OperationRelinquish(ContextPtr &context, const std::string &location,
                        TableIdentifier &table, RangeSpec &range);

    virtual ~OperationRelinquish() { }

    void initialize_dependencies();

    virtual void execute();
    virtual void display_state(std::ostream &os);
    virtual const String name();
    virtual const String label();

  private:
    const std::string m_location;
    TableIdentifierManaged m_table;
    RangeSpecManaged m_range;
    const std::string m_range_name;
  };

} // namespace Hypertable

#endif // Hypertable_Master_OperationRelinquish_h
