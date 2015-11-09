/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_HAS_PROPERTY_H__
#define __DELTA_DTDIS_HAS_PROPERTY_H__

#include <vector>
#include <dtCore/refptr.h>
#include <dtCore/actorproxy.h>
#include <string>
#include <dtDIS/dtdisexport.h>

namespace dtDIS
{
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {

      /// a utility that builds a vector of actors that support the property of interest
      class DT_DIS_EXPORT HasProperty
      {
      public:
         typedef dtCore::ActorPtrVector ActorVector;

         HasProperty(const std::string& prop_name);
         ~HasProperty();
         HasProperty(const HasProperty& hp);

         void operator ()(dtCore::BaseActorObject *&proxy);

         ActorVector& GetPassedActors();
         const ActorVector& GetPassedActors() const;

      private:
 //        HasProperty& operator =(const HasProperty& hp);  // not implemented by design

         ActorVector mActors;

         std::string mPropName;
      };

   }  // end namespace details
   ///@endcond
}

#endif  // __DELTA_DTDIS_HAS_PROPERTY_H__
