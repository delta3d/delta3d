
#include <osg/Vec3>
#include <osg/VertexProgram>
#include <osg/FragmentProgram>
#include "sg.h"

namespace dtCore
{
   ///A light scattering pixel shader for use with dtCore::Environment

   /** This shader is built into the Environment and is used by setting the
     * Environment::FogMode to ADV.
     */
   class SunlightShader
   {
   public:
      SunlightShader();
      ~SunlightShader();

      ///Update the shader with new values
      void Update(sgVec2 sunDir, sgVec3 eyeXYZ,
         float turbidity, float energy, float molecules);

      osg::VertexProgram *mLightScatterinVP;
      osg::FragmentProgram *mTerrainFP;

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
