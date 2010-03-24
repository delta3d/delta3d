/* -*-c++-*-
* Simulation Core
* Copyright 2007-2008, Alion Science and Technology
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
*/

#ifndef __WATER_GRID_BUILDER_H__
#define __WATER_GRID_BUILDER_H__

#include <dtDAL/plugin_export.h>
#include <dtActors/watergridactor.h>


namespace dtActors
{
   class DT_PLUGIN_EXPORT WaterGridBuilder
   {
   public:

      static osg::Node* CreateQuad(osg::Texture2D* texture, int renderBin);

      static osg::Texture2D* CreateTexture(int width, int height);

      static osg::Geometry* BuildRadialGrid(float &outComputedRadialDistance);

      static void BuildWaves(std::vector<WaterGridActor::Wave>& waveList);

      static void BuildTextureWaves(std::vector<WaterGridActor::TextureWave>& waveList);

   private:

      WaterGridBuilder(){}
      ~WaterGridBuilder(){}

      static void SetWaveDirection(WaterGridActor::Wave& wave);
   };
}

#endif // WATER_GRID_BUILDER_H
