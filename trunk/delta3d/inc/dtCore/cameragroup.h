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

#ifndef DELTA_CAMERA_GROUP
#define DELTA_CAMERA_GROUP

#include <dtCore/export.h>
#include <osg/Referenced>
#include <map>
#include <set>

namespace Producer
{
   class RenderSurface;
}

namespace dtCore
{
   class Camera;

   class DT_CORE_EXPORT CameraGroup : osg::Referenced
   {
      friend Camera;

   public:

      CameraGroup();

   protected:

      virtual ~CameraGroup();

      bool AddCamera( Camera* camera );
      bool RemoveCamera( Camera* camera );

   public:

      void Frame();

   private:

      typedef std::set<Camera*> CameraSet;
      typedef std::map< unsigned int, CameraSet > IntCameraSetMap;
      IntCameraSetMap mFrameBinMap;

      typedef std::set< Producer::RenderSurface* > RenderSurfaceSet;
      RenderSurfaceSet mRenderSurfaceSet;
   };
}

#endif //DELTA_CAMERA_GROUP