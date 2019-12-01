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

#ifndef DELTA_SUNLIGHTSHADER
#define DELTA_SUNLIGHTSHADER

#include <osg/Vec2>
#include <osg/Vec3>

#include <dtCore/export.h>
#include <dtCore/refptr.h>

// For some reason, you can't forward declare these.
#include <osg/VertexProgram>
#include <osg/FragmentProgram>

namespace dtCore
{
   ///A light scattering pixel shader for use with dtCore::Environment

   /** This shader is built into the Environment and is used by setting the
     * Environment::FogMode to ADV.
     */
   class DT_CORE_EXPORT SunlightShader
   {
      public:
         SunlightShader();
         ~SunlightShader();

         ///Update the shader with new values
         void Update(   const osg::Vec2& sunDir, 
                        const osg::Vec3& eyeXYZ,
                        float turbidity, 
                        float energy, 
                        float molecules );

         osg::VertexProgram* GetLightScatterinVP() { return mLightScatterinVP.get(); }
         osg::FragmentProgram* GetTerrainFP() { return mTerrainFP.get(); }

      private:

         dtCore::RefPtr<osg::VertexProgram> mLightScatterinVP;
         dtCore::RefPtr<osg::FragmentProgram> mTerrainFP;

         osg::Vec3 lambda;
         osg::Vec3 lambda2;
         osg::Vec3 lambda4;
         float n; ///<Refractive index of air
         float pn; ///<depolarization factor of air
         float greenstein; ///<eccentricity value
         float mBrightness; ///<scene brightness adjustment
         float mContrast; ///<scene contrast adjustment
         osg::Vec3 betaRay;
         osg::Vec3 betaMie;

         float ConcentrationFactor(float turbidity);
   };
}

#endif // DELTA_SUNLIGHTSHADER
