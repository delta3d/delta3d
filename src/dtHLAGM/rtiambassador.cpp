/* -*-c++-*-
 * Delta3D
 * Copyright 2012, MASA Group Inc.
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
 * David Guthrie
 */

#include <dtHLAGM/rtiambassador.h>

#include <dtUtil/librarysharingmanager.h>

#include <map>

namespace dtHLAGM
{

const std::string RTIAmbassador::RTI13_IMPLEMENTATION("rti13");
const std::string RTIAmbassador::RTI1516e_IMPLEMENTATION("rti1516e");

///////////////////////////////////////////////
RTIAmbassador::RTIAmbassador()
{
}

///////////////////////////////////////////////
RTIAmbassador::~RTIAmbassador()
{
}

typedef std::map<std::string, RTIAmbassador::CreateFuncType> CreateRTIFunctors;
static CreateRTIFunctors mImplementations;

///////////////////////////////////////////////
dtCore::RefPtr<RTIAmbassador> RTIAmbassador::Create(const std::string& implName)
{
   dtCore::RefPtr<RTIAmbassador> result;

   CreateRTIFunctors::iterator iter = mImplementations.find(implName);
   if (iter == mImplementations.end())
   {
      dtUtil::LibrarySharingManager::GetInstance().LoadSharedLibrary("dtHLAGM_" + implName);
      iter = mImplementations.find(implName);
   }

   if (iter != mImplementations.end())
   {
      result = iter->second();
   }

   return result;
}

///////////////////////////////////////////////
void RTIAmbassador::RegisterImplementation(const std::string& implName, CreateFuncType createFunctor)
{
   mImplementations.insert(std::make_pair(implName, createFunctor));
}


///////////////////////////////////////////////
void RTIAmbassador::UnregisterImplementation(const std::string& implName)
{
   mImplementations.erase(implName);
}

}
