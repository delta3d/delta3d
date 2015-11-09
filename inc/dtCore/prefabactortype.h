/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, Inc
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
#ifndef PREFABACTORTYPE_H_
#define PREFABACTORTYPE_H_

#include <dtCore/actortype.h>
#include <dtCore/resourcedescriptor.h>

namespace dtCore
{

   class PrefabActorType: public ActorType
   {
   public:
      PrefabActorType(const dtCore::ResourceDescriptor& prefabRes, const std::string& description, const ActorType* parentType);
      virtual ~PrefabActorType();

      const dtCore::ResourceDescriptor& GetPrefabResource() const { return mPrefabResource; }
   private:
      dtCore::ResourceDescriptor mPrefabResource;
   };

} /* namespace dtCore */

#endif /* PREFABACTORTYPE_H_ */
