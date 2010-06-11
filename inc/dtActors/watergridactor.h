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
* @author Bradley Anderegg
* @author Curtiss Murphy
*/

#ifndef __WATERGRIDACTOR_H__
#define __WATERGRIDACTOR_H__

#include <dtActors/basewateractor.h>
#include <dtDAL/plugin_export.h>

#include <dtUtil/refstring.h>
#include <vector>
#include <dtCore/camera.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Texture2D>
#include <osg/Texture3D>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Camera;
}
/// @endcond

namespace dtActors
{
   class WaterGridActorProxy;

   ////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT WaterGridActor: public BaseWaterActor
   {
   public:
      typedef BaseWaterActor BaseClass;

      struct Wave
      {
         float mWaveLength;
         float mSpeed;
         float mSteepness;
         osg::Vec2 mDirection; // This is computed from mDirectionInDegrees
         float mDirectionInDegrees;

         float mFreq;
         float mAmplitude;
      };

      struct TextureWave
      {
         float mWaveLength;
         float mSteepness;
         float mSpeed;
      };

      typedef std::vector<Wave> WaveArray;
      typedef std::vector<TextureWave> TextureWaveArray;

      //////////////////////////////////////////////
      // Choppiness settings are used to cycle between some basic versions of the waves.
      class DT_PLUGIN_EXPORT ChoppinessSettings : public dtUtil::Enumeration
      {
         DECLARE_ENUM(ChoppinessSettings);
      public:
         static ChoppinessSettings CHOP_FLAT;
         static ChoppinessSettings CHOP_MILD;
         static ChoppinessSettings CHOP_MED;
         static ChoppinessSettings CHOP_ROUGH;

         // These attributes are public since this is essentially a struct.
         std::string name;
         float mSpeedMod;
         float mRotationSpread;
         float mWaveLengthModifier;
         float mAmpModifier;
         float mTextureWaveModifier;

      private:
         //////////////////////////////////////////////////////////////////////////
         ChoppinessSettings(const std::string &name,  float speedMod,
            float rotationSpread, float waveLengthModifier, float ampModifier, float texMod)
               : dtUtil::Enumeration(name) , mSpeedMod(speedMod) , mRotationSpread(rotationSpread)
               , mWaveLengthModifier(waveLengthModifier) , mAmpModifier(ampModifier), mTextureWaveModifier(texMod)
         { 
            AddInstance(this);
         }
      };

      //////////////////////////////////////////////
      static const int MAX_WAVES = 8;
      static const int MAX_TEXTURE_WAVES;
      static const dtUtil::RefString UNIFORM_ELAPSED_TIME;
      static const dtUtil::RefString UNIFORM_MAX_COMPUTED_DISTANCE;
      static const dtUtil::RefString UNIFORM_WAVE_ARRAY;
      static const dtUtil::RefString UNIFORM_TEXTURE_WAVE_ARRAY;
      static const dtUtil::RefString UNIFORM_REFLECTION_MAP;
      static const dtUtil::RefString UNIFORM_NOISE_TEXTURE;
      static const dtUtil::RefString UNIFORM_WAVE_TEXTURE;
      static const dtUtil::RefString UNIFORM_SCREEN_WIDTH;
      static const dtUtil::RefString UNIFORM_SCREEN_HEIGHT;
      static const dtUtil::RefString UNIFORM_TEXTURE_WAVE_AMP;
      static const dtUtil::RefString UNIFORM_WATER_HEIGHT;
      static const dtUtil::RefString UNIFORM_CENTER_OFFSET;
      static const dtUtil::RefString UNIFORM_WAVE_DIRECTION;
      static const dtUtil::RefString UNIFORM_WATER_COLOR;

   public:
      WaterGridActor(WaterGridActorProxy& proxy);

      /*virtual*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);
      /*virtual*/ void OnEnteredWorld();

      void SetWaterColor(const osg::Vec4& color);
      osg::Vec4 GetWaterColor() const;

      void SetChoppiness(WaterGridActor::ChoppinessSettings& choppiness);
      WaterGridActor::ChoppinessSettings& GetChoppiness() const;

      void SetEnableWater(bool enable);

      void SetTextureWaveAmpOverLength(float ampOverLength);

      // Reflection related functions
      void SetReflectionScene(osg::Node* sceneNode);
      void ResetReflectionUpdate();

      void SetSceneCamera(dtCore::Camera* sceneCamera);

      //init is called on map loaded
      void Init(const dtGame::Message&);
      void Initialize();

      /**
      * Get the world-space surface height and normal at a specified detection point,
      * for all combined waves that make up the water surface.
      * @param detectionPoint Point from which to detect the world-space height of
      *        the water surface from below or above the point.
      * @param outHeight Metric height of the water surface at the specified point.
      * @param outNormal Normal of the water surface at the specified point.
      * @return TRUE if a point was detected, otherwise FALSE.
      */
      virtual bool GetHeightAndNormalAtPoint(const osg::Vec3& detectionPoint,
         float& outHeight, osg::Vec3& outNormal) const;

   protected:

      ~WaterGridActor();

      void Update(float dt);
      void CreateGeometry();
      void UpdateWaveUniforms(dtCore::Camera& pCamera);
      osg::Vec3 GetPosition();
      osg::Node* BuildSubmergedGeometry();
      void CreateWaveTexture();
      void CreateReflectionCamera();

      void BindTextureUniformToNode(osg::Node*, osg::Texture2D*, const std::string& name, unsigned texUnit);
      void InitAndBindToTarget(osg::Camera*, osg::Texture2D*, int width, int height);
      void AddOrthoQuad(osg::Camera*, osg::Texture2D*, const std::string& shader, const std::string& texUniform);
      void BindShader(osg::Node* node, const std::string& shaderName);

      void UpdateWaterPlaneFOV(dtCore::Camera& pCamera, const osg::Matrix& MVP);
      void UpdateScreenSpaceWaterHeight(dtCore::Camera& pCamera, const osg::Matrix& inverseMVP);
      void ComputeRay(int x, int y, const osg::Matrix& inverseMVPS, osg::Vec3& rayToFill);
      bool IntersectRayPlane(const osg::Vec4& plane, const osg::Vec3& rayOrigin, const osg::Vec3& rayDirection, osg::Vec3& intersectPoint);
      float GetAngleBetweenVectors(const osg::Vec3& v1, const osg::Vec3& v2);

      void SetRenderWaveTexture(bool b);

      void AddReflectionGroup(osg::Camera* cam);

      // Loops through the total wave set and determines which N waves will be marked as the 'current'.
      // This method populates mProcessedWaveData as well as sets mCameraFoVScalar.
      void DetermineCurrentWaveSet(dtCore::Camera& pCamera);

      void UpdateViewMatrix(dtCore::Camera& pCamera);

      friend class WaterGridActorProxy;

   private:

      float mElapsedTime;
      float mDeltaTime;
      bool  mRenderWaveTexture, mWireframe, mDeveloperMode;
      float mComputedRadialDistance;
      float mTextureWaveAmpOverLength;

      // Modulation of the base wave structures.
      float mModForWaveLength;
      float mModForSpeed;
      float mModForSteepness;
      float mModForAmplitude;
      float mModForDirectionInDegrees;
      float mModForFOV;
      float mCameraFoVScalar; // changes wave detail based on how much FoV is visible

      // Each frame (or as needed), the current set of waves is computed. This data
      // is pulled from the mWaves list based on the current camera height and wavelengths
      // The data is put in mProcessedWaveData and used for ocean shaders and ground clamping
      // Order is: waveLength, speed, amp, freq, steepness, UNUSED, dirX, dirY
      float mProcessedWaveData[MAX_WAVES][8];

      osg::Vec4 mWaterColor;
      osg::Vec3 mLastCameraOffsetPos, mCurrentCameraPos;

      ChoppinessSettings* mChoppinessEnum;

      osg::ref_ptr<osg::Camera>   mWaveCamera;
      osg::ref_ptr<osg::Camera>   mWaveCameraScreen;
      osg::ref_ptr<osg::Geometry> mGeometry;
      osg::ref_ptr<osg::Geode>	 mGeode;

      osg::observer_ptr<osg::Camera> mSceneCamera;

      osg::ref_ptr<osg::Camera>          mReflectionCamera;
      osg::ref_ptr<osg::MatrixTransform> mReflectionGroup;
      osg::observer_ptr<osg::Node>       mReflectionScene;

      osg::ref_ptr<osg::Texture2D> mReflectionTexture;
      osg::ref_ptr<osg::Texture3D> mNoiseTexture;
      osg::ref_ptr<osg::Texture2D> mWaveTexture;
      osg::observer_ptr<dtGame::GameActorProxy> mOceanDataProxy;

      WaveArray mWaves;
      TextureWaveArray mTextureWaves;
   };


