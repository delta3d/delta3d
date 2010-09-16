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
   
      SkyDome(const std::string& name = "SkyDome", bool createCapGeometry = true, float radius = 6000.0f);

   protected:

      virtual ~SkyDome();

   public:
      ///sets the base color
      void SetBaseColor(const osg::Vec3& color);

      ///gets the base color
      void GetBaseColor(osg::Vec3& color) const { color.set(mBaseColor); }

      bool GetCapEnabled() const { return mEnableCap; }

      ///the virtual paint function
      /** 0 degrees  = horizon
       *  90 degrees = high noon
       *  - degrees  = below horizon
       */
      virtual void Repaint(const osg::Vec3& skyColor, 
                           const osg::Vec3& fogColor,
                           double sunAngle, 
                           double sunAzimuth,
                           double visibility);

   private:

      /// Build the sky dome
      void Config(float radius);
      bool IsSunsetOrSunrise(double sunAngle) const;
      osg::Vec3 CalcCenterColors(double vis_factor,
                                 const osg::Vec3& skyColor, 
                                 const osg::Vec3& fogColor) const;
      
      void CalcNewColors(double visibility, const osg::Vec3& skyColor, 
                         const osg::Vec3& fogColor);

      double CalcCVF(double visibility) const;

      void SetUpperMiddleLowerColors(const osg::Vec3& skyColor, const osg::Vec3& fogColor,
                                     unsigned int i, double visibility );
      void AssignColors() const;

      double GetVisibilityFactor(double visibility) const;

      dtCore::RefPtr<osg::Geode> mGeode;
      dtCore::RefPtr<MoveEarthySkyWithEyePointTransformAzimuth> mXform;

      bool mEnableCap;
      osg::Vec3 mBaseColor;
      osg::Vec3 outer_param, outer_amt, outer_diff;
      osg::Vec3 middle_param, middle_amt, middle_diff;
      osg::Vec3 center_color;
      osg::Vec3 upper_color[19];
      osg::Vec3 middle_color[19];
      osg::Vec3 lower_color[19];
      osg::Vec3 bottom_color[19];
  };
}
#endif // DELTA_SKYDOME
