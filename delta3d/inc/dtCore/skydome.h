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

#ifndef DELTA_SKYDOME
#define DELTA_SKYDOME

#include <dtCore/enveffect.h>
#include <osg/Vec3>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Node;
   class Geode;
}
/// @endcond

namespace dtCore
{
   class MoveEarthySkyWithEyePointTransformAzimuth;

   ///A rendered dome that represents the sky
   class DT_CORE_EXPORT SkyDome :  public EnvEffect
   {
      DECLARE_MANAGEMENT_LAYER(SkyDome)

   public:
   
      SkyDome( const std::string& name = "SkyDome" );
   
   protected:
      
      virtual ~SkyDome();
      
   public:
      ///sets the base color
      void SetBaseColor( const osg::Vec3& color );
           
      //gets the base color
      void GetBaseColor( osg::Vec3& color ) const { color.set(mBaseColor); }
    
      ///the virtual paint function
      virtual void Repaint(   const osg::Vec3& skyColor, 
                              const osg::Vec3& fogColor,
                              double sunAngle, 
                              double sunAzimuth,
                              double visibility );

   private:

      /// Build the sky dome
      void Config();
      osg::Node* MakeDome();
      
      osg::Vec3 mBaseColor;
      dtCore::RefPtr<osg::Geode> mGeode;
      dtCore::RefPtr<MoveEarthySkyWithEyePointTransformAzimuth> mXform;
   };
}

#endif // DELTA_SKYDOME