   //////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT WaterGridActorProxy: public BaseWaterActorProxy
   {
   public:
      typedef BaseWaterActorProxy BaseClass;

      static const dtUtil::RefString CLASSNAME;
      static const dtUtil::RefString PROPERTY_CHOPPINESS;
      static const dtUtil::RefString PROPERTY_WATER_COLOR;
      static const dtUtil::RefString PROPERTY_SCENE_CAMERA;
      static const dtUtil::RefString INVOKABLE_MAP_LOADED;
      static const dtUtil::RefString INVOKABLE_ACTOR_CREATED;
      static const dtUtil::RefString INVOKABLE_ACTOR_UPDATE;

      WaterGridActorProxy();

      void CreateActor();

      void BuildPropertyMap();
      void BuildInvokables();

      void OnEnteredWorld();

      /**
      *  Called when this ActorProxy has finished loading from a Map and
      *  all Property values have been assigned.  Overwrite for custom
      *  behavior.
      */
      void OnMapLoadEnd();

      dtDAL::ActorProxyIcon* GetBillBoardIcon();
      /*virtual*/ const dtDAL::ActorProxy::RenderMode& GetRenderMode();

      void SetSceneCamera(const std::string& name); 
      std::string GetSceneCamera() const;

      void ResetSceneCamera();

   protected:

      ~WaterGridActorProxy();

      std::string mSceneCameraName;
   };
}

#endif//__WATERGRIDACTOR_H__
