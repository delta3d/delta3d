/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 * Erik Johnson
 */

#ifndef __DELTA_CAL3DWRAPPER_H__
#define __DELTA_CAL3DWRAPPER_H__

#include <dtAnim/export.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <cal3d/renderer.h>
#include <cal3d/mixer.h>
#include <cal3d/morphtargetmixer.h>
#include <cal3d/physique.h>
#include <cal3d/springsystem.h>

#include <osg/Quat>                   // for return type
#include <osg/Referenced>
#include <osg/Vec4>
#include <osg/Vec3>

#include <vector>                     // for param type

namespace dtAnim
{
   /**
    * Wraps the Cal3D CalModel class. It is expected that users will use the 
    * Cal3DModelWrapper instead of using the CalModel class directly.
    * To create a new Cal3DModelWrapper:
    * @code
    * dtCore::RefPtr<Cal3DModelWrapper> wrap = new Cal3DModelWrapper( calModel );
    * @endcode
    */
   class DT_ANIM_EXPORT Cal3DModelWrapper : public osg::Referenced
   {
      public:
         static const int NULL_BONE = -1;
   
         Cal3DModelWrapper( CalModel *model );

         void SetCalModel( CalModel *model );

         /**
          * Get a pointer to the internal CalModel.  For advanced users only!
          * 
          * Warning! This violates the protective services brought to you by the wrapper.
          * Only modify the CalModel if you know how it will impact the rest of the 
          * Delta3D animation system.  You have been warned.
          * @return A pointer to the internal CalModel this class operates on.
          */
         CalModel* GetCalModel();

         /** 
          * Get a const pointer to the internal CalModel.  For advanced users only!
          * 
          * Warning! This violates the protective services brought to you by the wrapper.
          * Only modify the CalModel if you know how it will impact the rest of the 
          * Delta3D animation system.  You have been warned.
          * @return A const pointer to the internal CalModel this class operates on.
          */
         const CalModel* GetCalModel() const;

         bool AttachMesh( int meshID ); 
         bool DetachMesh( int meshID ); 

         void ShowMesh( int meshID ); 
         void HideMesh( int meshID ); 
         bool IsMeshVisible( int meshID ); 

         void SetMaterialSet(int materialSetID) { mCalModel->setMaterialSet(materialSetID); }
         void SetLODLevel(float level) { mCalModel->setLodLevel(level); }

         /************************************************************************/
         /// Update the Cal3D system using the CalModel's update.
         void Update(float deltaTime) {mCalModel->update(deltaTime); }

         /// Update just the Cal3D's animation using the mixer
         void UpdateAnimation(float deltaTime) {mCalModel->getMixer()->updateAnimation(deltaTime);}

         /// Update just Cal3D's skeleton using the mixer
         void UpdateSkeleton()                 {mCalModel->getMixer()->updateSkeleton();}

         /// Update the CalModel's morph target mixer
         void UpdateMorphTargetMixer(float deltaTime)  {mCalModel->getMorphTargetMixer()->update(deltaTime);}

         /// Update the CalModel's physique
         void UpdatePhysique()                 {mCalModel->getPhysique()->update();}

         /// Update the CalModel's spring system
         void UpdateSpringSystem(float deltaTime)      {mCalModel->getSpringSystem()->update(deltaTime);}

         /************************************************************************/
         bool BeginRenderingQuery() { return mRenderer->beginRendering(); } 
         void EndRenderingQuery()   { mRenderer->endRendering(); }

         bool SelectMeshSubmesh(int meshID, int submeshID) {return mRenderer->selectMeshSubmesh(meshID, submeshID);}

         int GetVertexCount() { return mRenderer->getVertexCount(); }
         int GetFaceCount()   { return mRenderer->getFaceCount(); }
         int GetMapCount()    { return mRenderer->getMapCount(); }
         int GetMeshCount()                 { return mRenderer->getMeshCount(); }
         int GetSubmeshCount(int submeshID) { return mRenderer->getSubmeshCount(submeshID); }

         /************************************************************************/
         int GetCoreMeshCount() const { return mCalModel->getCoreModel()->getCoreMeshCount(); }

         ///Get the name for the mesh using the supplied meshID
         const std::string& GetCoreMeshName(int meshID) const;

         int GetCoreAnimationCount() const;

         /// Get the Cal3D rotation values.
         /// @param animid the core animation of interest.
         /// @param boneid the core bone within the animation, identifying the track.
         /// @param keyframeindex the keyframe array index of interest for the animation track.
         /// @return the rotation values that cal3d is using, converted into right hand coordinate frame.
         osg::Quat GetCoreTrackKeyFrameQuat(unsigned int animID, unsigned int boneID, unsigned int keyframeindex) const;

         /// @param boneID the ID for the CalBone instance.
         osg::Quat GetBoneAbsoluteRotation(unsigned int boneID) const;

         /** 
          * Get the Cal3D rotation values.
          * @param animid the core animation of interest.
          * @param boneid the core bone within the animation, identifying the track.
          * @param keyframeindex the keyframe array index of interest for the animation track.
          * @return the rotation values that cal3d is using, converted into right hand coordinate frame.
          */
         osg::Quat GetBoneAbsoluteRotationForKeyFrame(int animid, int boneid, unsigned int keyframeindex) const;

         /// Get the current translation for the CalBone.
         /// @param boneID the ID for the CalBone of interest.
         /// @return the translation vector in a right-hand coordinate system.
         osg::Vec3 GetBoneAbsoluteTranslation(unsigned int boneID) const;

         /// @param boneID the ID for the CalBone instance.
         osg::Quat GetBoneRelativeRotation(unsigned int boneID) const;

