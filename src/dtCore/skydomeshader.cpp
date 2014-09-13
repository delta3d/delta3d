#include <prefix/dtcoreprefix.h>
#include <dtCore/skydomeshader.h>

namespace dtCore
{

SkyDomeShader::SkyDomeShader()
{
   mLightScatterinVP = new osg::VertexProgram();
   mDomeFP = new osg::FragmentProgram();

   char data[] =
      "!!ARBvp1.0                                              \n"
      "                                                        \n"
      "   PARAM mvp[4] = { state.matrix.mvp };                 \n"
      "   PARAM camera = program.local[0];                     \n"
      "   PARAM dir = program.local[1];                        \n"
      "   PARAM betaRay = program.local[2];                    \n"
      "   PARAM betaMie = program.local[3];                    \n"
      "                                                          \n"
      "   # Energy / (betaRay + betaMie)                       \n"
      "   PARAM energyOverRayMie = program.local[4];              \n"
      "                                                            \n"
      "   # G = greenstein                                         \n"
      "   # (-G * 2.0, G * G + 1.0, (1.0 - G) * (1.0 - G))  \n"
      "   PARAM greenstein = program.local[5];                  \n"
      "                                                        \n"
      "   PARAM time = program.local[6];                         \n"
      "                                                         \n"
      "   ATTRIB xyz = vertex.position;                        \n"
      "   ATTRIB texcoord = vertex.texcoord;                    \n"
      "   ATTRIB norm = vertex.normal;                         \n"
      "                                                         \n"
      "   DP4 result.position.x, mvp[0], xyz;                  \n"
      "   DP4 result.position.y, mvp[1], xyz;                  \n"
      "   DP4 result.position.z, mvp[2], xyz;                   \n"
      "   DP4 result.position.w, mvp[3], xyz;                \n"
      "                                                        \n"
      "   TEMP temp;                                  \n"
      "   SUB temp, camera, xyz;                      \n"
      "   DP3 temp.w, temp, temp;                      \n"
      "   RSQ temp.w, temp.w;                         \n"
      "   MUL temp, temp, temp.w;                        \n"
      "                                               \n"
      "   TEMP cos;                                         \n"
      "   DP3 cos.x, temp, dir;                          \n"
      "   MUL cos.y, cos.x, cos.x;   # cos * cos           \n"
      "   SUB cos.z, 1.0, cos.y;      # 1.0 - cos * cos       \n"
      "                                                      \n"
      "   TEMP ray;                                             \n"
      "   MUL ray, cos.z, 0.0597;      # 3.0 / (16.0 * PI)  \n"
      "   MUL ray, ray, betaRay;                            \n"
      "                                                     \n"
      "   TEMP mie;                                         \n"
      "   MAD mie, greenstein.x, cos.x, greenstein.y;       \n"
      "   MOV cos.w, 1.5;                                   \n"
      "   POW mie.x, mie.x, cos.w;                          \n"
      "   RCP mie.x, mie.x;                                 \n"
      "   MUL mie, mie.x, greenstein.z;                      \n"
      "   MUL mie, mie, 0.0796;      # 1.0 / (4.0 * PI)       \n"
      "   MUL mie, mie, betaMie;                            \n"
      "                                                     \n"
      "   SUB temp, camera, xyz;                         \n"
      "   DP3 temp.w, temp, temp;                           \n"
      "   RSQ temp.w, temp.w;                               \n"
      "   RCP temp.w, temp.w;      # distance to camera        \n"
      "                                                     \n"
      "   TEMP fog;                                       \n"
      "   ADD fog, betaRay, betaMie;                         \n"
      "   MUL fog, fog, temp.w;                                \n"
      "   MUL fog, fog, 0.693;   # ln(2.0)                  \n"
      "                                                     \n"
      "   EX2 fog.x, -fog.x;                                \n"
      "   EX2 fog.y, -fog.y;                                \n"
      "   EX2 fog.z, -fog.z;                                \n"
      "                                                     \n"
      "   SUB temp, 1.0, fog;      # 1.0 - fog                 \n"
      "                                                     \n"
      "   TEMP scattering;                                  \n"
      "   ADD scattering, ray, mie;                          \n"
      "   MUL scattering, scattering, energyOverRayMie;        \n"
      "   MUL scattering, scattering, temp;                  \n"
      "                                                        \n"
      "   MOV result.texcoord[0], texcoord;                    \n"
      "   MOV result.texcoord[1], scattering;                 \n"
      "   MOV result.texcoord[2], fog;                         \n"
      "   MOV result.texcoord[3], norm;                        \n"
      "   MOV result.color, vertex.color;                      \n"
      "   END                                                  \n";

   mLightScatterinVP->setVertexProgram(data);

   char data2[] =
      "!!ARBfp1.0                                     \n"
      "                                                \n"
      "   # brightness, contrast                      \n"
      "   PARAM bc = program.local[0];                 \n"
      "                                                \n"
      "   TEMP clouds;                                   \n"
      "   MOV clouds, fragment.color;                    \n"
      "                                                  \n"
      "   TEMP color;                                 \n"
      "   LRP color, fragment.texcoord[2], clouds, fragment.texcoord[1];  \n"
      "                           \n"
      "   TEMP temp; \n"
      "   SUB temp, color, 0.5;  \n"
      "   MAD color, temp, bc.y, color;   \n"
      "   ADD result.color, color, bc.x;  \n"
      "                                      \n"
      "   END  \n";
   mDomeFP->setFragmentProgram( data2 );

   lambda = osg::Vec3(1.0f/650e-9f, 1.0f/570e-9f, 1.0f/475e-9f);

   for (int i = 0; i < 3; i++)
   {
      lambda2[i] = lambda[i] * lambda[i];
      lambda4[i] = lambda2[i] * lambda2[i];
   }

   //constants
   n = 1.003f; //refractive index of air
   pn = 0.035f; //depolarization factor of air

   mBrightness = 0.125f;
   mContrast = 0.15f;

   greenstein = 0.8f * 1.5f; ///< Magic number
}

SkyDomeShader::~SkyDomeShader()
{
}

/** Update the shader with new values.
  *
  *
  *@param sunDir : The az/el of the sun in Degrees.
  *                sunDir[0] = az (0=north, 180=south)
  *                sunDir[1] = el (90=horizon, 0=zenith)
  *@param eyeXYZ : The Camera position (meters)
  *@param turbidity : The quality of the air (1.0 seems about right)
  *@param energy : The amount of energy passed to the atmosphere (10.0 is about right)
  *@param molecules: The number of molecules per unit volume (0 = fully opaque,
  *                  2.545e25 looks about right)
  */
void SkyDomeShader::Update(   const osg::Vec2& sunDir,
                              float turbidity,
                              float energy,
                              float molecules )
{
   osg::Vec3 sunVec; ///<To the sun, unit vector
   float cos_alt = cos(osg::DegreesToRadians(sunDir[1]));
   sunVec[0] = (sin(osg::DegreesToRadians(sunDir[0])) * cos_alt);
   sunVec[1] = (cos(osg::DegreesToRadians(sunDir[0])) * cos_alt);
   sunVec[2] = sin(osg::DegreesToRadians(sunDir[1]));

   mLightScatterinVP->setProgramLocalParameter(0, osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f) );
   mLightScatterinVP->setProgramLocalParameter(1, osg::Vec4( -sunVec[0], -sunVec[1], -sunVec[2], 0));

