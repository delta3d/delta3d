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

#ifndef DELTA_CLOUDDOME
#define DELTA_CLOUDDOME

#include <dtCore/enveffect.h>
#include <dtCore/refptr.h>

#include <osg/Vec3>
#include <osg/Vec4>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Geometry;
   class Image;
   class Program;
   class ShaderProgram;
   class Texture3D;
}
/// @endcond

namespace dtCore
{
   class MoveEarthySkyWithEyePointTransform;

   /**
    * CloudDome:  This class can be used to generate procedural cloud cover
    *
    */
   class DT_CORE_EXPORT CloudDome : public dtCore::EnvEffect
   {

   public:
      DECLARE_MANAGEMENT_LAYER(CloudDome)

      /**
       * Constructor: These params initialize the noise function
       * for a detailed description see dtUtil::Fractal
       *
       * @sa dtUtil::Fractal
       */
      CloudDome(int   octaves,
                int   frequency,
                float amp,
                float persistence,
                float cutoff,
                float exponent,
                float radius,
                int   segments);

      /**
       * Constructor
       * @param radius: the radius of the cloud dome
       * @param segments: the number of segments in the dome tesselation
       */
      CloudDome(float radius,
                int   segments,
                const std::string& filename);

   protected:

      virtual ~CloudDome();

   public:

      float GetScale() const { return mScale; }
      float GetExponent() const { return mExponent; }
      float GetCutoff() const { return mCutoff; }
      float GetSpeedX() const { return mSpeedX; }
      float GetSpeedY() const { return mSpeedY; }
      float GetBias() const { return mBias; }
      osg::Vec3 GetCloudColor() const { return mCloudColor; } //return-by-value for dtCore propety types
      bool GetEnable() const { return mEnable; }

      void SetScale(float scale) { mScale = scale; }
      void SetExponent(float exponent) { mExponent = exponent; }
      void SetCutoff(float cutoff) { mCutoff = cutoff; }
      void SetSpeedX(float speedX) { mSpeedX = speedX; }
      void SetSpeedY(float speedY) { mSpeedY = speedY; }
      void SetBias(float bias) { mBias = bias; }
      void SetCloudColor(const osg::Vec3& mCC) { mCloudColor = mCC; }
      void SetShaderEnable(bool enable) { mEnable = enable; }

      virtual void Repaint(const osg::Vec3& skyColor,
                           const osg::Vec3& fogColor,
                           double sunAngle,
                           double sunAzimuth,
                           double visibility);

   private:

      void Create();
      osg::Geode* CreateDome(float radius, int segs);
      void LoadShaderSource(osg::Shader* obj, const std::string& fileName);
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;
      void Update(const double deltaFrameTime);

      dtCore::RefPtr<osg::Geode> mDome;
      dtCore::RefPtr<osg::Image> mImage_3D;
      dtCore::RefPtr<osg::Texture3D> mTex3D;
      std::string mFileName;
      osg::Vec3 mFogColor;

      int mOctaves;
      int mFrequency;
      float mPersistence;
      float mAmplitude;

      dtCore::RefPtr<MoveEarthySkyWithEyePointTransform> mXform;
      double ctime;
      bool mEnable;
      bool shaders_enabled;
      float mRadius;
      int mSegments;

      std::vector< dtCore::RefPtr<osg::Program> >     mProgList;
      dtCore::RefPtr<osg::Program>                    mCloudProg;
      dtCore::RefPtr<osg::Shader>                     mCloudVert;
      dtCore::RefPtr<osg::Shader>                     mCloudFrag;

      // Uniform variables for shaders
      float mScale;
      float mExponent;
      float mCutoff;
      float mSpeedX;
      float mSpeedY;
      float mBias;
      osg::Vec3 mCloudColor;
      osg::Vec3 mOffset;


   };
}

#endif // DELTA_CLOUDDOME
