#ifndef DELTA_SKYDOMESHADER
#define DELTA_SKYDOMESHADER

#include <osg/Vec3>
#include <osg/VertexProgram>
#include <osg/FragmentProgram>
#include "sg.h"

#include "export.h"

namespace dtCore
{
   ///A light scattering pixel shader for use with dtCore::Environment

   /** This shader is built into the Environment and is used by setting the
     * Environment::FogMode to ADV.
     */
   class DT_EXPORT SkyDomeShader
   {
   public:
      SkyDomeShader();
      ~SkyDomeShader();

      ///Update the shader with new values
      void Update(sgVec2 sunDir,
         float turbidity, float energy, float molecules);

      osg::VertexProgram *mLightScatterinVP;
      osg::FragmentProgram *mDomeFP;

   private:
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

#endif // DELTA_SKYDOMESHADER
