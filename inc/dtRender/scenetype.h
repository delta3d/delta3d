/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
* Bradley Anderegg
*/
#ifndef DELTA_SCENETYPE_H
#define DELTA_SCENETYPE_H

#include <dtRender/dtrenderexport.h>

#include <dtCore/actortype.h>

namespace dtRender
{
   class DT_RENDER_EXPORT SceneType: public dtCore::ActorType
   {
   public:
      static const dtCore::RefPtr<SceneType> BaseSceneType;

      SceneType(const std::string& name,
         const std::string& sceneType = "Scene",
         const std::string& desc="",
         const SceneType* parentType = BaseSceneType.get());
   protected:
      virtual ~SceneType();
   };

}

#endif // DELTA_SCENETYPE_H
