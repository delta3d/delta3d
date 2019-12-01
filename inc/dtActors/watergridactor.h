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
#include <dtCore/plugin_export.h>

#include <dtUtil/refstring.h>
#include <vector>
#include <dtCore/camera.h>

#include <dtCore/propertymacros.h>

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
      // Chop settings are used to cycle between some basic versions of the waves.
      class DT_PLUGIN_EXPORT ChopSettings : public dtUtil::Enumeration
      {
         DECLARE_ENUM(ChopSettings);
      public:
         static ChopSettings CHOP_FLAT;
         static ChopSettings CHOP_MILD;
         static ChopSettings CHOP_MED;
         static ChopSettings CHOP_ROUGH;

         float mRotationSpread;
         float mTextureWaveModifier;

      private:
         //////////////////////////////////////////////////////////////////////////
         ChopSettings(const std::string& name, float rotationSpread, float texMod);
      };


      //////////////////////////////////////////////
      // Sea State Settings control the significant wave height
      // they are based on the Beaufort Wind Force Scale and Sea State
      // which is enumerated from 1-12 with 1 being a calm sea and 12 being a hurricane
      // see: http://www.seakayak.ws/kayak/kayak.nsf/8db4c87cad13b187852569ff0050c911/e4e2c690916a3a24852570da0057e036!OpenDocument
      class DT_PLUGIN_EXPORT SeaState : public dtUtil::Enumeration
      {
         DECLARE_ENUM(SeaState);
      public:
         static SeaState SeaState_0;
         static SeaState SeaState_1;
         static SeaState SeaState_2;
         static SeaState SeaState_3;
         static SeaState SeaState_4;
         static SeaState SeaState_5;
         static SeaState SeaState_6;
         static SeaState SeaState_7;
         static SeaState SeaState_8;
         static SeaState SeaState_9;
         static SeaState SeaState_10;
         static SeaState SeaState_11;
         static SeaState SeaState_12;


         // These attributes are public since this is essentially a struct.
         float mAmplitudeModifier;
         float mWaveLengthModifier;
         float mSpeedModifier;

      private:
         //////////////////////////////////////////////
         SeaState(const std::string& name, float ampMod, float waveLenMod, float speedMod);
      };

      //////////////////////////////////////////////
      static const int MAX_WAVES = 32;
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
      static const dtUtil::RefString UNIFORM_WATER_HEIGHT;
      static const dtUtil::RefString UNIFORM_CENTER_OFFSET;
      static const dtUtil::RefString UNIFORM_WAVE_DIRECTION;
      static const dtUtil::RefString UNIFORM_WATER_COLOR;
      static const dtUtil::RefString UNIFORM_UNDERWATER_VIEW_DISTANCE;      
      static const dtUtil::RefString UNIFORM_TEXWAVE_RESOLUTION_SCALAR;
      static const dtUtil::RefString UNIFORM_TEXWAVE_AMPLITUDE_SCALAR;
      static const dtUtil::RefString UNIFORM_TEXWAVE_SPREAD_SCALAR;
      static const dtUtil::RefString UNIFORM_TEXWAVE_STEEPNESS;
      
   public:
      WaterGridActor(WaterGridActorProxy& proxy);

      /*virtual*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);
      /*virtual*/ void OnEnteredWorld();

      void SetWaterColor(const osg::Vec4& color);
      osg::Vec4 GetWaterColor() const;

      void SetChop(ChopSettings& choppiness);
      ChopSettings& GetChop() const;

      //this alternatively sets it by number, valid ranges from 0-12, see comment on SeaStateEnum above
      void SetSeaState(SeaState& choppiness);
      SeaState& GetSeaState() const;
      void SetSeaStateByNumber(unsigned force);

      void SetEnableWater(bool enable);

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


      void ClearWaves();
      void AddRandomizedWaves(float meanWaveLength, float meanAmplitude, float minPeriod, float maxPeriod, unsigned numWaves);

      // Modulation of the base wave structures.
      DT_DECLARE_ACCESSOR_INLINE(float, ModForWaveLength)
      DT_DECLARE_ACCESSOR_INLINE(float, ModForSpeed)
      DT_DECLARE_ACCESSOR_INLINE(float, ModForSteepness)         
      DT_DECLARE_ACCESSOR_INLINE(float, ModForAmplitude)
      DT_DECLARE_ACCESSOR_INLINE(float, ModForDirectionInDegrees)
      DT_DECLARE_ACCESSOR_INLINE(float, UnderWaterViewDistance)

      DT_DECLARE_ACCESSOR_INLINE(int, NumRows)
      DT_DECLARE_ACCESSOR_INLINE(int, NumColumns)
      DT_DECLARE_ACCESSOR_INLINE(bool, UseDebugKeys)
      
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, ReflectionMapResolution)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, TexWaveTextureResolution)
      DT_DECLARE_ACCESSOR_INLINE(float, TexWaveResolutionScalar)
      DT_DECLARE_ACCESSOR_INLINE(float, TexWaveAmpScalar)
      DT_DECLARE_ACCESSOR_INLINE(float, TexWaveAmpInternal)
      DT_DECLARE_ACCESSOR_INLINE(float, TexWaveSpreadScalar)
      DT_DECLARE_ACCESSOR_INLINE(float, TexWaveSteepness)

   protected:

      ~WaterGridActor();

      void Update(float dt);
      void CreateGeometry();
      void UpdateWaveUniforms(dtCore::Camera& pCamera);
      osg::Vec3 GetPosition();
      osg::Node* BuildSubmergedGeometry();
      void CreateWaveTexture();
      void CreateReflectionCamera();
      void CreateFoamTexture();
      void CreateNoiseTexture();

      void BindTextureUniformToNode(osg::Node*, osg::Texture2D*, const std::string& name, unsigned texUnit);
      void InitAndBindToTarget(osg::Camera*, osg::Texture2D*, int width, int height, bool mipmap);
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
      float mFarDistanceBetweenVerts;
      float mNearDistanceBetweenVerts;
      float mTextureWaveAmpOverLength;

      
      float mModForFOV;
      float mCameraFoVScalar; // changes wave detail based on how much FoV is visible

      osg::Vec4 mWaterColor;
      osg::Vec3 mLastCameraOffsetPos, mCurrentCameraPos;

      ChopSettings* mChopEnum;
      SeaState* mSeaStateEnum;

      // Each frame (or as needed), the current set of waves is computed. This data
      // is pulled from the mWaves list based on the current camera height and wavelengths
      // The data is put in mProcessedWaveData and used for ocean shaders and ground clamping
      // Order is: waveLength, speed, amp, freq, steepness, UNUSED, dirX, dirY
      float mProcessedWaveData[MAX_WAVES][8];


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

      void CreateDrawable();

      void BuildPropertyMap();
      void BuildInvokables();

      void OnEnteredWorld();

      /**
      *  Called when this BaseActorObject has finished loading from a Map and
      *  all Property values have been assigned.  Overwrite for custom
      *  behavior.
      */
      void OnMapLoadEnd();

      dtCore::ActorProxyIcon* GetBillBoardIcon();
      /*virtual*/ const dtCore::BaseActorObject::RenderMode& GetRenderMode();

      void SetSceneCamera(const std::string& name); 
      std::string GetSceneCamera() const;

      void ResetSceneCamera();

      DT_DECLARE_ACCESSOR(float, WaveDirection);
      DT_DECLARE_ACCESSOR(float, AmplitudeModifier);
      DT_DECLARE_ACCESSOR(float, WavelengthModifier);
      DT_DECLARE_ACCESSOR(float, SpeedModifier);

   protected:

      ~WaterGridActorProxy();

      std::string mSceneCameraName;
   };
}

#endif//__WATERGRIDACTOR_H__
