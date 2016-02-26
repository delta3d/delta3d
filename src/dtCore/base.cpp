/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/base.h>
#include <dtUtil/log.h>

using namespace dtUtil;

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(Base)

/**
 * Constructor.
 *
 * @param name the instance name
 */
Base::Base(const std::string& name) : mName(name)
{
   RegisterInstance(this);
}

/**
 * Destructor.
 */
Base::~Base()
{
   Log& log = Log::GetInstance();

   log.LogMessage(Log::LOG_DEBUG, __FUNCTION__, "Destroying '%s'", GetName().c_str());
   DeregisterInstance(this);
}

/**
 * Sets the name of this instance.
 *
 * @param name the new name
 */
void Base::SetName(const std::string& name)
{
   mName = name;
}

/**
 * Returns the name of this instance.
 *
 * @return the current name
 */
const std::string& Base::GetName() const
{
   return mName;
}

}