   float tempMie = 0.434 * ConcentrationFactor(turbidity) * osg::PI * (2 * osg::PI) * (2 * osg::PI) * 0.5;
   betaMie = osg::Vec3(lambda2[0] * 0.685,lambda2[1] * 0.679,lambda2[2] * 0.67) * tempMie;

   //Rayleigh scattering
   float tempRay = osg::PI * osg::PI * (n * n - 1.0) * (n * n - 1.0) * (6.0 + 3.0 * pn) / (6.0 - 7.0 * pn) / molecules;
   betaRay = lambda4 * 8.0 * tempRay * osg::PI / 3.0;

   mLightScatterinVP->setProgramLocalParameter(2, osg::Vec4(betaRay,0) );
   mLightScatterinVP->setProgramLocalParameter(3, osg::Vec4(betaMie, 0));
   mLightScatterinVP->setProgramLocalParameter(4, osg::Vec4(energy / (betaRay[0] + betaMie[0]),
                                                            energy / (betaRay[1] + betaMie[1]),
                                                            energy / (betaRay[2] + betaMie[2]),0));
   mLightScatterinVP->setProgramLocalParameter(5, osg::Vec4(-greenstein * 2.0,
                                                            greenstein * greenstein + 1.0,
                                                            (1.0 - greenstein) * (1.0 - greenstein),0));

   mDomeFP->setProgramLocalParameter(0, osg::Vec4( mBrightness, mContrast, 0, 0));
}

float SkyDomeShader::ConcentrationFactor(float turbidity)
{
   return (6.544 * turbidity - 6.51) * 1e-17; ///<more magic numbers
}

}
