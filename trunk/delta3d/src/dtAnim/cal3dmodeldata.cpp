/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 07/17/2007
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/cal3dloader.h>
#include <dtAnim/constants.h>
#include <dtAnim/animatable.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <osg/BufferObject>
#include <osgDB/FileNameUtils>

DT_DISABLE_WARNING_ALL_START
#include <cal3d/coremodel.h>
#include <cal3d/hardwaremodel.h>
DT_DISABLE_WARNING_END

#include <algorithm>

 namespace dtAnim
 {
   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::Cal3DModelData(const std::string& modelName, const dtCore::ResourceDescriptor& resource)
      : BaseClass(modelName, resource, Constants::CHARACTER_SYSTEM_CAL3D)
      , mCoreModel(NULL)
      , mHardwareModel(NULL)
   {
      mCoreModel = new CalCoreModel(modelName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::~Cal3DModelData()
   {
      delete mCoreModel;
      mCoreModel = NULL;

      if (mHardwareModel != NULL)
      {
         delete mHardwareModel;
         mHardwareModel = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::AddHotspot(const std::pair<dtUtil::HotSpotDefinition, std::string>& attachment)
   {
      mAttachments.push_back(attachment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::RemoveHotspot(unsigned int which)
   {
      mAttachments.erase(mAttachments.begin() + which);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CalCoreModel* Cal3DModelData::GetCoreModel()
   {
      return mCoreModel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const CalCoreModel* Cal3DModelData::GetCoreModel() const
   {
      return mCoreModel;
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   float Cal3DModelData::GetAnimationDuration(const std::string& name) const
   {
      int id = mCoreModel->getCoreAnimationId(name);

      return id < 0 ? 0.0f : mCoreModel->getCoreAnimation(id)->getDuration();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtAnim::AttachmentArray& Cal3DModelData::GetAttachments()
   {
      return mAttachments;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtAnim::AttachmentArray& Cal3DModelData::GetAttachments() const
   {
      return mAttachments;
   }

   /////////////////////////////////////////////////////////////////////////////
   CalHardwareModel* Cal3DModelData::GetCalHardwareModel()
   {
      return mHardwareModel;
   }

   /////////////////////////////////////////////////////////////////////////////
   CalHardwareModel* Cal3DModelData::GetOrCreateCalHardwareModel()
   {
      if (!mHardwareModel)
      {
         // We need a core model to make the hardware model
         if (mCoreModel)
         {
            mHardwareModel = new CalHardwareModel(mCoreModel);
         }
         else
         {
            LOG_ERROR("Unable to create hardware model.");
         }
      }

      return mHardwareModel;
   }

   int Cal3DModelData::LoadResource(dtAnim::ModelResourceType resourceType,
      const std::string& file, const std::string& objectName)
   {
      int result = -1;

      if (file.empty())
      {
         LOG_ERROR("No model resource file specified for object \"" + objectName
            + "\" (resource type " + BaseModelData::GetResourceTypeName(resourceType) + ").");
         return result;
      }

      switch (resourceType)
      {
      case dtAnim::ANIM_FILE:
         result = LoadCoreAnimation(file, objectName);
         break;

      case dtAnim::MAT_FILE:
         result = LoadCoreMaterial(file, objectName);
         break;

      case dtAnim::MESH_FILE:
         result = LoadCoreMesh(file, objectName);
         break;

      case dtAnim::SKEL_FILE:
   #if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
         result = LoadCoreSkeleton(file, objectName);
   #else
         result = LoadCoreSkeleton(file);
   #endif
         break;
         
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      case dtAnim::MORPH_FILE:
         result = LoadCoreMorph(file, objectName);
         break;
#endif

      default:
         break;
      }

      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName, resourceType);
      }

      return result;
   }

   int Cal3DModelData::UnloadResource(dtAnim::ModelResourceType resourceType, const std::string& objectName)
   {
      int result = 0;

      if (UnregisterObjectName(objectName, resourceType) > 0)
      {
         int id = -1;
         switch(resourceType)
         {
         case dtAnim::ANIM_FILE:
            id = mCoreModel->getCoreAnimationId(objectName);
            result = mCoreModel->unloadCoreAnimation(id);
            break;

         case dtAnim::MAT_FILE:
            id = mCoreModel->getCoreMaterialId(objectName);
            result = mCoreModel->unloadCoreMaterial(id);
            break;

         case dtAnim::MESH_FILE:
            id = mCoreModel->getCoreMeshId(objectName);
            result = mCoreModel->unloadCoreMesh(id);
            break;

         case dtAnim::MORPH_FILE:
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
            id = mCoreModel->getCoreAnimatedMorphId(objectName);
            result = mCoreModel->unloadCoreAnimatedMorph(id);
#endif
            break;

         default:
            break;
         }
      }

      return result;
   }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::LoadCoreSkeletonBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      bool success = mCoreModel->loadCoreSkeleton(buffer);
      if (success)
      {
         mCoreModel->getCoreSkeleton()->setName(objectName);
      }
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMorphBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      // Currently, the buffer overload for loading morphs does not exist.
      return LoadCoreMorph(file, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMaterialBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      return mCoreModel->loadCoreMaterial(buffer, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMeshBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      return mCoreModel->loadCoreMesh(buffer, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreAnimationBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      return mCoreModel->loadCoreAnimation(buffer, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMorph(const std::string& file, const std::string& objectName)
   {
      return mCoreModel->loadCoreAnimatedMorph(file);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::LoadCoreSkeleton(const std::string& file, const std::string& objectName)
   {
      bool success = mCoreModel->loadCoreSkeleton(file);
      if (success)
      {
         mCoreModel->getCoreSkeleton()->setName(objectName);
      }
      return success;
   }
#else

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::LoadCoreSkeleton(const std::string& file)
   {
      return mCoreModel->loadCoreSkeleton(file);
   }
#endif

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMaterial(const std::string& file, const std::string& objectName)
   {
      return mCoreModel->loadCoreMaterial(file, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMesh(const std::string& file, const std::string& objectName)
   {
      return mCoreModel->loadCoreMesh(file, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreAnimation(const std::string& file, const std::string& objectName)
   {
      return mCoreModel->loadCoreAnimation(file, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtAnim::ModelResourceType Cal3DModelData::GetFileType(const std::string& file) const
   {
      ModelResourceType fileType = NO_FILE;

      if (file.length() >= 4)
      {
         std::string ext = osgDB::getLowerCaseFileExtension(file);

         Cal3dExtensionEnum* calFileType = Cal3dExtensionEnum::GetValueForName(ext);
         if (calFileType == NULL)
         {
            return fileType;
         }

         // Determine the file type from the extension on the file.
         fileType = calFileType->GetResourceType();
      }

      return fileType;
   }

   int Cal3DModelData::GetIndexForObjectName(dtAnim::ModelResourceType fileType, const std::string& objectName) const
   {
      int id = -1;
      switch (fileType)
      {
      case MAT_FILE:
         id = mCoreModel->getCoreMaterialId(objectName);
         break;
      case MESH_FILE:
         id = mCoreModel->getCoreMeshId(objectName);
         break;
      case ANIM_FILE:
         id = mCoreModel->getCoreAnimationId(objectName);
         break;
      default: break;
      }

      return id;
   }

} //namespace dtAnim
