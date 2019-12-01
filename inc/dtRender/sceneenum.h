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
#ifndef DELTA_SCENEENUM_H
#define DELTA_SCENEENUM_H

#include <dtRender/dtrenderexport.h>
#include <dtUtil/enumeration.h>

namespace dtRender
{         
      /*
      *
      *  These types define the different sub scenes which
      *     can be created.  
      */
      class DT_RENDER_EXPORT SceneEnum : public dtUtil::Enumeration 
      {
         DECLARE_ENUM(SceneEnum);
      public:

         static SceneEnum PRE_RENDER; //0
         
         static SceneEnum MULTIPASS; //1

         static SceneEnum BACKGROUND;//2

         static SceneEnum NON_TRANSPARENT_OBJECTS; //3

         static SceneEnum DEFAULT_SCENE; //4

         static SceneEnum TRANSPARENT_OBJECTS;  //5

         static SceneEnum FOREGROUND; //6

         static SceneEnum POST_RENDER; //7
        
         static SceneEnum NUM_SCENES; //8

         /**
         *  @return the child number of the specified scene.
         */
         int GetSceneNumber() const;

         static SceneEnum& FindSceneByNumber(int);

      protected:         
         SceneEnum(const std::string &name, int order);

      private:
         const int mSceneEnum;         
      };
}

#endif // DELTA_SCENEENUM_H
