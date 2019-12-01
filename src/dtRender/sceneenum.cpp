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

#include <dtRender/sceneenum.h>
#include <dtUtil/log.h>

namespace dtRender
{

   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(SceneEnum);

   SceneEnum SceneEnum::PRE_RENDER("PRE_RENDER", 0);
   SceneEnum SceneEnum::MULTIPASS("MULTIPASS", 1);
   SceneEnum SceneEnum::BACKGROUND("BACKGROUND", 2);
   SceneEnum SceneEnum::NON_TRANSPARENT_OBJECTS("NON_TRANSPARENT_OBJECTS", 3);
   SceneEnum SceneEnum::DEFAULT_SCENE("DEFAULT_SCENE", 4);
   SceneEnum SceneEnum::TRANSPARENT_OBJECTS("TRANSPARENT_OBJECTS", 5);
   SceneEnum SceneEnum::FOREGROUND("FOREGROUND", 6);
   SceneEnum SceneEnum::POST_RENDER("POST_RENDER", 7);
   SceneEnum SceneEnum::NUM_SCENES("NUM_SCENES", 8);


   SceneEnum::SceneEnum(const std::string &name, int order) 
      : dtUtil::Enumeration(name)
      , mSceneEnum(order)
   {
      AddInstance(this);
   }

   int SceneEnum::GetSceneNumber() const
   {
      return mSceneEnum;
   }

   SceneEnum& SceneEnum::FindSceneByNumber( int i)
   {
      EnumerateListType::iterator iter = mInstances.begin();
      EnumerateListType::iterator iterEnd = mInstances.end();
    
      for (;iter != iterEnd; ++iter)
      {
         SceneEnum* se = *iter;
         if(se->GetSceneNumber() == i)
         {
            return *se;
         }
      }

      LOG_ERROR("SceneEnumeration out of bounds!");
      return DEFAULT_SCENE;     
   }

   
}//namespace dtRender