         /// Get the Cal3D CoreBone ID
         /// @return the ID for the CoreBone.
         int GetCoreBoneID(const std::string& name) const;

         /// Get all bone IDs within the CoreSkeleton.
         /// @param toFill a vector to be filled with the bone IDs for the entire CoreSkeleton.
         void GetCoreBoneNames(std::vector<std::string>& toFill) const;

         ///Get the name that equates to the supplied animation ID
         const std::string& GetCoreAnimationName(int animID) const;

         int GetCoreAnimationIDByName(const std::string &name) const;

         ///Get the number of tracks this animation uses
         unsigned int GetCoreAnimationTrackCount(int animID) const;

         ///Get the id of the parent to boneID
         int GetParentBoneID(unsigned int boneID) const;

         /// Get the bone IDs of all children for a parent bone.
         /// @param parentCoreBoneID the bone ID for the parent bone.
         /// @param toFill a vector to be filled with the bone IDs for all child bones.
         void GetCoreBoneChildrenIDs(int parentCoreBoneID, std::vector<int>& toFill) const;

         /// Get all the root bone IDs
         /// @param toFill a container of bone IDs consisting only of root bones.
         void GetRootBoneIDs(std::vector<int>& toFill) const;

         ///Get the total number of keyframes in this animation
         unsigned int GetCoreAnimationKeyframeCount(int animID) const;

         ///Get the number of keyframes in the animation for a particular bone
         unsigned int GetCoreAnimationKeyframeCountForTrack(int animID, int boneID) const;

         ///Get the duration of this animation (seconds?)
         float GetCoreAnimationDuration(int animID) const;

         int GetCoreMaterialCount() const {return mCalModel->getCoreModel()->getCoreMaterialCount();}
         CalCoreMaterial* GetCoreMaterial(int matID) {return mCalModel->getCoreModel()->getCoreMaterial(matID); }

         ///Get the core material diffuse color (rgba 0-255)
         osg::Vec4 GetCoreMaterialDiffuse(int matID) const;

         ///Get the core material ambient color (rgba 0-255)
         osg::Vec4 GetCoreMaterialAmbient(int matID) const;

         ///Get the core material specular color (rgba 0-255)
         osg::Vec4 GetCoreMaterialSpecular(int matID) const;

         ///Get the core material shininess
         float     GetCoreMaterialShininess(int matID) const;

         ///Get the name associated with the material using the supplied material ID
         const std::string& GetCoreMaterialName(int matID) const {return mCalModel->getCoreModel()->getCoreMaterial(matID)->getName(); }

         /************************************************************************/
         int GetFaces(int *faces)          { return mRenderer->getFaces( (CalIndex*)faces ); }
         int GetNormals(float *normals, int stride=0) { return mRenderer->getNormals(normals, stride); }
         int GetTextureCoords(int mapID, float *coords, int stride=0) {return mRenderer->getTextureCoordinates(mapID, coords, stride);}
         int GetVertices(float *vertBuffer, int stride=0) {return mRenderer->getVertices(vertBuffer, stride);}

         /************************************************************************/
         void GetAmbientColor( unsigned char *colorBuffer ) {mRenderer->getAmbientColor(colorBuffer);}
         void GetDiffuseColor( unsigned char *colorBuffer ) {mRenderer->getDiffuseColor(colorBuffer);}
         void GetSpecularColor( unsigned char *colorBuffer ) {mRenderer->getSpecularColor(colorBuffer);}
         float GetShininess() { return mRenderer->getShininess(); }
         void* GetMapUserData(int mapID) { return mRenderer->getMapUserData(mapID); }


         /************************************************************************/

         bool HasTrackForBone(unsigned int animID, int boneID) const;

         bool HasBone(int boneID) const;

         bool HasAnimation(int animID) const;

         /** 
          * @param id : a valid ID of an animation (0 based)
          * @param weight : the strength of this animation in relation to the other
          *                 animations already being blended.
          * @param delay : how long it takes for this animation to become full strength (seconds)
          * @return true if successful, false if an error happened.
          */
         bool BlendCycle(int id, float weight, float delay);

         /** 
          * @param id : a valid ID of an animation already being blended (0 based)
          * @param delay : how long it takes to fade this animation out (seconds)
          * @return true if successful, false if an error happened.
          */
         bool ClearCycle(int id, float delay);

         /// Remove all existing animations from the mixer
         void ClearAll();

         /** 
          * Peform a one time animation.
          * 
          * @param id : a valid ID of a animation to perform one-time (0 based)
          * @param delayIn : how long it takes to fade in this animation to full strength (seconds)
          * @param delayOut: how long it takes to fade out this animation (seconds)
          * @param weightTgt : the strength of this animation
          * @param autoLock : true prevents the action from being reset and removed on the last
          *                   key frame
          * @return true if successful, false if an error happened.
          */
         bool ExecuteAction(int id, float delayIn, float delayOut,
                            float weightTgt=1.0f, bool autoLock=false);

         /** 
          * Remove an existing one-time animation from the mixer
          * 
          * @param id : a valid ID of a one-time animation already playing (0 based)
          * @return true if successful, false if an error happened or animation doesn't exist.
          */
         bool RemoveAction(int id);

         /// sets the offset time used in synchronized looping animations.
         void SetAnimationTime(float time);
         /// @return the offset time used when playing looping animations.
         float GetAnimationTime();

      protected:
         virtual ~Cal3DModelWrapper();

      private:
         CalModel    *mCalModel;
         CalRenderer *mRenderer;
         CalMixer    *mMixer;

         typedef std::map<int, bool> MeshVisibilityMap;
         MeshVisibilityMap mMeshVisibility;
   };
}//namespace dtAnim

#endif // __DELTA_CAL3DWRAPPER_H__
