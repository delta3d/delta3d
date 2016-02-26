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

#ifndef DELTA_CLOUDPLANE
#define DELTA_CLOUDPLANE

#include <dtCore/enveffect.h>
#include <dtCore/refptr.h>

#include <osg/Array>
#include <osg/Vec2>
#include <osg/Vec4>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Fog;
   class Geode;
   class Geometry;
   class TexMat;
   class Texture2D;
   class Texture3D;
}
/// @endcond

namespace dtCore
{
   class MoveEarthySkyWithEyePointTransform;

   /**
    * This class can be used to simulate cloud cover it derives from EnvEffect.
    */
   class DT_CORE_EXPORT CloudPlane : public dtCore::EnvEffect
   {
   public:

       DECLARE_MANAGEMENT_LAYER(CloudPlane)

       /**
        * Constructor: These params initialize the noise function
        * for a detailed description see dtUtil::Fractal
        *
        * @sa dtUtil::Fractal
        */
       CloudPlane(int  octaves,
                  float cutoff,
                  int   frequency,
                  float amp,
                  float persistence,
                  float density,
                  int   texSize,
                  float height,
                  const std::string& name = "CloudPlane",
                  const std::string& textureFilePath = "");

       CloudPlane(float height, const std::string& name, const std::string& textureFilePath);

   protected:

      virtual ~CloudPlane();

   public:

      virtual void Repaint(const osg::Vec3& skyColor,
                           const osg::Vec3& fogColor,
                           double sunAngle,
                           double sunAzimuth,
                           double visibility);

      ///@return height of skyplane
      float GetHeight() { return mHeight; };

      ///Save generated texture to file
      ///@return success of save
      bool SaveTexture( const std::string& textureFilePath );

      ///Load generated texture from file
      ///@return success of load
      bool LoadTexture( const std::string& textureFilePath );

      ///Sets the color of the Cloud Plane
      void SetColor( const osg::Vec4& );

      ///Returns the color of the clouds
      ///@return current cloud color
      osg::Vec4 GetColor() const;

      ///Sets the Wind Vector (speed and direction) on the Cloud Plane
      void SetWind(osg::Vec2 wv);

      ///Sets the Wind Vector (speed and direction) on the Cloud Plane
      void SetWind(float x, float y);

      osg::Vec2 GetWind() const {return mWind;}

   private:

      void Create(const std::string& textureFilePath = "");

      void FilterTexture();

      void CreateCloudTexture(const std::string& filename);
      void createPerlinTexture();
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;
      void Update(const double deltaFrameTime);
      void UpdateColors(const osg::Vec4& fog, const osg::Vec4& black);
      osg::Geometry* createPlane(float, float);

      int mOctaves;
      float mCutoff;
      int   mFrequency;
      float mAmplitude;
      float mPersistence;
      float mDensity;
      float mHeight;
      int   mTexSize;
      osg::Vec2 mWind;
      osg::Vec4 mCloudColor;
      osg::Vec4 mSkyColor;
      osg::Vec2Array* mTexCoords;
      osg::Vec4Array* mColors;
      dtCore::RefPtr<osg::Texture2D> mCloudTexture;
      dtCore::RefPtr<MoveEarthySkyWithEyePointTransform> mXform;
      dtCore::RefPtr<osg::TexMat> mTexMat;
      dtCore::RefPtr<osg::Geode> mGeode;
      dtCore::RefPtr<osg::Geometry> mPlane;
      dtCore::RefPtr<osg::Fog> mFog; ///< The fog adjuster
   };
}

#endif // DELTA_CLOUDPLANE
