/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have included with MySQL.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef _XPL_EXPECT_GTID_H_
#define _XPL_EXPECT_GTID_H_

#include "expect.h"

namespace xpl
{
  class Expect_gtid : public Expect_condition
  {
  public:
    explicit Expect_gtid(const std::string &data);
    virtual ngs::Error_code check();

    virtual Expect_condition *copy()
    {
      return new Expect_gtid(*this);
    }

    void set_timeout(int t) { m_timeout = t; }

  private:
    Expect_gtid() {}
    int m_timeout;
    std::list<std::string> m_gtids;
  };
}


#endif